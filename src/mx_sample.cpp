#include "Common.hpp"
#include "MTX.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

#include "getopt.h"

// display
void displayProgramOptions_sample()
{
    std::cout << "Usage: mx sample [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to sort" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}

// options
static int verbose_flag;

void parseProgramOptions_sample(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:a:p";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"axis", required_argument, 0, 'a'},
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
        case 'a':
            opt.axis = atoi(optarg);
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
bool validateProgramOptions_sample(MX_opt &opt)
{
    bool ret = true;
    return ret;
}

// function

// here we implement reservoir sampling
// as described here https://florian.github.io/reservoir-sampling/

void mx_sample(MX_opt &opt)
{
    // Setup file direction IN
    std::streambuf *inbuf = nullptr;
    std::ifstream infstream;
    if (opt.stream_in)
    {
        inbuf = std::cin.rdbuf();
    }
    else
    {
        infstream.open(opt.files[0]); // only does the first file
        inbuf = infstream.rdbuf();
    }
    std::istream inf(inbuf);

    // Setup file direction OUT
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

    // perform the sampling
    MTXHeader header;
    parseHeader(inf, header);

    MTXRecord r;
    std::string line;

    while (std::getline(inf, line))
    {
        std::stringstream ss(line);
        ss >> r.row >> r.col >> r.val;
    }
}