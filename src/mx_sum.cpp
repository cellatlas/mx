#include "Common.hpp"
#include "MTX.h"

#include <fstream>  // std::ifstream
#include <string>   // string
#include <sstream>  // stringstream
#include <iostream> // std::cout
#include <vector>

void mx_sum(MX_opt &opt)
{
    std::ifstream inf(opt.files[0]);
    MTXHeader header;
    parseHeader(inf, header);

    std::string line;
    MTXRecord r;

    int axis = opt.axis;

    // get first line
    std::getline(inf, line);
    std::stringstream ss(line);
    ss >> r.row >> r.col >> r.val;

    std::vector<int> idx;
    idx.push_back(r.row);
    idx.push_back(r.col);
    std::vector<int> prev_idx = idx;

    float s = r.val;

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

    // loop through file
    while (std::getline(inf, line))
    {
        std::stringstream ss(line);
        ss >> r.row >> r.col >> r.val;

        idx[0] = r.row;
        idx[1] = r.col;

        if (idx[axis] != prev_idx[axis])
        {
            outf << s << '\n';
            // outf.write((char *)&s, sizeof(s));
            s = 0;
        }
        s += r.val;

        prev_idx = idx;
    }
    // write out the straggler
    outf << s << '\n';
    // outf.write((char *)&s, sizeof(s));
}