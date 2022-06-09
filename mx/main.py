import sys
import argparse

from .mx_diff import mx_diff
from .mx_inspect import mx_inspect
from .mx_filter import mx_filter
from .mx_normalize import mx_normalize
from .mx_extract import mx_extract
from .mx_clean import mx_clean
from .mx_assign import mx_assign_fn


def main():
    # mx_assign.py
    # mx_clean.py

    # Define parent parser
    parser = argparse.ArgumentParser(description="mx", add_help=False)
    # Initiate subparsers
    subparsers = parser.add_subparsers(dest="command")
    # Define parent (not sure why I need both parent parser and parent, but otherwise it does not work)
    # parent = argparse.ArgumentParser(add_help=False)

    # Add custom help argument to parent parser
    parser.add_argument("-h", "--help", action="store_true", help="Print help.")

    # Filter ###################################################
    filter_info = "Filter cells from matrix"
    parser_filter = subparsers.add_parser(
        "filter", description=filter_info, help=filter_info, add_help=True
    )

    # filter subparser arguments
    parser_filter.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="path to input barcodes file",
    )
    parser_filter.add_argument(
        "-bo",
        "--bcs-out",
        default=None,
        type=str,
        required=True,
        help="Path to output barcodes file",
    )
    parser_filter.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Path to output matrix",
    )
    parser_filter.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    # Normlize ###################################################
    normalize_info = "Normalize matrix with PFlog1pPF"
    parser_normalize = subparsers.add_parser(
        "normalize", description=normalize_info, help=normalize_info, add_help=True
    )

    # normalize subparser arguments
    parser_normalize.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_normalize.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    # Extract ###################################################
    extract_info = "Extract submatrix of genes"
    parser_extract = subparsers.add_parser(
        "extract", description=extract_info, help=extract_info, add_help=True
    )
    parser_extract.add_argument(
        "-t",
        "--targets",
        default=None,
        type=str,
        required=True,
        help="Path to targets file to extract",
    )

    # extract subparser arguments
    parser_extract.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    parser_extract.add_argument(
        "-go",
        "--genes-out",
        default=None,
        type=str,
        required=True,
        help="Output path for genes.txt",
    )

    parser_extract.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_extract.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    # inspect ###################################################
    inspect_info = "Inspect matrix (metrics.json)"
    parser_inspect = subparsers.add_parser(
        "inspect", description=inspect_info, help=inspect_info, add_help=True
    )

    # inspect subparser arguments
    parser_inspect.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save metrics.json",
    )

    parser_inspect.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    # Clean ###################################################
    clean_info = "Drop rows/cols with all zeros"
    parser_clean = subparsers.add_parser(
        "clean", description=clean_info, help=clean_info, add_help=True
    )

    # clean subparser arguments
    parser_clean.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    parser_clean.add_argument(
        "-go",
        "--genes-out",
        default=None,
        type=str,
        required=True,
        help="Output path for genes.txt",
    )
    # clean subparser arguments
    parser_clean.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="Input path for bcs.txt",
    )
    parser_clean.add_argument(
        "-bo",
        "--bcs-out",
        default=None,
        type=str,
        required=True,
        help="Output path for bcs.txt",
    )

    parser_clean.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_clean.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    # Assign ###################################################
    assign_info = "Run assignment algorithm"
    parser_assign = subparsers.add_parser(
        "assign", description=assign_info, help=assign_info, add_help=True
    )

    # assign subparser arguments
    parser_assign.add_argument(
        "-g",
        "--groups",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    # assign subparser arguments
    parser_assign.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    # assign subparser arguments
    parser_assign.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="Input path for bcs.txt",
    )
    parser_assign.add_argument(
        "-e",
        "--ec",
        default=None,
        type=str,
        required=True,
        help="Output path for bcs.txt",
    )

    parser_assign.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_assign.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    # Diff ###################################################
    diff_info = "Differential Expression"
    parser_diff = subparsers.add_parser(
        "diff", description=diff_info, help=diff_info, add_help=True
    )

    # diff subparser arguments
    parser_diff.add_argument(
        "-a",
        "--assignments",
        default=None,
        type=str,
        required=True,
        help="Input path for assignments.txt",
    )
    # diff subparser arguments
    parser_diff.add_argument(
        "-gi",
        "--genes-in",
        default=None,
        type=str,
        required=True,
        help="Input path for genes.txt",
    )
    # diff subparser arguments
    parser_diff.add_argument(
        "-bi",
        "--bcs-in",
        default=None,
        type=str,
        required=True,
        help="Input path for bcs.txt",
    )

    parser_diff.add_argument(
        "-o",
        "--output",
        default=None,
        type=str,
        required=True,
        help="Output path to save matrix",
    )

    parser_diff.add_argument(
        "matrix", metavar="matrix.mtx", type=str, help="Path to matrix.mtx file"
    )

    if len(sys.argv) == 1:
        parser.print_help()

    args = parser.parse_args()

    if args.command == "filter":
        mx_filter(
            args.matrix,
            args.bcs_in,
            args.output,
            args.bcs_out,
            sum_axis=1,
            comps=[2],
            select_axis=None,  # if you want to do the knee only on certain columns
        )
    elif args.command == "normalize":
        mx_normalize(args.matrix, args.output)
    elif args.command == "extract":
        mx_extract(
            args.matrix,
            args.genes_in,
            args.targets,
            args.genes_out,
            args.output,
        )
    elif args.command == "inspect":
        mx_inspect(args.matrix, args.output)
    elif args.command == "clean":
        mx_clean(
            args.matrix,
            args.genes_in,
            args.bcs_in,
            args.output,
            args.genes_out,
            args.bcs_out,
        )
    elif args.command == "assign":
        mx_assign_fn(
            args.matrix,
            args.bcs_in,
            args.genes_in,
            args.ec,
            args.groups,
            args.output,
        )
    elif args.command == "diff":
        mx_diff(args.matrix, args.bcs_in, args.genes_in, args.assignments, args.output)


if __name__ == "__main__":
    main()
