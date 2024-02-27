import json
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt

from scipy.io import mmread
from scipy.optimize import curve_fit
from .utils import nd


def setup_mx_plot_args(parser):
    # plot ###################################################
    plot_info = "Plot figures for a matrix"
    parser_format = parser.add_parser(
        "plot", description=plot_info, help=plot_info, add_help=True
    )

    # plot subparser arguments
    parser_format.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save plot",
    )
    parser_format.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )


    parser_format.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="Input path for bcs.txt",
    )

    parser_format.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )
    return parser_format


def validate_mx_plot_args(parser, args):
    mtx_fn = args.matrix
    plot_fn = args.output
    axis = args.axis
    barcodes_fn = args.bcs_in
    genes_fn = args.genes_in


    run_mx_plot(mtx_fn, plot_fn)


def run_mx_plot(mtx_fn, plot_fn):
    mtx = mmread(mtx_fn)
    fig, ax = plt.subplots(figsize=(10,10))
    ax = mx_plot(mtx, ax)
    fig.savefig(plot_fn)


def yex(ax):
    lims = [
        np.min([ax.get_xlim(), ax.get_ylim()]),  # min of both axes
        np.max([ax.get_xlim(), ax.get_ylim()]),  # max of both axes
    ]

    # now plot both limits against eachother
    ax.plot(lims, lims, c="k", alpha=0, zorder=0)
    ax.set(**{"aspect": "equal", "xlim": lims, "ylim": lims})
    return ax

def mx_plot(mtx, ax):
    s = np.array(mtx.sum(axis=1)).flatten()

    
    x = s.sort()
    y = np.arange(x.shape[0])
    ax.scatter(x,y)
    ax.set(**{
        "xscale": "log",
        "yscale": "log",
        "xlabel": "UMI count",
        "ylabel": "Number of cells"
    })
    yex(ax)
    
    return ax