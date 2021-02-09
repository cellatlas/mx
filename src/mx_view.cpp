#include "mx_view.h"
#include "MTX.h"

#include <sstream>
#include <iostream>
#include <fstream>

void mx_shape(const MX_opt &opt)
{
    bool std_out = false;
    std::ofstream outf;
    // If there is no output file write to stdout
    if (opt.output.empty())
    {
        std_out = true;
    }
    else
    {
        outf.open(opt.output);
    }

    // loop through the input files
    for (int i = 0; i < opt.files.size(); i++)
    {
        std::ifstream inf(opt.files[i]);
        MTXHeader header;

        // Parse header
        parseHeader(inf, header);
        if (std_out)
        {
            std::cout << header.nrows << "\t" << header.ncols << "\t" << header.nnzero << std::endl;
        }
        else
        {
            outf << header.nrows << "\t" << header.ncols << "\t" << header.nnzero << std::endl;
        }
    }
    if (!std_out)
    {
        outf.close();
    }
}

void mx_view(const MX_opt &opt)
{

    std::ifstream inf(opt.files[0]);
    MTXHeader header;

    // Parse header
    parseHeader(inf, header);

    // Parse the rest of the matrix
    std::string line;

    MTXRecord r;
    while (std::getline(inf, line))
    {
        std::stringstream ss(line);
        ss >> r.row >> r.col >> r.val;

        std::cout << r.row << ' ' << r.col << ' ' << r.val << '\n';
    }
}