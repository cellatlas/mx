#include "Common.hpp"
#include "getopt.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional> // std::function
#include <algorithm>  // std::sort

// display
void displayProgramOptions_sort()
{
    std::cout << "Usage: mx sort [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to sort (default = 0)" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}

// program options
static int verbose_flag;

void parseProgramOptions_sort(int argc, char *argv[], MX_opt &opt)
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
bool validateProgramOptions_sort(MX_opt &opt)
{
    bool ret = true;
    return ret;
}
// function

bool cmp_val(const MXRecord &a, const MXRecord &b)
{
    return (a.value < b.value);
}

// // TODO, recursive strategy for comparator
class cmp
{
    int axis;
    int ndim;

public:
    cmp(int a, int nd) : axis(a), ndim(nd) {}

    bool operator()(const MXRecord &a, const MXRecord &b)
    {
        // want this to go axis >> axis + 1 % %ndim >> axis +2 % ndim
        // axis + i where i = 0 .. ndim
        // logic uses axis
        // for (int i = axis; i < (ndim + axis); i++)
        // {
        //     int m = i % ndim;
        // }
        if (a.idx[axis % ndim] == b.idx[axis % ndim])
        {
            return (a.idx[(axis + 1) % ndim] < b.idx[(axis + 1) % ndim]);
        }
        else
        {
            return (a.idx[axis % ndim] < b.idx[axis % ndim]);
        }
    }
};

void mx_sort(MX_opt &opt)
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
    auto cmp_func = cmp(axis, h.ndim);
    if (axis == -1)
    {
        auto cmp_func = cmp_val;
    }

    // vector to store records for sorting
    std::vector<MXRecord> vec;

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
                // add each record to vector
                vec.push_back(p[i]);
            }
        }

        // no more records or error out then break
        else
        {
            break;
        }
    }

    // sort records
    std::sort(vec.begin(), vec.end(), cmp_func);

    // write sorted records to ostream
    for (int i = 0; i < vec.size(); i++)
    {
        writeMXRecord(outf, vec[i], h);
    }

    // close ostream
    if (!opt.stream_out)
    {
        ofs.close();
    }
    std::cerr << "Sorted " << nr << " records along axis " << axis << std::endl;
}