from collections import defaultdict
import numpy as np


def nd(arr):
    return np.asarray(arr).reshape(-1)


def read_markers(
    fname,
    markers_ec=defaultdict(list),
    celltype=defaultdict(),
    marker_genes=defaultdict(),
):
    with open(fname, "r") as f:
        for idx, line in enumerate(f.readlines()):
            ct, genes = line.strip().split("\t")
            celltype[ct] = idx

            # two things
            # 1. make marker_genes list
            # 2. make markers_ec
            for g in genes.split(","):
                gidx = len(marker_genes)

                # check if the gene has been added already
                if g in marker_genes.keys():  # gene repeated
                    gidx = marker_genes[g]
                else:
                    marker_genes[g] = gidx

                # for the cell type index, add the marker gene index
                markers_ec[celltype[ct]].append(marker_genes[g])

            # sort the marker genes
            markers_ec[celltype[ct]] = sorted(markers_ec[celltype[ct]])


def read_int_list(fname, lst=[]):
    with open(fname) as f:
        for idx, i in enumerate(f.readlines()):
            lst.append(int(i.strip()))


def read_str_list(fname, lst=list):
    with open(fname, "r") as f:
        for idx, line in enumerate(f.readlines()):
            lst.append(line.strip())


def write_list(fname, lst=list):
    with open(fname, "w") as f:
        for idx, ele in enumerate(lst):
            f.write(f"{ele}\n")


def write_markers(fname, markers):
    with open(fname, "w") as f:
        for k, v in markers.items():
            f.write(f"{k}\t")
            n = len(v)
            for idx, i in enumerate(v):
                f.write(f"{i}")
                if idx < n - 1:
                    f.write(",")
            f.write("\n")


def map_dict_list_keys_values(dct, k_lst, v_lst, nd=defaultdict(list)):
    for k, v in dct.items():
        nd[k_lst[k]] = [v_lst[i] for i in v]


def map_dict_list_keys(dct, k_lst, v_lst, nd=defaultdict(list)):
    for k, v in dct.items():
        nd[k_lst[k]] = v


def map_dict_list_values(dct, k_lst, v_lst, nd=defaultdict(list)):
    for k, v in dct.items():
        nd[k] = [v_lst[i] for i in v]


def read_markers_ec(fname, markers_ec=defaultdict(list)):
    with open(fname, "r") as f:
        for idx, line in enumerate(f.readlines()):
            split = line.strip().split("\t")
            if len(split) == 2:
                ct_id, gene_ids = split
                markers_ec[int(ct_id)] = [int(i) for i in gene_ids.split(",")]
            else:
                ct_id = split[0]
                markers_ec[int(ct_id)] = []


def read_markers_str(fname, markers=defaultdict(list)):
    with open(fname, "r") as f:
        for idx, line in enumerate(f.readlines()):
            split = line.strip().split("\t")
            if len(split) == 2:
                ct_id, gene_ids = split 
                markers[ct_id] = [i for i in gene_ids.split(",")]
            if len(split) == 1:
                ct_id = split
                markers[ct_id] = []


def drop_markers(markers_ec, drop_ids):
    if len(drop_ids) == 0:
        return

    for k, v in markers_ec.items():

        gidx = len(v) - 1
        while gidx > -1:
            mg = markers_ec[k][gidx]

            if mg in drop_ids:
                markers_ec[k].pop(gidx)
            else:
                to_sub = 0
                for d in drop_ids:
                    if d < mg:
                        to_sub += 1
                markers_ec[k][gidx] -= to_sub
            gidx -= 1
