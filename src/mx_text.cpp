#include "Common.hpp"
#include "getopt.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <map>

// display
void displayProgramOptions_text()
{
    std::cout << "Usage: mx text [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output      File for output" << std::endl
              << "-p, --pipe        Pipe output to standard out" << std::endl
              << std::endl;
}

void displayProgramOptions_shape()
{
    std::cout << "Usage: mx shape [options] mtx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output      File for output" << std::endl
              << "-p, --pipe        Pipe output to standard out" << std::endl
              << std::endl;
}

// parse
static int verbose_flag;

void parseProgramOptions_text(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:p";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
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
bool validateProgramOptions_text(MX_opt &opt)
{
    bool ret = true;
    return ret;
}

void mx_fromtext(MX_opt &opt)
{
    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    MTXHeader mtx_h;
    MTXRecord mtx_r;

    MXHeader mx_h;
    MXRecord mx_r;
    readMTXHeader(inf, mtx_h);
    MTX_to_MX_Header(mtx_h, mx_h);
    writeMXHeader(outf, mx_h);

    std::string line;
    bool readr = false;
    while (!inf.eof())
    {
        readr = readMTXRecord(inf, mtx_r, mtx_h);
        if (readr)
        {
            MTX_to_MX_Record(mtx_r, mx_r);
            writeMXRecord(outf, mx_r, mx_h);
        }
        else
        {
            break;
        }
    }
    if (!opt.stream_out)
    {
        ofs.close();
    }
}

void mx_text(MX_opt &opt)
{
    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    MTXHeader mtx_h;
    MTXRecord mtx_r;
    MXHeader mx_h;
    MXRecord mx_r;

    readMXHeader(inf, mx_h);
    MX_to_MTX_Header(mx_h, mtx_h);
    writeMTXHeader(outf, mtx_h);

    std::string line;
    size_t N = 1000;
    size_t nr = 0;
    int rc = 0;
    MXRecord *p = new MXRecord[N];
    bool readr = false;
    while (true)
    {
        readr = readManyMXRecords(inf, p, mx_h, N);
        if (readr)
        {
            rc = inf.gcount() / sizeof(MXRecord);
            if (rc == 0)
            {
                break;
            }
            for (size_t i = 0; i < rc; i++)
            {
                MX_to_MTX_Record(p[i], mtx_r); // p[i] is an mx record
                writeMTXRecord(outf, mtx_r, mtx_h);
            }
        }
        else
        {
            break;
        }
    }
    if (!opt.stream_out)
    {
        ofs.close();
    }
}

void mx_shape(MX_opt &opt)
{
    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    MXHeader mx_h;
    MXRecord mx_r;

    readMXHeader(inf, mx_h);
    for (int i = 0; i < mx_h.ndim; i++)
    {
        outf << mx_h.idx_data[i] << '\t';
    }
    outf << '\n';

    if (!opt.stream_out)
    {
        ofs.close();
    }
}