import os
import numpy as np
from .utils import read_str_list, write_list
from scipy.io import mmread, mmwrite


def mx_split(matrix_fn, bcs_in_fn, genes_in_fn, assignments_fn, output_fn):
    mtx = mmread(matrix_fn).tocsr()
    genes = []
    read_str_list(genes_fn, genes)
    barcodes = []
    read_str_list(barcodes_fn, barcodes)

    assignments = []
    read_str_list(assignments_fn, assignments)

    unique = np.unique(assignments)
    for a in unique:
        mask = np.array(assignments) == a

        out_path = os.path.join(output_fn, a)
        # write matrix
        mmwrite(os.path.join(out_path, "matrix.mtx"), mtx[mask])
        # write barcodes
        write_list(
            os.path.join(out_path, "barcodes.txt"), np.array(barcodes)[mask].tolist()
        )

    return
