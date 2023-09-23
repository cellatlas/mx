import json
import numpy as np
import pandas as pd

from scipy.io import mmread
from scipy.optimize import curve_fit
from .utils import nd


def setup_mx_inspect_args(parser):
    # inspect ###################################################
    inspect_info = "Inspect matrix"
    parser_format = parser.add_parser(
        "inspect", description=inspect_info, help=inspect_info, add_help=True
    )

    # inspect subparser arguments
    parser_format.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save metrics",
    )
    parser_format.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    parser_format.add_argument(
        "-a", "--axis", required=False, default=None, choices=["rows", "cols"]
    )

    parser_format.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="Input path for bcs.txt",
    )

    parser_format.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )
    return parser_format


def validate_mx_inspect_args(parser, args):
    mtx_fn = args.matrix
    metrics_fn = args.output
    axis = args.axis
    barcodes_fn = args.bcs_in
    genes_fn = args.genes_in

    if axis == "rows":
        run_mx_inspect_rows(mtx_fn, barcodes_fn, metrics_fn)
    elif axis == "cols":
        run_mx_inspect_cols(mtx_fn, genes_fn, metrics_fn)
    else:
        run_mx_inspect(mtx_fn, metrics_fn)


def run_mx_inspect(mtx_fn, metrics_fn):
    mtx = mmread(mtx_fn)
    metrics = mx_inspect(mtx)
    with open(metrics_fn, "w") as jsonFile:
        json.dump(metrics, jsonFile)


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


def mx_inspect(mtx):
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


def run_mx_inspect_rows(matrix_fn, barcodes_fn, barcodes_out_fn):
    mtx = mmread(matrix_fn).tocsr()
    barcodes = pd.read_csv(barcodes_fn, header=None, names=["bcs"])["bcs"]
    # genes = pd.read_csv(genes_fn, header=None, names=["bcs"])

    df = mx_inspect_rows(mtx, barcodes)
    df.to_csv(barcodes_out_fn, sep="\t")
    return


def mx_inspect_rows(mtx, md):
    counts_min = nd(mtx.min(axis=1).A)
    counts_max = nd(mtx.max(axis=1).A)
    counts_sum = nd(np.sum(mtx, axis=1))
    counts_mean = nd(np.mean(mtx, axis=1))
    # counts_median = nd(np.median(mtx, axis=1))
    counts_nnzero = mtx.getnnz(1)  # nd(np.sum(mtx == 0, axis=1))
    mtx_mean = mtx.mean(1)
    mtx.data **= 2
    counts_variance = nd(mtx.mean(1)) - nd(mtx_mean) ** 2

    return pd.DataFrame(
        {
            "counts_min": counts_min,
            "counts_max": counts_max,
            "counts_sum": counts_sum,
            "counts_mean": counts_mean,
            # "counts_median": counts_median,
            "counts_nnzero": counts_nnzero,
            "counts_variance": counts_variance,
        },
        index=pd.Index(md, name="bcs"),
    )


def run_mx_inspect_cols(matrix_fn, genes_fn, genes_out_fn):
    mtx = mmread(matrix_fn).tocsr()
    # barcodes = pd.read_csv(barcodes_fn, header=None, names=["bcs"])
    genes = pd.read_csv(genes_fn, header=None, names=["genes"])["genes"]

    df = mx_inspect_cols(mtx, genes)
    df.to_csv(genes_out_fn, sep="\t")
    return


def mx_inspect_cols(mtx, md):
    counts_min = nd(mtx.min(axis=0).A)
    counts_max = nd(mtx.max(axis=0).A)
    counts_sum = nd(np.sum(mtx, axis=0))
    counts_mean = nd(np.mean(mtx, axis=0))
    # counts_median = nd(np.median(mtx, axis=0))
    counts_nnzero = mtx.getnnz(0)  # nd(np.sum(mtx == 0, axis=0))
    mtx_mean = mtx.mean(0)
    mtx.data **= 2
    counts_variance = nd(mtx.mean(0)) - nd(mtx_mean) ** 2

    return pd.DataFrame(
        {
            "counts_min": counts_min,
            "counts_max": counts_max,
            "counts_sum": counts_sum,
            "counts_mean": counts_mean,
            # "counts_median": counts_median,
            "counts_nnzero": counts_nnzero,
            "counts_variance": counts_variance,
        },
        index=pd.Index(md, name="genes"),
    )
