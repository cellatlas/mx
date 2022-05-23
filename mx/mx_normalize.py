from scipy.io import mmread, mmwrite
from scipy import sparse
import numpy as np


def mx_normalize(matrix_fn, matrix_fn_out):
    mtx = mmread(matrix_fn)
    PF = mtx.sum(axis=1).A.ravel()
    log1pPF = np.log1p(sparse.diags(PF.mean() / PF) @ mtx)

    PF = log1pPF.sum(axis=1).A.ravel()
    PFlog1pPF = sparse.diags(PF.mean() / PF) @ log1pPF

    mmwrite(matrix_fn_out, PFlog1pPF)
