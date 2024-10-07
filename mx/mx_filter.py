import numpy as np
import pandas as pd
from scipy.io import mmread, mmwrite
from scipy.stats import entropy
from sklearn.mixture import GaussianMixture
from scipy.sparse import csr_matrix
from .utils import nd, read_str_list, write_list


def setup_mx_filter_args(parser):
    filter_info = "Filter cells from matrix"
    parser_filter = parser.add_parser(
        "filter", description=filter_info, help=filter_info, add_help=True
    )

    parser_filter.add_argument(
        "-t",
        "--tool",
        default="probabilistic",
        type=str,
        help="Tool to use for filtering",
    )


    # filter subparser arguments
    parser_filter.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="path to input barcodes file",
    )
    parser_filter.add_argument(
        "-bo",
        "--bcs-out",
        default=None,
        type=str,
        required=True,
        help="Path to output barcodes file",
    )
    parser_filter.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Path to output matrix",
    )

    parser_filter.add_argument(
        "-c",
        "--comps",
        default=[2],
        type=int,
        nargs="+",
        help="Number of components for GMM",
    )

    parser_filter.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )
    return parser_filter


def validate_mx_filter_args(parser, args):

    if args.tool == "bustools":
        run_mx_filter_bustools(
            args.matrix,
            args.bcs_in,
            args.output,
            args.bcs_out,
        )
    else:
        run_mx_filter(
            args.matrix,
            args.bcs_in,
            args.output,
            args.bcs_out,
            sum_axis=1,
            comps=args.comps,
            select_axis=None,  # if you want to do the knee only on certain columns
        )


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


def run_mx_filter(
    matrix_fn,
    axis_data_fn,
    matrix_fn_out,
    axis_data_out_fn,
    sum_axis=1,
    comps=[2],
    select_axis=None,  # if you want to do the knee only on certain columns
):
    # read matrix
    mtx = mmread(matrix_fn).toarray()

    # read barcodes
    axis_data = []
    # read_str_list(axis_data_fn, axis_data)
    if axis_data_fn.split(".")[-1] == "gz":
        axis_data = pd.read_csv(axis_data_fn, header=None, compression="gzip").values.flatten()

    else:
        axis_data = pd.read_csv(axis_data_fn, header=None).values.flatten()

    (mtx_f, axis_data_f) = mx_filter(mtx, axis_data, sum_axis, comps, select_axis)

    # save filtered matrix
    mmwrite(matrix_fn_out, csr_matrix(mtx_f))

    # save filtered metadata
    write_list(axis_data_out_fn, axis_data_f)

def run_mx_filter_bustools(
    matrix_fn,
    axis_data_fn,
    matrix_fn_out,
    axis_data_out_fn,
    sum_axis=1,
    comps=[2],
    select_axis=None,  # if you want to do the knee only on certain columns
):
    # read matrix
    mtx = mmread(matrix_fn)

    # read barcodes
    axis_data = []
    # read_str_list(axis_data_fn, axis_data)
    if axis_data_fn.split(".")[-1] == "gz":
        axis_data = pd.read_csv(axis_data_fn, header=None, compression="gzip").values.flatten()

    else:
        axis_data = pd.read_csv(axis_data_fn, header=None).values.flatten()

    (mtx_f, axis_data_f) = mx_filter_bustools(mtx, axis_data, sum_axis, comps, select_axis)

    # save filtered matrix
    mmwrite(matrix_fn_out, csr_matrix(mtx_f))

    # save filtered metadata
    write_list(axis_data_out_fn, axis_data_f)

def mx_filter_bustools(mtx, axis_data, sum_axis, comps, select_axis):
    s = np.asarray(mtx.sum(axis=sum_axis)).reshape(-1)
    s_sort = np.sort(s)[::-1]

    # Step 2: Determine threshold using the top 100 barcodes
    M = 100  # Using top 10 barcodes to determine the threshold
    ERROR_RATE = 0.01  # Error rate as defined in the C++ code
    bclen = 12  # Assuming a UMI length of 12, modify according to your dataset

    # Get the top M barcode counts
    top_m_counts = s_sort[:M]
    avg_count = np.mean(top_m_counts)

    # Calculate the threshold based on the formula
    threshold = avg_count * (1 - np.power(1 - ERROR_RATE, bclen))
    mask = s > threshold
    mtx_f = mtx[mask]
    axis_data_f = np.array(axis_data)[mask]
    return (mtx_f, axis_data_f)

def mx_filter(mtx, axis_data, sum_axis, comps, select_axis):
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
    axis_data_f = np.array(axis_data)[mask]
    return (mtx_f, axis_data_f)
