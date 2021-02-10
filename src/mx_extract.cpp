#include "Common.hpp"
#include "MTX.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

void mx_extract(MX_opt &opt)
{
    std::ifstream inf(opt.files[0]);
    int axis = opt.axis;

    MTXHeader header;
    MTXRecord r;

    parseHeader(inf, header);

    int lower = opt.range.first;
    int upper = opt.range.second;
    if (upper == -1)
    {
        if (axis == 0)
        {
            upper = header.nrows;
        }
        else if (axis == 1)
        {
            upper = header.ncols;
        }
    }
    // Setup file direction
    std::streambuf *buf = nullptr;
    std::ofstream of;
    if (opt.output.empty())
    {
        buf = std::cout.rdbuf();
    }
    else
    {
        of.open(opt.output);
        buf = of.rdbuf();
    }
    std::ostream outf(buf);

    // write header
    outf << header.format << '\n'
         << "%\n";
    outf << header.nrows << ' ' << header.ncols << ' ' << header.nnzero << '\n';

    std::string line;
    std::vector<int> idx;
    idx.push_back(0);
    idx.push_back(0);
    while (std::getline(inf, line))
    {
        std::stringstream ss(line);
        ss >> r.row >> r.col >> r.val;
        idx[0] = r.row;
        idx[1] = r.col;
        // subtract 1 since MTX is 1-indexed but I use 0-index for lower-upper range
        if (idx[axis] - 1 >= lower && idx[axis] - 1 < upper)
        {
            outf << r.row << ' ' << r.col << ' ' << r.val << '\n';
        }
    }
}