#include "Common.hpp"
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
              << "                      e.g. one of [0, -3, 1-, 2-5]" << std::endl
              << "-f, --file            single column file" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
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
// TODO handle when the index changes
void mx_extract_file(MX_opt &opt)
{
    // must be sorted by axis
    // Setup file direction in
    std::cout << "extract file" << std::endl;
    // std::streambuf *inbuf = nullptr;
    // std::ifstream infstream;
    // if (opt.stream_in)
    // {
    //     inbuf = std::cin.rdbuf();
    // }
    // else
    // {
    //     infstream.open(opt.files[0]); // only does the first file
    //     inbuf = infstream.rdbuf();
    // }
    // std::istream inf(inbuf);

    // // Setup file direction out
    // std::streambuf *buf = nullptr;
    // std::ofstream of;
    // if (opt.stream_out)
    // {
    //     buf = std::cout.rdbuf();
    // }
    // else
    // {
    //     of.open(opt.output);
    //     buf = of.rdbuf();
    // }
    // std::ostream outf(buf);

    // //////////////////
    // int axis = opt.axis;

    // newMTXHeader h, nh; // nh = new h
    // newMTXRecord rp, r; // rp = r previous
    // int32_t nw = 0;     // num of (axis) elements written

    // parseMTXHeader(inf, h);
    // nh = h;

    // // set the idx_data equal to zero (there must be a better way to do this)
    // for (int i = 0; i < nh.ndim; i++)
    // {
    //     // n cols etc = 0
    //     nh.idx_data[i] = 0;
    // }
    // // number of nonzero = 0
    // nh.idx_data[nh.ndim] = 0;

    // // read in file indices
    // std::unordered_map<int32_t, int32_t>
    //     extractList;
    // parseExtractList(opt.file, extractList);
    // // printExtractList(extractList);

    // if (!opt.stream_out)
    // {
    //     writeMTXHeader(outf, h);
    // }
    // std::string line;

    // // initial record (rp)
    // std::getline(inf, line);
    // parseMTXRecord(line, rp, nh);

    // while (std::getline(inf, line))
    // {

    //     // new record
    //     parseMTXRecord(line, r, nh);

    //     // search for rp
    //     auto it = extractList.find(rp.idx[axis]);
    //     if (it == extractList.end())
    //     {
    //     }
    //     else
    //     {
    //         // if it exists, write it
    //         nw++;
    //         int old = rp.idx[axis];
    //         trackIndex(rp, nh, nw, axis);
    //         writeMTXRecord(outf, rp, nh);
    //         while (old == r.idx[axis])
    //         {
    //             trackIndex(r, nh, nw, axis);
    //             writeMTXRecord(outf, r, nh);
    //             rp = r;
    //             old = rp.idx[axis];
    //             std::getline(inf, line);
    //             parseMTXRecord(line, r, nh);
    //         }
    //     }
    //     rp = r;
    // }
    // if (!opt.stream_out)
    // {
    //     // fix the header
    //     outf.seekp(0);
    //     writeMTXHeader(outf, nh);
    // }
}

void mx_extract_index(MX_opt &opt)
{
    std::cout << "extract by index" << std::endl;
    //     // must be sorted by axis
    //     // Setup file direction in
    //     std::streambuf *inbuf = nullptr;
    //     std::ifstream infstream;
    //     if (opt.stream_in)
    //     {
    //         inbuf = std::cin.rdbuf();
    //     }
    //     else
    //     {
    //         infstream.open(opt.files[0]); // only does the first file
    //         inbuf = infstream.rdbuf();
    //     }
    //     std::istream inf(inbuf);

    //     // Setup file direction out
    //     std::streambuf *buf = nullptr;
    //     std::ofstream of;
    //     if (opt.stream_out)
    //     {
    //         buf = std::cout.rdbuf();
    //     }
    //     else
    //     {
    //         of.open(opt.output);
    //         buf = of.rdbuf();
    //     }
    //     std::ostream outf(buf);

    //     ///////////////////
    //     int axis = opt.axis;

    //     newMTXHeader header;
    //     newMTXRecord r;

    //     parseMTXHeader(inf, header);

    //     int lower = opt.range.first;
    //     int upper = opt.range.second;
    //     if (upper == -1)
    //     {
    //         if (axis == 0)
    //         {
    //             upper = header.nrows;
    //         }
    //         else if (axis == 1)
    //         {
    //             upper = header.ncols;
    //         }
    //     }

    //     // TODO write header later
    //     writeMTXHeader(outf, header);
    //     std::string line;
    //     std::unordered_set<int> cols;

    //     // get the first line
    //     std::getline(inf, line);
    //     std::stringstream ss(line);
    //     ss >> r.row >> r.col >> r.val;

    //     std::vector<int> idx, prev_idx;
    //     idx.push_back(r.row);
    //     idx.push_back(r.col);
    //     prev_idx = idx;
    //     cols.insert(r.col);

    //     std::vector<int> vsize;
    //     vsize.push_back(0);
    //     vsize.push_back(0);
    //     vsize.push_back(0);

    //     // Get first line
    //     // subtract 1 since MTX is 1-indexed but I use 0-index for lower-upper range
    //     if (idx[axis] - 1 >= lower && idx[axis] - 1 < upper)
    //     {
    //         vsize[axis]++;
    //         outf << r.row << ' ' << r.col << ' ' << r.val << '\n';

    //         // add to col (row)
    //         cols.insert(r.col);
    //         vsize[(axis + 1) % 2]++;
    //         vsize[vsize.size() - 1]++;
    //     }

    //     // get the rest of the lines
    //     while (std::getline(inf, line))
    //     {
    //         std::stringstream ss(line);
    //         ss >> r.row >> r.col >> r.val;
    //         idx[0] = r.row;
    //         idx[1] = r.col;

    //         // subtract 1 since MTX is 1-indexed but I use 0-index for lower-upper range
    //         if (idx[axis] - 1 >= lower && idx[axis] - 1 < upper)
    //         {

    //             if (idx[axis] != prev_idx[axis])
    //             {
    //                 vsize[axis]++;
    //             }

    //             // check if we've seen this col before
    //             auto it = cols.find(r.col);
    //             if (it == cols.end())
    //             {
    //                 cols.insert(r.col);
    //                 vsize[((1 + axis) % 2)]++;
    //             }
    //             prev_idx = idx;
    //             outf << r.row << ' ' << r.col << ' ' << r.val << '\n';
    //             vsize[vsize.size() - 1]++;
    //         }
    //     }
    //     // get straggler
    //     // std::cout << vsize[axis] << ", " << vsize[((1 + axis) % 2)] << ", " << vsize[vsize.size() - 1] << std::endl;

    //     // fix the header
}

// TODO update the header with new nrows/ncols/nzeros
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