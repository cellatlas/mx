from scipy.io import mmread, mmwrite
from .utils import read_str_list, write_list
import numpy as np
import pandas as pd

def setup_mx_extract_args(parser):
    extract_info = "Extract submatrix of genes"
    parser_extract = parser.add_parser(
        "extract", description=extract_info, help=extract_info, add_help=True
    )
    parser_extract.add_argument(
        "-t",
        "--targets",
        default=None,
        type=str,
        required=True,
        help="Path to targets file to extract",
    )

    # extract subparser arguments
    parser_extract.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    parser_extract.add_argument(
        "-go",
        "--genes-out",
        default=None,
        type=str,
        required=True,
        help="Output path for genes.txt",
    )

    parser_extract.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_extract.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )
    return parser_extract


def validate_mx_extract_args(parser, args):
    run_mx_extract(
        args.matrix,
        args.genes_in,
        args.targets,
        args.genes_out,
        args.output,
    )


def run_mx_extract(matrix_fn, genes_in_fn, targets_fn, genes_out_fn, output_fn):

    # read in
    targets = []
    # read_str_list(targets_fn, targets)
    if targets_fn.split(".")[-1] == "gz":
        targets = pd.read_csv(targets_fn, header=None, compression="gzip").values.flatten()
    else:
        targets = pd.read_csv(targets_fn, header=None).values.flatten()
    genes = []
    # read_str_list(genes_in_fn, genes)
    if genes_in_fn.split(".")[-1] == "gz":
        genes = pd.read_csv(genes_in_fn, header=None, compression="gzip").values.flatten()
    else:
        genes = pd.read_csv(genes_in_fn, header=None).values.flatten()
    mtx = mmread(matrix_fn).tocsr()

    # execute
    sel_mtx, sel_genes = mx_extract(mtx, genes, targets)

    # write out
    mmwrite(output_fn, sel_mtx)
    write_list(genes_out_fn, sel_genes)


def mx_extract(mtx, genes, targets):
    # given markers.txt, extract the matrix (in the order of the genes there)
    select_reorder = [genes.index(i) for i in targets]
    sr_genes = [genes[i] for i in select_reorder]

    # verify that select does its job
    assert 0 == (
        [genes[gidx] == targets[idx] for idx, gidx in enumerate(select_reorder)]
    ).count(False)
    assert True == (len(select_reorder) == len(targets))
    assert True == (sr_genes == targets)

    sel_mtx = mtx[:, select_reorder].copy()
    sel_genes = np.array(genes)[select_reorder]
    return (sel_mtx, sel_genes.tolist())
