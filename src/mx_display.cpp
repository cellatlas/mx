#include <iostream>

void displayProgramOptions_MX()
{
    std::cout << "Usage: mx <CMD> [arguments] ..." << std::endl
              << std::endl
              << "Where <CMD> can be one of: " << std::endl
              << std::endl
              << "view              View matrix" << std::endl
              << "shape             Print shape of matrix" << std::endl
              << "sort              Sort matrix" << std::endl
              << "sum               Sum elements across axis" << std::endl
              << "extract           Extract elements across axis" << std::endl
              << std::endl
              << "Running mx <CMD> without arguments prints usage information for <CMD>"
              << std::endl
              << std::endl;
}
void displayProgramOptions_view()
{
    std::cout << "Usage: mx view [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output      File for output" << std::endl
              << "-p, --pipe        Pipe output to standard out" << std::endl
              << std::endl;
}

void displayProgramOptions_shape()
{
    std::cout << "Usage: mx shape [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output      File for output" << std::endl
              << "-p, --pipe        Pipe output to standard out" << std::endl
              << std::endl;
}

void displayProgramOptions_sum()
{
    std::cout << "Usage: mx sum [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to sum" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}

void displayProgramOptions_sort()
{
    std::cout << "Usage: mx sort [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to sort" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}

void displayProgramOptions_extract()
{
    std::cout << "Usage: mx extract [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<int>      Axis along which to extract" << std::endl
              << "-i, --index=<int-int> Indices to slice, left closed right open interval" << std::endl
              << "                      e.g. one of [0, -3, 1-, 2-5]" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}