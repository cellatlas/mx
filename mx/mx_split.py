import os
import numpy as np
from .utils import read_str_list, write_list
from scipy.io import mmread, mmwrite


def setup_mx_split_args(parser):
    # Split ###################################################
    split_info = "Split matrix by assignments"
    parser_split = parser.add_parser(
        "split", description=split_info, help=split_info, add_help=True
    )

    # split subparser arguments
    parser_split.add_argument(
        "-a",
        "--assignments",
        default=None,
        type=str,
        required=True,
        help="Input path for assignments.txt",
    )
    # split subparser arguments
    parser_split.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    # split subparser arguments
    parser_split.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="Input path for bcs.txt",
    )

    parser_split.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_split.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )
    return parser_split


def validate_mx_split_args(parser, args):
    run_mx_split(args.matrix, args.bcs_in, args.genes_in, args.assignments, args.output)


def run_mx_split(matrix_fn, bcs_fn, genes_fn, assignments_fn, output_fn):
    mtx = mmread(matrix_fn).tocsr()
    genes = []
    read_str_list(genes_fn, genes)
    barcodes = []
    read_str_list(bcs_fn, barcodes)
    assignments = []
    read_str_list(assignments_fn, assignments)

    for m, b, g, t in mx_split(mtx, barcodes, genes, assignments):

        # set path according to the target
        out_path = os.path.join(output_fn, t)
        if not os.path.exists(out_path):
            os.makedirs(out_path)
        # write matrix
        mmwrite(os.path.join(out_path, "matrix.mtx"), m)
        # write barcodes
        write_list(os.path.join(out_path, "barcodes.txt"), b)
        # write genes
        write_list(os.path.join(out_path, "genes.txt"), g)

    return


def mx_split(mtx, barcodes, genes, assignments):
    unique = np.unique(assignments)
    for target in unique:
        mask = np.array(assignments) == target
        yield (mtx[mask], np.array(barcodes)[mask].tolist(), genes, target)
