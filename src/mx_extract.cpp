#include "Common.hpp"
#include "MTX.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>

// TODO update the header with new nrows/ncols/nzeros
void mx_extract(MX_opt &opt)
{
    // must be sorted by axis
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
    std::unordered_set<int> cols;

    // get the first line
    std::getline(inf, line);
    std::stringstream ss(line);
    ss >> r.row >> r.col >> r.val;

    std::vector<int> idx, prev_idx;
    idx.push_back(r.row);
    idx.push_back(r.col);
    prev_idx = idx;
    cols.insert(r.col);

    std::vector<int> vsize;
    vsize.push_back(0);
    vsize.push_back(0);
    vsize.push_back(0);

    // Get first line
    // subtract 1 since MTX is 1-indexed but I use 0-index for lower-upper range
    if (idx[axis] - 1 >= lower && idx[axis] - 1 < upper)
    {
        vsize[axis]++;
        outf << r.row << ' ' << r.col << ' ' << r.val << '\n';

        // add to col (row)
        cols.insert(r.col);
        vsize[(axis + 1) % 2]++;
        vsize[vsize.size() - 1]++;
    }

    // get the rest of the lines
    while (std::getline(inf, line))
    {
        std::stringstream ss(line);
        ss >> r.row >> r.col >> r.val;
        idx[0] = r.row;
        idx[1] = r.col;

        // subtract 1 since MTX is 1-indexed but I use 0-index for lower-upper range
        if (idx[axis] - 1 >= lower && idx[axis] - 1 < upper)
        {

            if (idx[axis] != prev_idx[axis])
            {
                vsize[axis]++;
            }

            // check if we've seen this col before
            auto it = cols.find(r.col);
            if (it == cols.end())
            {
                cols.insert(r.col);
                vsize[((1 + axis) % 2)]++;
            }
            prev_idx = idx;
            outf << r.row << ' ' << r.col << ' ' << r.val << '\n';
            vsize[vsize.size() - 1]++;
        }
    }
    // get straggler
    // std::cout << vsize[axis] << ", " << vsize[((1 + axis) % 2)] << ", " << vsize[vsize.size() - 1] << std::endl;

    // fix the header
}