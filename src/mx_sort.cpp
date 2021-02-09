#include "Common.hpp"
#include "MTX.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool cmp_row(MTXRecord &a, MTXRecord &b)
{
    return (a.row < b.row);
}

bool cmp_col(MTXRecord &a, MTXRecord &b)
{
    return (a.col < b.col);
}

void mx_sort(MX_opt &opt)
{
    std::function<bool(MTXRecord, MTXRecord)> cmp[] = {&cmp_row, &cmp_col};
    int axis = opt.axis;

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

    // write the new matrix
    bool std_out = false;
    std::ofstream outf;
    // If there is no output file write to stdout
    if (opt.output.empty())
    {
        std_out = true;
        std::cout << header.format << '\n'
                  << "%\n";
        std::cout << header.nrows << ' ' << header.ncols << ' ' << header.nnzero << '\n';
    }
    else
    {
        outf.open(opt.output);
        outf << header.format << '\n'
             << "%\n";
        outf << header.nrows << ' ' << header.ncols << ' ' << header.nnzero << '\n';
    }

    for (int i = 0; i < vec.size(); i++)
    {
        if (std_out)
        {
            std::cout << vec[i].row << ' ' << vec[i].col << ' ' << vec[i].val << '\n';
        }
        else
        {
            outf << vec[i].row << ' ' << vec[i].col << ' ' << vec[i].val << '\n';
        }
    }
}