#include "Common.hpp"
#include "MTX.h"
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
              << "-a, --axis=<integer>  Axis along which to sort" << std::endl
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

bool cmp_row(const MTXRecord &a, const MTXRecord &b)
{
    if (a.row == b.row)
    {
        return (a.col < b.col);
    }
    else
    {
        return (a.row < b.row);
    }
}

bool cmp_col(const MTXRecord &a, const MTXRecord &b)
{
    if (a.col == b.col)
    {
        return (a.row < b.row);
    }
    else
    {
        return (a.col < b.col);
    }
}

bool cmp_val(const MTXRecord &a, const MTXRecord &b)
{
    return (a.val < b.val);
}

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

    int naxis = 3;

    std::function<bool(const MTXRecord &, const MTXRecord &)> cmp[] = {&cmp_row, &cmp_col, &cmp_val};
    int axis = opt.axis;
    // TODO: get length of cmp
    if (axis == -1)
    {
        // sort by value
        axis = naxis - 1;
    }

    MTXHeader header;
    parseHeader(inf, header);

    MTXRecord r;
    std::vector<MTXRecord> vec;
    std::string line;

    while (std::getline(inf, line))
    {
        std::stringstream ss(line);
        ss >> r.row >> r.col >> r.val;
        vec.push_back(r);
    }

    std::sort(vec.begin(), vec.end(), cmp[axis]);

    writeHeader(outf, header);

    // entries
    for (int i = 0; i < vec.size(); i++)
    {
        outf << vec[i].row << ' ' << vec[i].col << ' ' << vec[i].val << '\n';
    }
}