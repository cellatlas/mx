from .utils import read_str_list, write_list
from scipy.io import mmread, mmwrite
import numpy as np


def setup_mx_clean_args(parser):

    # Clean ###################################################
    clean_info = "Drop rows/cols with all zeros"
    parser_clean = parser.add_parser(
        "clean", description=clean_info, help=clean_info, add_help=True
    )

    # clean subparser arguments
    parser_clean.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    parser_clean.add_argument(
        "-go",
        "--genes-out",
        default=None,
        type=str,
        required=True,
        help="Output path for genes.txt",
    )
    # clean subparser arguments
    parser_clean.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="Input path for bcs.txt",
    )
    parser_clean.add_argument(
        "-bo",
        "--bcs-out",
        default=None,
        type=str,
        required=True,
        help="Output path for bcs.txt",
    )

    parser_clean.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_clean.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )


def validate_mx_clean_args(parser, args):

    run_mx_clean(
        args.matrix,
        args.genes_in,
        args.bcs_in,
        args.output,
        args.genes_out,
        args.bcs_out,
    )


def run_mx_clean(
    matrix_fn, genes_fn, barcodes_fn, matrix_out_fn, genes_out_fn, barcodes_out_fn
):
    mtx = mmread(matrix_fn).tocsr()
    genes = []
    read_str_list(genes_fn, genes)
    barcodes = []
    read_str_list(barcodes_fn, barcodes)

    mtx_f, bcs_f, genes_f = mx_clean(mtx, barcodes, genes)

    mmwrite(matrix_out_fn, mtx_f)
    write_list(genes_out_fn, genes_f)
    write_list(barcodes_out_fn, bcs_f)


def mx_clean(mtx, bcs, genes):
    ## mx clean
    # subsetting the matrix means that some cells/genes will now have zero counts -> drop em!

    # check if any cells are zero
    nonzero_cells = np.where(mtx.sum(1).A.ravel() != 0)[0]
    print(f"Dropping {mtx.shape[0] - len(nonzero_cells)} cells")
    mtx_f = mtx[nonzero_cells].copy()
    genes_f = genes  # np.array(genes)[nonzero_genes].tolist() # TODO handle no genes
    bcs_f = np.array(bcs)[nonzero_cells].tolist()

    # check if any genes are zero NB: if genes end up getting dropped, will have to handle that in the EC files
    nonzero_genes = np.where(mtx.sum(0).A.ravel() != 0)[0]
    print(f"Dropping {mtx.shape[1] - len(nonzero_genes)} genes")
    mtx = mtx[:, nonzero_genes].copy()
    return (mtx_f, bcs_f, genes_f)
