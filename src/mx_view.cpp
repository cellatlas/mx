#include "mx_view.h"
#include "MTX.h"
#include "getopt.h"

#include <sstream>
#include <iostream>
#include <fstream>

// display
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
// parse
static int verbose_flag;

void parseProgramOptions_view(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:p";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"pipe", no_argument, 0, 'p'},
            {0, 0, 0, 0}};

    int c;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'o':
            opt.output = optarg;
            break;
        case 'p':
            opt.stream_out = true;
            break;
        default:
            break;
        }
    }
    if (verbose_flag)
    {
        std::cout << "Verbose flag is set" << std::endl;
    }

    // the rest of the arguments are files
    while (optind < argc)
    {
        opt.files.push_back(argv[optind++]);
    }

    if (opt.files.size() == 1 && opt.files[0] == "-")
    {
        opt.stream_in = true;
    }
}

// validate
bool validateProgramOptions_view(MX_opt &opt)
{
    bool ret = true;
    return ret;
}

// function

void mx_shape(MX_opt &opt)
{
    // If there is no output file write to stdout
    std::streambuf *buf = nullptr;
    std::ofstream of;
    if (opt.stream_out)
    {
        buf = std::cout.rdbuf();
    }
    else
    {
        of.open(opt.output);
        buf = of.rdbuf();
    }
    std::ostream outf(buf);

    // loop through the input files
    for (int i = 0; i < opt.files.size(); i++)
    {
        // Setup file direction in
        std::streambuf *inbuf = nullptr;
        std::ifstream infstream;
        if (opt.stream_in)
        {
            inbuf = std::cin.rdbuf();
        }
        else
        {
            infstream.open(opt.files[i]); // only does the first file
            inbuf = infstream.rdbuf();
        }
        std::istream inf(inbuf);
        MTXHeader header;

        // Parse header
        parseHeader(inf, header);

        outf << header.nrows << "\t" << header.ncols << "\t" << header.nnzero << std::endl;
    }
}

void mx_view(MX_opt &opt)
{

    std::streambuf *buf = nullptr;
    std::ofstream of;
    if (opt.stream_out)
    {
        buf = std::cout.rdbuf();
    }
    else
    {
        of.open(opt.output);
        buf = of.rdbuf();
    }
    std::ostream outf(buf);

    for (int i = 0; i < opt.files.size(); i++)
    {
        // Setup file direction in
        std::streambuf *inbuf = nullptr;
        std::ifstream infstream;
        if (opt.stream_in)
        {
            inbuf = std::cin.rdbuf();
        }
        else
        {
            infstream.open(opt.files[i]);
            inbuf = infstream.rdbuf();
        }
        std::istream inf(inbuf);
        MTXHeader header;

        // Parse header
        parseHeader(inf, header);

        // header
        outf << header.format << '\n'
             << "%\n";
        outf << header.nrows << ' ' << header.ncols << ' ' << header.nnzero << '\n';

        // Parse the rest of the matrix
        std::string line;

        MTXRecord r;
        while (std::getline(inf, line))
        {
            std::stringstream ss(line);
            ss >> r.row >> r.col >> r.val;

            outf << r.row << ' ' << r.col << ' ' << r.val << '\n';
        }
    }
}