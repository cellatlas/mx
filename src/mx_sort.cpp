#include "Common.hpp"
#include "MTX.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional> // std::function
#include <algorithm>  // std::sort
#include <list>

bool cmp_row(const MTXRecord &a, const MTXRecord &b)
{
    return (a.row < b.row);
}

bool cmp_col(const MTXRecord &a, const MTXRecord &b)
{
    return (a.col < b.col);
}

bool cmp_val(const MTXRecord &a, const MTXRecord &b)
{
    return (a.val < b.val);
}

void mx_sort(MX_opt &opt)
{
    int naxis = 3;

    std::function<bool(const MTXRecord &, const MTXRecord &)> cmp[] = {&cmp_row, &cmp_col, &cmp_val};
    int axis = opt.axis;
    // TODO: get length of cmp
    if (axis == -1)
    {
        // sort by value
        axis = naxis - 1;
    }

    std::ifstream inf(opt.files[0]);
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

    // write the new matrix
    // header
    outf << header.format << '\n'
         << "%\n";
    outf << header.nrows << ' ' << header.ncols << ' ' << header.nnzero << '\n';
    // entries
    for (int i = 0; i < vec.size(); i++)
    {

        outf << vec[i].row << ' ' << vec[i].col << ' ' << vec[i].val << '\n';
    }
}