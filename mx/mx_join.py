from scipy.io import mmread, mmwrite
from .utils import read_str_list, write_list
import numpy as np
import pandas as pd


# def setup_mx_join_args(parser):
#     join_info = "Join two matrices"
#     parser_join = parser.add_parser(
#         "join", description=join_info, help=join_info, add_help=True
#     )
#     parser_join.add_argument(
#         "-t",
#         "--targets",
#         default=None,
#         type=str,
#         required=True,
#         help="Path to targets file to join",
#     )

#     # join subparser arguments
#     parser_join.add_argument(
#         "-gi",
#         "--genes-in",
#         default=None,
#         type=str,
#         required=True,
#         help="Input path for genes.txt",
#     )
#     parser_join.add_argument(
#         "-go",
#         "--genes-out",
#         default=None,
#         type=str,
#         required=True,
#         help="Output path for genes.txt",
#     )

#     parser_join.add_argument(
#         "-o",
#         "--output",
#         default=None,
#         type=str,
#         required=True,
#         help="Output path to save matrix",
#     )

#     parser_join.add_argument(
#         "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
#     )
#     return parser_join


def validate_mx_join_args(parser, args):
    matrixA_fn = args.matrixA_fn
    bcsA_fn = args.bcsA_fn
    genesA_fn = args.genesA_fn
    matrixB_fn = args.matrixB_fn
    bcsB_fn = args.bcsB_fn
    genesB_fn = args.genesB_fn
    out_matrixA_fn = args.out_matrixA_fn
    out_bcsA_fn = args.out_bcsA_fn
    out_genesA_fn = args.out_genesA_fn
    out_matrixB_fn = args.out_matrixB_fn
    out_bcsB_fn = args.out_bcsB_fn
    out_genesB_fn = args.out_genesB_fn

    run_mx_join(
        matrixA_fn,
        bcsA_fn,
        genesA_fn,
        matrixB_fn,
        bcsB_fn,
        genesB_fn,
        out_matrixA_fn,
        out_bcsA_fn,
        out_genesA_fn,
        out_matrixB_fn,
        out_bcsB_fn,
        out_genesB_fn,
        how="inner",
    )


def run_mx_join(
    matrixA_fn,
    bcsA_fn,
    genesA_fn,
    matrixB_fn,
    bcsB_fn,
    genesB_fn,
    out_matrixA_fn,
    out_bcsA_fn,
    out_genesA_fn,
    out_matrixB_fn,
    out_bcsB_fn,
    out_genesB_fn,
    how="inner",
):
    # read in
    mtxA = mmread(matrixA_fn).tocsr()
    bcsA = []
    read_str_list(bcsA_fn, bcsA)
    genesA = []
    read_str_list(genesA_fn, genesA)

    mtxB = mmread(matrixB_fn).tocsr()
    bcsB = []
    read_str_list(bcsB_fn, bcsB)
    genesB = []
    read_str_list(genesB_fn, genesB)

    # execute
    (join_mtxA, join_bcsA, join_mtxB, join_bcsB) = mx_join_barcodes(
        mtxA, bcsA, mtxB, bcsB, how="inner"
    )
    (join_mtxA, join_genesA, join_mtxB, join_genesB) = mx_join_genes(
        join_mtxA, genesA, join_mtxB, genesB, how="inner"
    )
    # write out
    mmwrite(output_mtxA_fn, join_mtxA)
    mmwrite(output_mtxB_fn, join_mtxB)
    write_list(out_bcsA_fn, join_bcsA)
    write_list(out_bcsB_fn, join_bcsB)
    write_list(out_genesA_fn, join_genesA)
    write_list(out_genesB_fn, join_genesB)


def mx_join_barcodes(mtxA, bcsA, mtxB, bcsB, how="inner"):
    # join two matrices
    # mtxA: csr matrix
    # bcsA: list of barcodes
    # mtxB: csr matrix
    # bcsB: list of barcodes
    # how: "inner" or "left" or "right"
    # return: csr matrix, list of barcodes

    # get indices of barcodes
    bcsA = np.array(bcsA)
    bcsB = np.array(bcsB)
    if how == "inner":
        sel_bcs = np.intersect1d(bcsA, bcsB)
    elif how == "left":
        sel_bcs = np.setdiff1d(bcsA, bcsB)
    elif how == "right":
        sel_bcs = np.setdiff1d(bcsB, bcsA)
    else:
        raise ValueError("how must be 'inner' or 'left' or 'right")

    bcsA_mask = pd.Series({"bcs": bcsA}).isin(sel_bcs).values
    bcsB_mask = pd.Series({"bcs": bcsB}).isin(sel_bcs).values

    join_mtxA = mtxA[bcsA_mask, :]
    join_mtxB = mtxB[bcsB_mask, :]
    join_bcsA = bcsA[bcsA_mask]
    join_bcsB = bcsB[bcsB_mask]

    return (join_mtxA, join_bcsA, join_mtxB, join_bcsB)


def mx_join_genes(mtxA, genesA, mtxB, genesB, how="inner"):
    # join two matrices
    # mtxA: csr matrix
    # genesA: list of barcodes
    # mtxB: csr matrix
    # genesB: list of barcodes
    # how: "inner" or "left" or "right"
    # return: csr matrix, list of barcodes

    # get indices of barcodes
    genesA = np.array(genesA)
    genesB = np.array(genesB)
    if how == "inner":
        sel_genes = np.intersect1d(genesA, genesB)
    elif how == "left":
        sel_genes = np.setdiff1d(genesA, genesB)
    elif how == "right":
        sel_genes = np.setdiff1d(genesB, genesA)
    else:
        raise ValueError("how must be 'inner' or 'left' or 'right")

    genesA_mask = pd.Series({"genes": genesA}).isin(sel_genes).values
    genesB_mask = pd.Series({"genes": genesB}).isin(sel_genes).values

    join_mtxA = mtxA[:, genesA_mask]
    join_mtxB = mtxB[:, genesB_mask]
    join_genesA = genesA[genesA_mask]
    join_genesB = genesB[genesB_mask]

    return (join_mtxA, join_genesA, join_mtxB, join_genesB)
