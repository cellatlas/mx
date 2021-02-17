#include "Common.hpp"
#include "MTX.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>

#include "getopt.h"

// display
void displayProgramOptions_sample()
{
    std::cout << "Usage: mx sample [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output              File for output" << std::endl
              << "-a, --axis=<integer>      Axis along which to sample" << std::endl
              << "-k, --ksamples=<integer>  Number of elements to sample" << std::endl
              << "-p, --pipe                Pipe output to standard out" << std::endl
              << std::endl;
}

// options
static int verbose_flag;

void parseProgramOptions_sample(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:a:pk:";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"axis", required_argument, 0, 'a'},
            {"pipe", no_argument, 0, 'p'},
            {"ksamples", required_argument, 0, 'k'},
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
        case 'k':
            opt.k_samples = atoi(optarg);
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
// helpful post on random number generation in cpp
// https://stackoverflow.com/a/7114482/13731947

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

    // perform the sampling
    MTXHeader header; // defaults set in MTX.h
    parseHeader(inf, header);

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

    MTXRecord r;
    std::string line;

    MTXRecord *buffer = new MTXRecord[opt.k_samples]; // dynamically allocated
    int nr = 0;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> rd_idx(0, opt.k_samples - 1); // [0, k-1] random index

    while (std::getline(inf, line))
    {
        std::stringstream ss(line);
        ss >> r.row >> r.col >> r.val;

        // place first k records into buffer
        if (nr < opt.k_samples)
        {
            buffer[nr] = r;
        }
        else
        {
            std::uniform_int_distribution<std::mt19937::result_type> dist(0, nr); // distribution in range [0, nr]
            // determine if random number generator is between [0, k) (replace) or [k, i) (dont replace)
            if (dist(rng) < opt.k_samples)
            {
                buffer[rd_idx(rng)] = r;
            }
        }
        nr++;
    }

    // TODO fix header
    // write the new matrix
    // header

    writeHeader(outf, header);

    for (int i = 0; i < opt.k_samples; i++)
    {
        outf << buffer[i].row << ' ' << buffer[i].col << ' ' << buffer[i].val << '\n';
    }

    delete[] buffer;
}