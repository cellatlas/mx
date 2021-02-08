#include "MTX.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

void parseHeader(std::ifstream &inf, MTXHeader &header)
{
    std::string line;
    while (std::getline(inf, line))
    {
        if (line[0] != '%')
        {
            std::stringstream ss(line);
            ss >> header.nrows >> header.ncols >> header.nnzero;

            break;
        }
    }
}