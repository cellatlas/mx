from scipy.stats import ttest_ind
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
    nfeatures = np.empty((nuniq))

    pval.fill(np.nan)
    pval_corr.fill(np.nan)
    stat.fill(np.nan)
    es.fill(np.nan)
    nfeatures.fill(np.nan)

    for tidx, t in enumerate(unique):
        logging.info("{} of {} assignments: {}".format(tidx + 1, nuniq, t))
        t_mat, c_mat = split_by_target(mat, assignments, t, axis=0)

        pvalue = np.nan
        statistic = np.nan
        effect_size = np.nan

        test = ttest_ind(t_mat, c_mat, nan_policy="propagate", equal_var=False)
        pvalue = test.pvalue
        statistic = test.statistic
        effect_size = nd(t_mat.mean(axis=0)) - nd(c_mat.mean(axis=0))
        nfeatures[tidx] = t_mat.shape[1]

        pval[tidx, :] = pvalue
        stat[tidx, :] = statistic
        es[tidx, :] = effect_size
    return (pval, stat, es, nfeatures)


def make_table(assignments, features, p_raw, p_corr, es):
    unique = np.unique(assignments)
    markers = pd.DataFrame(index=unique, columns=["name", "p_raw", "p_corr", "es"])

    for uidx, u in enumerate(unique):
        markers.loc[u]["name"] = features
        markers.loc[u]["p_raw"] = p_raw[uidx]
        markers.loc[u]["p_corr"] = p_corr[uidx]
        markers.loc[u]["es"] = es[uidx]

    markers = markers.apply(pd.Series.explode).reset_index()
    markers = markers[markers.notnull()["p_raw"]]
    markers = markers.rename(columns={"index": "group_id"})
    return markers


def mx_diff(matrix_fn, bcs_fn, genes_fn, assignments_fn, degs_fn):
    matrix = mmread(matrix_fn).A
    components = []
    read_str_list(bcs_fn, components)
    components = np.array(components)

    features = []
    read_str_list(genes_fn, features)
    features = np.array(features)
    assignments = pd.read_csv(assignments_fn, sep="\t", index_col=0).label

    nan_cutoff = 0.1  # of elements in cluster
    corr_method = "bonferroni"

    p_raw, stat, es, nfeat = dexpress(
        matrix, components, features, assignments, nan_cutoff=nan_cutoff
    )
    p_raw = p_raw / 2
    p_corr = correct_pval(p_raw, nfeat, corr_method)

    markers_gene = make_table(assignments, features, p_raw, p_corr, es)

    # convert the 0 pvalues to the smallest possible float
    markers_gene["p_corr"][markers_gene.eval("p_corr == 0").values] = sys.float_info.min
    markers_gene = markers_gene.query("es > 0").query("p_corr < 0.05")
    markers_gene.to_csv(degs_fn, sep="\t", index=False)
