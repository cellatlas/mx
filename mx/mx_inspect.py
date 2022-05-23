import json

import numpy as np

from scipy.io import mmread
from scipy.optimize import curve_fit


def meanvar(x, alpha):
    return x + alpha * x**2


def compute_overdispersion(mtx):
    mtx_squared = mtx.copy()
    mtx_squared.data **= 2
    gvar = (mtx_squared.mean(0) - np.square(mtx.mean(0))).A.ravel()

    x = mtx.mean(axis=0).A.ravel()
    y = gvar

    popt, pcov = curve_fit(meanvar, x, y)
    return popt[0]


def compute_metrics(mtx):
    ncells, ngenes = mtx.shape
    apc = mtx.sum(1).mean()
    apg = mtx.sum(0).mean()
    mnc = int(mtx.sum(1).min())
    mxc = int(mtx.sum(1).max())
    cellsum = int(mtx.sum())
    nvals = mtx.nnz
    n = mtx.shape[0] * mtx.shape[1]
    sp = nvals / n
    overdispersion = compute_overdispersion(mtx)

    entry = {
        "ncells": ncells,
        "ngenes": ngenes,
        "nvals": nvals,
        "density": sp,
        "avg_per_cell": apc,
        "avg_per_gene": apg,
        "min_cell": mnc,
        "max_cell": mxc,
        "total_count": cellsum,
        "overdispersion": overdispersion,
    }
    return entry


def mx_inspect(mtx_fn, metrics_fn):
    mtx = mmread(mtx_fn)
    metrics = compute_metrics(mtx)
    with open(metrics_fn, "w") as jsonFile:
        json.dump(metrics, jsonFile)
