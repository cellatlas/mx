#include "MTX.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

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

template <typename Out>
void split(const std::string &s, char delim, Out result)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim))
    {
        *result++ = item;
    }
}

void parseNewHeader(std::istream &inf, MTXHeader &header)
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
            char delim = ' ';
            split(line, delim, std::back_inserter(header.axis_data));

            std::cout << "new header struct: " << std::endl
                      << "header shape: " << header.axis_data.size() << std::endl
                      << "header vals: " << header.axis_data[0] << '\t' << header.axis_data[1] << '\t' << header.axis_data[2] << std::endl;

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

void writeRecord(std::ostream &outf, MTXRecord &r)
{
    const char delim = ' ';
    outf << r.row << delim << r.col << delim << r.val << '\n';
}

void parseRecord(std::string &line, MTXRecord &r)
{
    std::stringstream ss(line);
    ss >> r.row >> r.col >> r.val;
}