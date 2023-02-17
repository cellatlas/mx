from .utils import read_str_list, write_list
from scipy.io import mmread, mmwrite
import numpy as np


def mx_clean(
    matrix_fn, genes_fn, barcodes_fn, matrix_out_fn, genes_out_fn, barcodes_out_fn
):
    mtx = mmread(matrix_fn).tocsr()
    genes = []
    read_str_list(genes_fn, genes)
    barcodes = []
    read_str_list(barcodes_fn, barcodes)

    ## mx clean
    # subsetting the matrix means that some cells/genes will now have zero counts -> drop em!

    # check if any cells are zero
    nonzero_cells = np.where(mtx.sum(1).A.ravel() != 0)[0]
    print(f"Dropping {mtx.shape[0] - len(nonzero_cells)} cells")
    mtx = mtx[nonzero_cells].copy()

    # check if any genes are zero NB: if genes end up getting dropped, will have to handle that in the EC files
    #nonzero_genes = np.where(mtx.sum(0).A.ravel() != 0)[0]
    #print(f"Dropping {mtx.shape[1] - len(nonzero_genes)} genes")
    #mtx = mtx[:, nonzero_genes].copy()

    mmwrite(matrix_out_fn, mtx)
    write_list(genes_out_fn, np.array(genes))
    write_list(barcodes_out_fn, np.array(barcodes)[nonzero_cells].tolist())
