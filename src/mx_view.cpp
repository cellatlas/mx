#include "mx_view.h"
#include "MTX.h"

#include <sstream>
#include <iostream>
#include <fstream>

void mx_shape(MX_opt &opt)
{
    // If there is no output file write to stdout
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

    // loop through the input files
    for (int i = 0; i < opt.files.size(); i++)
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
            infstream.open(opt.files[i]); // only does the first file
            inbuf = infstream.rdbuf();
        }
        std::istream inf(inbuf);
        MTXHeader header;

        // Parse header
        parseHeader(inf, header);

        outf << header.nrows << "\t" << header.ncols << "\t" << header.nnzero << std::endl;
    }
}

void mx_view(MX_opt &opt)
{

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

    for (int i = 0; i < opt.files.size(); i++)
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
            infstream.open(opt.files[i]);
            inbuf = infstream.rdbuf();
        }
        std::istream inf(inbuf);
        MTXHeader header;

        // Parse header
        parseHeader(inf, header);

        // header
        outf << header.format << '\n'
             << "%\n";
        outf << header.nrows << ' ' << header.ncols << ' ' << header.nnzero << '\n';

        // Parse the rest of the matrix
        std::string line;

        MTXRecord r;
        while (std::getline(inf, line))
        {
            std::stringstream ss(line);
            ss >> r.row >> r.col >> r.val;

            outf << r.row << ' ' << r.col << ' ' << r.val << '\n';
        }
    }
}