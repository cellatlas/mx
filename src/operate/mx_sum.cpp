#include "../Common.hpp"

#include "getopt.h"

#include <fstream>  // std::ifstream
#include <string>   // string
#include <sstream>  // stringstream
#include <iostream> // std::cout
#include <vector>

// display
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
// parse
static int verbose_flag;

void parseProgramOptions_sum(int argc, char *argv[], MX_opt &opt)
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
bool validateProgramOptions_sum(MX_opt &opt)
{
    bool ret = true;
    return ret;
}

// function

void mx_sum(MX_opt &opt)
{
    std::cerr << ".mx file must be sorted along axis " << opt.axis << std::endl;
    /// Notes for Angel
    // for summing along axis i, mx file must be sorted first
    // along that axis
    // read in header
    // read in mtx records
    // iterate through records
    /// End notes for Angel

    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    // read in header
    MXHeader h;
    MXRecord r;
    readMXHeader(inf, h);

    // set axis along which to sum
    int axis = opt.axis;

    // storage for summed value
    int32_t sum;

    // setup bulk parsing
    std::string line;
    size_t N = 1000;
    size_t nr = 0;
    int rc = 0;
    MXRecord *p = new MXRecord[N];
    bool readr = false;
    while (true)
    {
        // read in records
        readr = readManyMXRecords(inf, p, h, N);
        if (readr)
        {
            rc = inf.gcount() / sizeof(MXRecord);
            nr += rc;
            if (rc == 0)
            {
                break;
            }

            // main code change here
            for (size_t i = 0; i < rc; i++)
            {
                // do summing here
                // if p[i].idx[axis] is diff from p[i+1].idx[axis]
                // write sum
                // restart sum
                // else sum += p[i].idx[axis]
            }
        }

        // no more records or error out then break
        else
        {
            break;
        }
    }

    // close ostream
    if (!opt.stream_out)
    {
        ofs.close();
    }
    std::cerr << "Parsed " << nr << " records" << std::endl;
}