from scipy.io import mmread, mmwrite
from scipy import sparse
import numpy as np
from scipy.stats import rankdata
from scipy.sparse import csr_matrix

def setup_mx_normalize_args(parser):
    normalize_info = "Normalize matrix with PFlog1pPF"
    parser_normalize = parser.add_parser(
        "normalize", description=normalize_info, help=normalize_info, add_help=True
    )

    # normalize subparser arguments
    parser_normalize.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_normalize.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    parser_normalize.add_argument(
        "-m", "--method", required=False, default="log1pPF", choices=["PF", "log1pPF", "rank"]
    )

    return parser_normalize


def validate_mx_normalize_args(parser, args):
    run_mx_normalize(args.matrix, args.output, args.method)


def run_mx_normalize(matrix_fn, matrix_fn_out, method):
    mtx = mmread(matrix_fn)
    n = mx_normalize(mtx, method)

    mmwrite(matrix_fn_out, n)


def mx_normalize(mtx, method="log1pPF"):

    if method == "log1pPF":
        PF = mtx.sum(axis=1).A.ravel()
        log1pPF = np.log1p(sparse.diags(PF.mean() / PF) @ mtx)

        PF = log1pPF.sum(axis=1).A.ravel()
        m = sparse.diags(PF.mean() / PF) @ log1pPF
    elif method == "PF":
        pre_pf = mtx.sum(axis=1).A.ravel()
        m = sparse.diags(pre_pf.mean() / pre_pf) @ mtx
    elif method == "rank":
        m = csr_matrix((rankdata(mtx.toarray(), axis=0, method="min"))-1)
    else:
        raise NotImplementedError()

    return m
