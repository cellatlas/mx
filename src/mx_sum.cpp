#include "Common.hpp"
#include "MTX.h"
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

    newMTXHeader header;
    // problem: if streaming in then we dont have the header..
    parseNewHeader(inf, header);

    std::string line;
    newMTXRecord prev_r, curr_r;

    int axis = opt.axis;

    // get first line
    std::getline(inf, line);
    parseNewRecord(line, prev_r, header);

    float s = prev_r.value;

    // loop through file
    while (std::getline(inf, line))
    {
        parseNewRecord(line, curr_r, header);

        if (curr_r.idx[axis] != prev_r.idx[axis])
        {
            outf << prev_r.idx[axis] << header.delim << s << '\n';
            // outf.write((char *)&s, sizeof(s));
            s = 0;
        }
        s += curr_r.value;
        prev_r = curr_r;
    }
    // write out the straggler
    outf << prev_r.idx[axis] << header.delim << s << '\n';
}