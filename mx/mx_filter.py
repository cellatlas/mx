import numpy as np
import pandas as pd
from scipy.io import mmread, mmwrite
from scipy.stats import entropy
from sklearn.mixture import GaussianMixture
from scipy.sparse import csr_matrix
from .utils import nd, read_str_list, write_list


def knee(mtx, sum_axis):
    u = nd(mtx.sum(sum_axis))  # counts per barcode
    x = np.sort(u)[::-1]  # sorted
    v = np.log1p(x).reshape(-1, 1)  # log1p and reshaped for gmm
    return (u, x, v)


def knee_select(mtx, select_axis):
    u = nd(mtx[:, select_axis])  # counts per barcode
    x = np.sort(u)[::-1]  # sorted
    v = np.log1p(x).reshape(-1, 1)  # log1p and reshaped for gmm
    return (u, x, v)


def gmm(x, v, comps):
    n_comps = comps.pop(0)

    gm = GaussianMixture(n_components=n_comps, random_state=42)
    labels = gm.fit_predict(v)
    prob = gm.predict_proba(v)
    ent = entropy(prob, axis=1)

    # index of v where low count cell is
    cutoff = 0
    if n_comps == 2:
        ind = np.argmax(ent)
        # log1p_cutoff = v[ind][0]
        cutoff = x[ind]
    elif n_comps > 2:
        # sort means, and pick the range of the top two
        means = np.sort((np.exp(gm.means_) - 1).flatten())
        r = np.logical_and(x > means[-2], x < means[-1])  # make ranage
        df = pd.DataFrame({"ent": ent, "idx": np.arange(ent.shape[0]).astype(int)})[r]
        # get the index (of x) where the entropy is the max (in range r)
        amax = df["ent"].argmax()
        idx = df.iloc[amax]["idx"].astype(int)
        cutoff = x[idx]

    # n_iter -= 1
    n_iter = len(comps)
    if n_iter <= 0:
        return (cutoff, (x > cutoff).sum())
    return gmm(x[x > cutoff], v[x > cutoff], comps)  # , n_comps, n_iter)


def mx_filter(
    matrix_fn,
    barcodes_fn,
    matrix_fn_out,
    barcodes_fn_out,
    sum_axis=1,
    comps=[2],
    select_axis=None,  # if you want to do the knee only on certain columns
):
    # read matrix
    mtx = mmread(matrix_fn).toarray()

    # read barcodes
    barcodes = []
    read_str_list(barcodes_fn, barcodes)

    # find knee
    # check this, do it twice?
    u, x, v = knee(mtx, sum_axis)
    if select_axis:
        u, x, v = knee_select(mtx, select_axis)

    (cutoff, ncells) = gmm(x, v, comps=comps)

    print(f"Filtered to {ncells:,.0f} cells with at least {cutoff:,.0f} UMIs.")

    # mask matrix and netadata
    mask = u > cutoff
    mtx_f = mtx[mask]
    barcodes_f = np.array(barcodes)[mask]

    # save filtered matrix
    mmwrite(matrix_fn_out, csr_matrix(mtx_f))

    # save filtered metadata
    write_list(barcodes_fn_out, barcodes_f)
