from scipy.io import mmread, mmwrite
from scipy import sparse
import numpy as np


def mx_normalize(matrix_fn, matrix_fn_out, pf_only=False):
   
   if pf_only:
       mtx = mmread(matrix_fn)
       pre_pf = mtx.sum(axis=1).A.ravel()
       pf = sparse.diags(pre_pf.mean()/pre_pf) @ mtx
       mmwrite(matrix_fn_out, pf)
       
   else:
       mtx = mmread(matrix_fn)
       PF = mtx.sum(axis=1).A.ravel()
       log1pPF = np.log1p(sparse.diags(PF.mean() / PF) @ mtx)

       PF = log1pPF.sum(axis=1).A.ravel()
       PFlog1pPF = sparse.diags(PF.mean() / PF) @ log1pPF

       mmwrite(matrix_fn_out, PFlog1pPF)
