#include "../Common.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <utility> // std::pair, std::make_pair
#include <unordered_map>

#include "getopt.h"

// display
void displayProgramOptions_extract()
{
    std::cout << "Usage: mx extract [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<int>      Axis along which to extract" << std::endl
              << "-i, --index=<int-int> Indices to slice, left closed right open interval" << std::endl
              << "                      0-indexed, e.g. one of [0, -3, 1-, 2-5]" << std::endl
              << "-f, --file            Single column file of indices to extract, 1-indexed" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl
              << "** Note that header will not be updated if piping to standard out **" << std::endl
              << std::endl;
}

// parse
static int verbose_flag;

void parseProgramOptions_extract(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:a:i:f:p";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"axis", required_argument, 0, 'a'},
            {"index", required_argument, 0, 'i'},
            {"file", optional_argument, 0, 'f'},
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
        case 'i':
            opt.index = optarg;
            break;
        case 'f':
            opt.file = optarg;
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
bool validateProgramOptions_extract(MX_opt &opt)
{
    bool ret = true;

    if (!opt.stream_out && opt.output.empty())
    {
        ret = false;
        std::cerr << "[error] no output specified" << std::endl;
    }
    else if (opt.stream_out && !opt.output.empty())
    {
        ret = false;
        std::cerr << "[error] specify either -p or -o but not both" << std::endl;
    }

    if (opt.index.empty())
    {
        if (opt.file.empty())
        {
            ret = false;
            std::cerr << "[error] no index or file specified" << std::endl;
        }
    }
    else
    {
        if (!opt.file.empty())
        {
            ret = false;
            std::cerr << "[error] cannot specify index and file" << std::endl;
        }
        else
        {

            char delimiter = '-';
            size_t pos = opt.index.find(delimiter);
            int lower, upper;

            // if delimiter not found
            if (pos == std::string::npos)
            {
                lower = atoi(opt.index.c_str());
                upper = lower + 1;
            }
            else // else split at delimiter and set bounds
            {
                if (pos == 0) // - at beginning of string ie "-1432"
                {
                    lower = 0;
                    upper = atoi(opt.index.substr(pos + 1, opt.index.size()).c_str());
                }
                else if (pos == opt.index.size() - 1) // the - is at end of string ie "12-"
                {
                    lower = atoi(opt.index.substr(0, pos).c_str());
                    upper = -1;
                }
                else // the - is in between two numbers 43-100
                {
                    lower = atoi(opt.index.substr(0, pos).c_str());
                    upper = atoi(opt.index.substr(pos + 1, opt.index.size()).c_str());
                }
            }

            if (upper == -1 || lower < upper)
            {
                opt.range = std::make_pair(lower, upper);
                // std::cout << lower << ", " << upper << '\n';
            }
            else
            {
                std::cerr << "[error] lower must be strictly smaller than upper" << std::endl;
                ret = false;
            }
        }
    }
    return ret;
}

// command
void mx_extract_file(MX_opt &opt)
{
    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    // read in header
    MXHeader h, nh;
    MXRecord r;
    readMXHeader(inf, h);
    writeMXHeader(outf, h);

    // set axis along which to extract
    int axis = opt.axis;

    // read in extract list
    std::unordered_map<int32_t, int32_t>
        extractList;
    parseExtractList(opt.file, extractList);
    int32_t nw = 0;
    int32_t n_ridx = 0;
    int32_t n_cidx = 0;
    // int32_t n_idx[2] = {0, 0};
    // only necessary if updated row and column index
    // row and column indices to lookup
    // std::unordered_set<uint32_t> ridx;
    // std::unordered_set<uint32_t> cidx;
    // std::unordered_set<uint32_t> idx[2];
    // idx[0] = ridx;
    // idx[1] = cidx;

    // setup bulk parsing
    std::string line;
    size_t N = std::pow(2, 10); // 1024
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
                // check if index of mx record is in extract list
                auto it = extractList.find(p[i].idx[axis]);
                if (it != extractList.end())
                {
                    // write mx record
                    writeMXRecord(outf, p[i], h);
                    nw++;

                    // only necessary if updating row and column index
                    // // check if the indices have been seen before
                    // // this allows us to update the header
                    // for (int j = 0; j < h.ndim; j++)
                    // {
                    //     if (idx[j].find(p[i].idx[j]) == idx[j].end())
                    //     {
                    //         // add the index to the set and update the count
                    //         // if we havent seen it before
                    //         idx[j].insert(p[i].idx[j]);
                    //         n_idx[j]++;
                    //     }
                    // }
                }
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
        // only necessary if updating row and column index
        // update row, column, and ndim in the header
        // for (int i = 0; i < h.ndim; i++)
        // {
        //     h.idx_data[i] = n_idx[i];
        // }
        h.idx_data[h.ndim] = nw;
        outf.seekp(0);
        writeMXHeader(outf, h);
        ofs.close();
    }
    std::cerr << "Parsed " << nr << " records" << std::endl;
    std::cerr << "Wrote  " << nw << " records" << std::endl;
}

void mx_extract_index(MX_opt &opt)
{

    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    // read in header
    MXHeader h, nh;
    MXRecord r;
    readMXHeader(inf, h);
    writeMXHeader(outf, h);

    // set axis along which to extract
    int axis = opt.axis;

    // read in extract list
    std::unordered_map<int32_t, int32_t>
        extractList;
    parseExtractList(opt.file, extractList);
    int32_t nw = 0;

    int lower = opt.range.first;
    int upper = opt.range.second;
    if (upper == -1)
    {
        upper = h.idx_data[axis];
    }

    // setup bulk parsing
    std::string line;
    size_t N = std::pow(2, 10); // 1024
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
                // check if index of mx record is in range
                // subtract 1 since MTX is 1-indexed but I use 0-index for lower-upper range
                if (p[i].idx[axis] - 1 >= lower && p[i].idx[axis] - 1 < upper)
                {
                    // write mx record
                    writeMXRecord(outf, p[i], h);
                    nw++;
                }
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
        h.idx_data[h.ndim] = nw;
        outf.seekp(0);
        writeMXHeader(outf, h);
        ofs.close();
    }
    std::cerr << nr << " records read" << std::endl;
    std::cerr << nw << " records written" << std::endl;
}

// extract requires the header to be fixed
void mx_extract(MX_opt &opt)
{
    if (!opt.index.empty())
    {
        mx_extract_index(opt);
    }
    else if (!opt.file.empty())
    {
        mx_extract_file(opt);
    }
}