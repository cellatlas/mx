#include "../Common.hpp"
#include "getopt.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional> // std::function
#include <algorithm>  // std::sort

// display
void displayProgramOptions_collapse()
{
    std::cout << "Usage: mx collapse [options] sorted mx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to collapse (default = 0)" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}

// program options
static int verbose_flag;

void parseProgramOptions_collapse(int argc, char *argv[], MX_opt &opt)
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
bool validateProgramOptions_collapse(MX_opt &opt)
{
    bool ret = true;
    return ret;
}
// function

void mx_collapse(MX_opt &opt)
{
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
    writeMXHeader(outf, h);

    // pick comparator based on axis
    int axis = opt.axis;

    // setup collapse specific variables
    uint32_t element_idx = 0;
    uint32_t prev_idx = 0;

    // setup bulk parsing
    std::string line;
    size_t N = 256;
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
            size_t i;
            for (i = 0; i < rc;)
            {
                // change the axis index of the first dude
                if (prev_idx != p[i].idx[axis])
                {
                    prev_idx = p[i].idx[axis];
                    element_idx += 1;
                }
                p[i].idx[axis] = element_idx;
                // write the dude
                writeMXRecord(outf, p[i], h);

                size_t j = i + 1;
                for (; j < rc; j++)
                {
                    // if the axis index differ, break
                    if (prev_idx != p[j].idx[axis])
                    {
                        prev_idx = p[j].idx[axis];
                        element_idx += 1;
                        break;
                    }

                    // change the axis index of the subsequent dudes
                    p[j].idx[axis] = element_idx;
                    // write the dude
                    writeMXRecord(outf, p[j], h);
                }
                i = j;
            }
        }

        // no more records or error out then break
        else
        {
            break;
        }
    }
    // update the header
    // close ostream
    if (!opt.stream_out)
    {
        h.idx_data[axis] = element_idx;
        outf.seekp(0);
        writeMXHeader(outf, h);
        ofs.close();
    }
    std::cerr << nr << " records collapsed along axis " << axis << std::endl;
}