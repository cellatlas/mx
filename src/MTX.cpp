#include "MTX.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

void parseHeader(std::istream &inf, MTXHeader &header)
{
    std::string line;
    int nl = 0;
    while (std::getline(inf, line))
    {
        if (nl == 0)
        {
            header.format = line;
            nl++;
        }
        if (line[0] != '%')
        {
            std::stringstream ss(line);
            ss >> header.nrows >> header.ncols >> header.nnzero;

            break;
        }
    }
}

void writeHeader(std::ostream &outf, MTXHeader &header)
{
    outf << header.format << '\n'
         << "%\n";
    outf << header.nrows << ' ' << header.ncols << ' ' << header.nnzero << '\n';
}