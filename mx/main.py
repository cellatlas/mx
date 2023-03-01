from . import __version__
import sys
import argparse
import logging

from .mx_assign import setup_mx_assign_args, validate_mx_assign_args
from .mx_clean import setup_mx_clean_args, validate_mx_clean_args
from .mx_diff import setup_mx_diff_args, validate_mx_diff_args
from .mx_extract import setup_mx_extract_args, validate_mx_extract_args
from .mx_filter import setup_mx_filter_args, validate_mx_filter_args
from .mx_inspect import setup_mx_inspect_args, validate_mx_inspect_args
from .mx_normalize import setup_mx_normalize_args, validate_mx_normalize_args
from .mx_split import setup_mx_split_args, validate_mx_split_args


def main():
    # setup parsers
    parser = argparse.ArgumentParser(
        description=f"mx {__version__}: handle with mx files"
    )
    parser.add_argument(
        "--verbose", help="Print debugging information", action="store_true"
    )
    subparsers = parser.add_subparsers(
        dest="command",
        metavar="<CMD>",
    )

    # Setup the arguments for all subcommands
    command_to_parser = {
        "assign": setup_mx_assign_args(subparsers),
        "clean": setup_mx_clean_args(subparsers),
        "diff": setup_mx_diff_args(subparsers),
        "extract": setup_mx_extract_args(subparsers),
        "filter": setup_mx_filter_args(subparsers),
        "inspect": setup_mx_inspect_args(subparsers),
        "normalize": setup_mx_normalize_args(subparsers),
        "split": setup_mx_split_args(subparsers),
    }

    # Show help when no arguments are given
    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)
    if len(sys.argv) == 2:
        if sys.argv[1] in command_to_parser:
            command_to_parser[sys.argv[1]].print_help(sys.stderr)
        else:
            parser.print_help(sys.stderr)
        sys.exit(1)

    args = parser.parse_args()

    # setup logging
    logging.basicConfig(
        format="[%(asctime)s] %(levelname)7s %(message)s",
        level=logging.DEBUG if args.verbose else logging.INFO,
    )

    # Setup validator and runner for all subcommands (validate and run if valid)
    COMMAND_TO_FUNCTION = {
        "assign": validate_mx_assign_args,
        "clean": validate_mx_clean_args,
        "diff": validate_mx_diff_args,
        "extract": validate_mx_extract_args,
        "filter": validate_mx_filter_args,
        "inspect": validate_mx_inspect_args,
        "normalize": validate_mx_normalize_args,
        "split": validate_mx_split_args,
    }
    COMMAND_TO_FUNCTION[sys.argv[1]](parser, args)


if __name__ == "__main__":
    main()
