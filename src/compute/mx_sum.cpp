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
    int axis = opt.axis;

    // filter variables
    std::string type_covariance = "full"; // <-> "diagonal"
    int n_pts = h.idx_data[axis];         // number of points
    int n_dim = 1;                        // dimension of space
    int n_itr = 200;                      // number of EM iterations
    int n_ctr = 2;                        // number of centroids
    double **data = new double *[n_pts];  // container for points
    for (int i = 0; i < n_pts; i++)
    {
        data[i] = new double[n_dim]; // make space for the data
        data[i][0] = 0;
    }

    int pidx = 0;
    size_t j = 0;
    int idx_diff = 0;
    std::string means_fn = opt.output + "means.txt";
    std::string assignments_fn = opt.output + "assignments.txt";
    MXRecord *m = new MXRecord[h.idx_data[h.ndim]];

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
            if (rc == 0)
            {
                break;
            }

            // main code change here
            for (size_t i = 0; i < rc;)
            {
                m[i + nr] = p[i];
                data[pidx][0] += p[i].value;
                j = i + 1;
                for (; j < rc; j++)
                {
                    m[j + nr] = p[j];
                    idx_diff = p[j].idx[axis] - p[i].idx[axis];
                    if (idx_diff > 0)
                    {
                        // make the value log(x + 1)
                        data[pidx][0] = std::log(data[pidx][0]);
                        pidx += idx_diff;
                        break;
                    }
                    data[pidx][0] += p[j].value;
                }
                // increase the index
                i = j;
            }
            nr += rc;
        }

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