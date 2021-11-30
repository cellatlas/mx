#include "Common.hpp"
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

// void file_direction(std::string &ifn, std::string &ofn, bool &stream_in, bool &stream_out, std::istream *&inf_ptr, std::ostream *&outf_ptr)
// {
//     // IN
//     std::ifstream ifs;
//     std::streambuf *ibuf = nullptr;
//     if (stream_in)
//     {
//         ibuf = std::cin.rdbuf();
//     }
//     else
//     {
//         ifs.open(ifn, std::ios::in);
//         ibuf = ifs.rdbuf();
//     }
//     std::istream inf(ibuf);
//     inf_ptr = &inf;

//     // OUT
//     std::ofstream ofs;
//     std::streambuf *obuf = nullptr;
//     if (stream_out)
//     {
//         obuf = std::cout.rdbuf();
//     }
//     else
//     {
//         ofs.open(ofn);
//         obuf = ofs.rdbuf();
//     }
//     std::ostream outf(obuf);
//     outf_ptr = &outf;
//     return;
// }

void mx_sum(MX_opt &opt)
{
    // Setup file direction in and out
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    int axis = opt.axis;

    size_t nr = 0;
    size_t N = 100000;
    MTXRecord *p = new MTXRecord[N];
    char *buf = new char[N];

    // problem: if streaming in then we dont have the header..
    MTXHeader header;
    MTXRecord prev_r, curr_r;
    std::string line;

    parseMTXHeader(inf, header);
    std::getline(inf, line); // get first line
    parseMTXRecord(line, prev_r, header);

    int32_t s = prev_r.value;

    while (true)
    {
        inf.read((char *)p, N * sizeof(MTXRecord));
        size_t rc = inf.gcount() / sizeof(MTXRecord);
        if (rc == 0)
        {
            break;
        }
        for (size_t i = 0; i < rc; i++)
        {
            writeMTXRecord(std::cout, p[i], header);
            // parseMTXRecord(p[i], curr_r, header);
            // if (curr_r.idx[axis] != prev_r.idx[axis])
            // {
            //     outf << prev_r.idx[axis] << header.delim << s << '\n';
            //     // outf.write((char *)&s, sizeof(s));
            //     s = 0;
            // }
            // s += curr_r.value;
            // prev_r = curr_r;
            if (i > 10)
            {
                break;
            }
        }
        break;
    }

    // // loop through file
    // while (std::getline(inf, line))
    // {
    // }
    // // write out the straggler
    // // outf << prev_r.idx[axis] << header.delim << s << '\n';
}