from scipy.stats import ttest_ind, rankdata
import numpy as np
import pandas as pd
import logging
import sys
from scipy.io import mmread

from mx.utils import read_str_list

logger = logging.getLogger()
logger.setLevel(logging.INFO)
logging.basicConfig(format="%(asctime)s - %(message)s", datefmt="%d-%b-%y %H:%M:%S")


def split_by_target(mat, targets, target, axis=0):
    """
    Split the rows of mat by the proper assignment

    mat = ndarray
    targets, length is equal to number of components (axis=0) or features (axis=1)
    target is a singular element from unique(assignments/features)
    """
    if axis == 0 and len(targets) != mat.shape[axis]:
        return -1
    if axis == 1 and len(targets) != mat.shape[axis]:
        return -1

    mask = targets == target

    if axis == 0:
        t_mat = mat[mask]  # target matrix
        c_mat = mat[~mask]  # complement matrix
    elif axis == 1:
        t_mat = mat[:, mask]  # target matrix
        c_mat = mat[:, ~mask]  # complement matrix

    return (t_mat, c_mat)


def correct_pval(pval, nfeat, method):
    if method == "bonferroni":
        pval_corr = np.minimum(pval * nfeat[:, None], 1.0)
    return pval_corr


def nd(arr):
    return np.asarray(arr).reshape(-1)


def correct_pvals(pvals):
    r = np.empty(np.shape(pvals))
    r[:] = np.nan
    for pidx, p in enumerate(pvals):
        mask = ~np.isnan(p)

    return r


def dexpress(mat, components, features, assignments, **kwargs):
    """
    mat: ndarray
    components: nparray
    features: nparray
    assignments: nparray, length is equal to length of components
    """
    nan_cutoff = kwargs.get("nan_cutoff", 0)

    unique = np.unique(assignments)

    ncomp = components.shape[0]
    nfeat = features.shape[0]
    nuniq = unique.shape[0]

    pval = np.empty((nuniq, nfeat))  # unique assignments by features
    pval_corr = np.empty((nuniq, nfeat))  # unique assignments by features
    stat = np.empty((nuniq, nfeat))  # unique assignments by features
    es = np.empty((nuniq, nfeat))  # unique assignments by features
    nnz = np.empty((nuniq, nfeat))  # unique assignments by features
    nnz_frac = np.empty((nuniq, nfeat))  # unique assignments by features
    rank = np.empty((nuniq, nfeat))  # unique assignments by features
    mean = np.empty((nuniq, nfeat))  # unique assignments by features
    nfeatures = np.empty((nuniq))

    pval.fill(np.nan)
    pval_corr.fill(np.nan)
    stat.fill(np.nan)
    es.fill(np.nan)
    nnz.fill(np.nan)
    nnz_frac.fill(np.nan)
    rank.fill(np.nan)
    mean.fill(np.nan)
    nfeatures.fill(np.nan)

    for tidx, t in enumerate(unique):
        logging.info("{} of {} assignments: {}".format(tidx + 1, nuniq, t))
        t_mat, c_mat = split_by_target(mat, assignments, t, axis=0)

        pvalue = np.nan
        statistic = np.nan
        effect_size = np.nan

        means = t_mat.mean(0).ravel()
        ranks = means.shape[0] - rankdata(means) # also make rankdata as frac
        

        test = ttest_ind(t_mat, c_mat, nan_policy="propagate", equal_var=False)
        ct_nnz = (t_mat > 0).sum(0).ravel()
        ct_nnz_frac = ct_nnz / t_mat.shape[0]
        pvalue = test.pvalue
        statistic = test.statistic
        effect_size = nd(t_mat.mean(axis=0)) - nd(c_mat.mean(axis=0))
        nfeatures[tidx] = t_mat.shape[1]

        pval[tidx, :] = pvalue
        stat[tidx, :] = statistic
        es[tidx, :] = effect_size
        nnz[tidx, :] = ct_nnz
        nnz_frac[tidx, :] = ct_nnz_frac
        mean[tidx, :] = means
        rank[tidx, :] = ranks
        es[tidx, :] = effect_size
    return (pval, stat, es, nfeatures, nnz, nnz_frac, rank, mean)


def make_table(assignments, features, p_raw, p_corr, es, nnz, nnz_frac, rank, mean):
    unique = np.unique(assignments)
    markers = pd.DataFrame(
        index=unique,
        columns=[
            "target",
            "rank",
            "nnz",
            "nnz_frac",
            "mean",
            "log_fc",
            "p_raw",
            "p_corr",
        ],
    )
    # group
    # target
    # intra_group_rank
    # nz_frac
    # target_mean (in the group)
    # log_fc
    # p_raw
    # p_corr

    for uidx, u in enumerate(unique):
        # "target",
        # "rank",
        # "nnz",
        # "nnz_frac",
        # "mean",
        # "log_fc",
        # "p_raw",
        # "p_corr",
        markers.loc[u]["target"] = features
        markers.loc[u]["rank"] = rank[uidx]
        markers.loc[u]["nnz"] = nnz[uidx]
        markers.loc[u]["nnz_frac"] = nnz_frac[uidx]
        markers.loc[u]["mean"] = mean[uidx]
        markers.loc[u]["p_raw"] = p_raw[uidx]
        markers.loc[u]["p_corr"] = p_corr[uidx]
        markers.loc[u]["log_fc"] = es[uidx]

    markers = markers.apply(pd.Series.explode).reset_index()
    markers = markers[markers.notnull()["p_raw"]]
    markers = markers.rename(columns={"index": "group"})
    return markers


def mx_diff(matrix_fn, bcs_fn, genes_fn, assignments_fn, degs_fn):
    matrix = mmread(matrix_fn).A
    components = []
    read_str_list(bcs_fn, components)
    components = np.array(components)

    features = []
    read_str_list(genes_fn, features)
    features = np.array(features)
    assignments = pd.read_csv(assignments_fn, sep="\t", index_col=0)["label"].values

    nan_cutoff = 0.1  # of elements in cluster
    corr_method = "bonferroni"
    # (pval, stat, es, nfeatures, nnz, nnz_frac, rank, mean)
    p_raw, stat, es, nfeat, nnz, nnz_frac, rank, mean = dexpress(
        matrix, components, features, assignments, nan_cutoff=nan_cutoff
    )
    p_raw = p_raw / 2
    p_corr = correct_pval(p_raw, nfeat, corr_method)

    markers_gene = make_table(
        assignments, features, p_raw, p_corr, es, nnz, nnz_frac, rank, mean
    )

    # convert the 0 pvalues to the smallest possible float
    markers_gene["p_corr"][markers_gene.eval("p_corr == 0").values] = sys.float_info.min
    markers_gene = markers_gene.query("log_fc > 0").query("p_corr < 0.05")
    markers_gene.to_csv(degs_fn, sep="\t", index=False)
