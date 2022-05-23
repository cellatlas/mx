from scipy.io import mmread, mmwrite
from .utils import read_str_list, write_list
import numpy as np


def mx_extract(matrix_fn, genes_in_fn, targets_fn, genes_out_fn, output_fn):

    # read in targets.txt
    targets = []
    read_str_list(targets_fn, targets)

    genes = []
    read_str_list(genes_in_fn, genes)

    mtx = mmread(matrix_fn).tocsr()

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
    mmwrite(output_fn, sel_mtx)
    sel_genes = np.array(genes)[select_reorder]
    write_list(genes_out_fn, sel_genes.tolist())
