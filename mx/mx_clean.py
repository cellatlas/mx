from .utils import read_str_list, write_list, read_markers_str, write_markers
from scipy.io import mmread, mmwrite
import numpy as np
from collections import defaultdict

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
        "-mi",
        "--markers-in",
        default=None,
        type=str,
        required=True,
        help="Input path for markers.txt",
    )
    parser_clean.add_argument(
        "-mo",
        "--markers-out",
        default=None,
        type=str,
        required=True,
        help="Output path for markers.txt",
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

    return parser_clean

def validate_mx_clean_args(parser, args):

    run_mx_clean(
        args.matrix,
        args.genes_in,
        args.bcs_in,
        args.markers_in,
        args.output,
        args.genes_out,
        args.bcs_out,
        args.markers_out
    )

def run_mx_clean(
    matrix_fn, genes_fn, barcodes_fn, markers_fn, matrix_out_fn, genes_out_fn, barcodes_out_fn, markers_out_fn
):
    mtx = mmread(matrix_fn).tocsr()
    genes = []
    read_str_list(genes_fn, genes)
    barcodes = []
    read_str_list(barcodes_fn, barcodes)
    markers = defaultdict(list)
    read_markers_str(markers_fn, markers)
    
    mtx_f, bcs_f, genes_f, markers_f = mx_clean(mtx, barcodes, genes, markers)

    mmwrite(matrix_out_fn, mtx_f)
    write_list(genes_out_fn, genes_f)
    write_list(barcodes_out_fn, bcs_f)
    write_markers(markers_out_fn, markers_f)


def mx_clean(mtx, bcs, genes, markers):
    ## mx clean
    # subsetting the matrix means that some cells/genes will now have zero counts -> drop em!

    # check if any cells are zero
    nonzero_cells = np.where(mtx.sum(1).A.ravel() != 0)[0]
    nonzero_genes = np.where(mtx.sum(0).A.ravel() != 0)[0]
    print(f"Dropping {mtx.shape[0] - len(nonzero_cells)} cells")
    print(f"Dropping {mtx.shape[1] - len(nonzero_genes)} genes")

    mtx_f = mtx[nonzero_cells].copy()
    mtx_ff = mtx_f[:,nonzero_genes].copy()
    print(nonzero_genes)
    print(genes)
    # Filter barcodes and genes
    bcs_f = np.array(bcs)[nonzero_cells].tolist()
    genes_f = np.array(genes)[nonzero_genes].tolist()
    print(markers)
    # Filter markers.txt file
    markers_f = defaultdict(list)
    for ct in list(markers.keys()):
      ct_gf = [g for g in markers[ct] if g in genes_f]
      markers_f[ct] =  ct_gf
    
    print(markers_f)
    return (mtx_ff, bcs_f, genes_f, markers_f)
