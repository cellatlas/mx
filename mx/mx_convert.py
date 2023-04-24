from kb_python.utils import import_matrix_as_anndata


def setup_mx_convert_args(parser):
    info = "Convert matrix"
    subparser = parser.add_parser("convert", description=info, help=info, add_help=True)

    # subparser arguments
    subparser.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )
    subparser.add_argument(
        "-bi",
        "--barcodes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for barcodes.txt",
    )
    subparser.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )

    subparser.add_argument(
        "-t",
        "--filetype",
        metavar="filetype",
        help=("file Type"),
        type=str,
        default="h5ad",
    )

    subparser.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    return subparser


def validate_mx_convert_args(parser, args):
    run_mx_convert(
        args.matrix, args.barcodes_in, args.genes_in, args.output, args.filetype
    )


def run_mx_convert(matrix_fn, barcodes_fn, genes_fn, output_fn, filetype):
    # mtx = mmread(matrix_fn)
    # barcodes = read_str_list(barcodes_fn)
    # genes = read_str_list(genes_fn)

    out = import_matrix_as_anndata(matrix_fn, barcodes_fn, genes_fn)
    out.write_h5ad(output_fn)


def mx_convert(mtx, barcodes, genes):
    pass
