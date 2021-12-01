#include "../Common.hpp"

#include "getopt.h"

#include <fstream>  // std::ifstream
#include <string>   // string
#include <sstream>  // stringstream
#include <iostream> // std::cout
#include <vector>
#include <queue>

// display
void displayProgramOptions_multiply()
{
    std::cout << "Usage: mx multiply [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to multiply" << std::endl
              << "-v, --value=<float>   Value to multiply by (default=1.0)" << std::endl
              << "-f, --file            Optional: File with values (same shape as axis)" << std::endl
              << "-i, --inverse         Optional: 1/value" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}
// parse
static int verbose_flag;

void parseProgramOptions_multiply(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:a:v:f:ip";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"axis", required_argument, 0, 'a'},
            {"value", required_argument, 0, 'v'},
            {"file", required_argument, 0, 'f'},
            {"inverse", no_argument, 0, 'i'},
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
        case 'v':
            opt.value = atof(optarg);
            break;
        case 'f':
            opt.file = optarg;
            break;
        case 'i':
            opt.inverse = true;
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
bool validateProgramOptions_multiply(MX_opt &opt)
{
    bool ret = true;
    if (!opt.file.empty())
    {
        if (opt.value != 1)
        {
            ret = false;
            std::cerr << "[error] can multiply by either a file or value, not both" << std::endl;
        }
    }
    else if (opt.file.empty())
    {
        if (opt.value == 0)
        {
            ret = false;
            std::cerr << "[error] value must be non-zero" << std::endl;
        }
    }
    return ret;
}

// function
void mx_multiply_value(MX_opt &opt)
{
    std::cout << "multiple value" << std::endl;
    // // Setup file direction in
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

    // newMTXHeader header;
    // parseMTXHeader(inf, header);
    // if (!opt.stream_out)
    // {
    //     writeMTXHeader(outf, header);
    // }

    // std::string line;
    // newMTXRecord r;

    // int axis = opt.axis;
    // float v = opt.value;

    // if (opt.inverse)
    // {
    //     v = 1 / v;
    // }
    // while (std::getline(inf, line))
    // {
    //     parseMTXRecord(line, r, header);
    //     r.value = r.value * v;
    //     writeMTXRecord(outf, r, header);
    // }
}

void mx_multiply_file(MX_opt &opt)
{
    std::cout << "multiply file" << std::endl;
    // int nf = 0; // number of elements in file
    // std::queue<float> q;
    // if (!opt.file.empty())
    // {
    //     std::ifstream fin;
    //     fin.open(opt.file);
    //     std::string line;
    //     float val;
    //     while (std::getline(fin, line))
    //     {
    //         nf++;
    //         std::stringstream ss(line);
    //         ss >> val;

    //         if (val == 0)
    //         {
    //             std::cerr << "[warning] value equals 0.. setting equal to 1" << std::endl;
    //             val = 1;
    //         }

    //         if (opt.inverse)
    //         {
    //             val = 1 / val;
    //         }
    //         q.push(val);
    //     }
    // }
    // // std::cout << "There are " << nf << " number of values" << std::endl;

    // // Setup file direction in
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

    // MTXHeader header;
    // parseMTXHeader(inf, header);
    // if (!opt.stream_out)
    // {
    //     writeMTXHeader(outf, header);
    // }

    // std::string line;
    // MTXRecord prev_r, curr_r;

    // int axis = opt.axis;

    // // get first line
    // std::getline(inf, line);
    // parseMTXRecord(line, prev_r);
    // curr_r = prev_r;

    // float v = q.front();
    // q.pop();

    // prev_r.val = prev_r.val * v;
    // writeMTXRecord(outf, prev_r);

    // while (std::getline(inf, line))
    // {
    //     prev_r = curr_r;
    //     parseMTXRecord(line, curr_r);

    //     if (curr_r.row != prev_r.row)
    //     {
    //         v = q.front();
    //         q.pop();
    //     }
    //     curr_r.val = curr_r.val * v;
    //     writeMTXRecord(outf, curr_r);
    // }
}

void mx_multiply(MX_opt &opt)
{
    if (!opt.file.empty())
    {
        mx_multiply_file(opt);
    }
    else
    {
        mx_multiply_value(opt);
    }
}