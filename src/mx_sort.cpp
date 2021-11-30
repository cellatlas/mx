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

// bool cmp_row(const MTXRecord &a, const MTXRecord &b)
// {
//     if (a.row == b.row)
//     {
//         return (a.col < b.col);
//     }
//     else
//     {
//         return (a.row < b.row);
//     }
// }

// bool cmp_col(const MTXRecord &a, const MTXRecord &b)
// {
// if (a.col == b.col)
// {
//     return (a.row < b.row);
// }
// else
// {
//     return (a.col < b.col);
// }
// }

bool cmp_val(const MTXRecord &a, const MTXRecord &b)
{
    return (a.value < b.value);
}

// TODO, recursive strategy for comparator
class cmp
{
    int axis;
    int ndim;

public:
    cmp(int a, int nd) : axis(a), ndim(nd) {}

    bool operator()(const MTXRecord &a, const MTXRecord &b)
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
    // Setup file direction in
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

    // Setup file direction out
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

    MTXHeader header;
    parseMTXHeader(inf, header);

    int axis = opt.axis;
    auto cmp_func = cmp(axis, header.ndim);
    if (axis == -1)
    {
        auto cmp_func = cmp_val;
    }

    MTXRecord r;
    std::vector<MTXRecord> vec;
    std::string line;

    while (std::getline(inf, line))
    {
        parseMTXRecord(line, r, header);
        vec.push_back(r);
    }

    std::sort(vec.begin(), vec.end(), cmp_func);

    if (!opt.stream_out)
    {
        writeMTXHeader(outf, header);
    }

    // entries
    for (int i = 0; i < vec.size(); i++)
    {
        writeMTXRecord(outf, vec[i], header);
    }
}