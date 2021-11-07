#include "MTX.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

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

void parseNewHeader(std::istream &inf, newMTXHeader &header)
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
            header.delim = delim;

            std::istringstream iss(line);
            std::string item;
            int ndim = 0;
            while (std::getline(iss, item, delim))
            {
                header.idx_data[ndim] = stoi(item);
                ndim++;
            }

            header.ndim = ndim - 1; // discounting the nnzero
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

void parseNewRecord(std::string &line, newMTXRecord &r, newMTXHeader &h)
{
    std::istringstream iss(line);
    std::string item;
    for (int i = 0; i < h.ndim; i++)
    {
        std::getline(iss, item, h.delim);
        r.idx[i] = stoi(item);
    }
    // last element is value
    std::getline(iss, item, h.delim);
    r.value = stof(item);
}

void writeNewRecord(std::ostream &outf, newMTXRecord &r, newMTXHeader &h)
{
    for (int i = 0; i < h.ndim; i++)
    {
        outf << r.idx[i] << h.delim;
    }
    outf << r.value << '\n';
}

void writeNewHeader(std::ostream &outf, newMTXHeader &h)
{
    // write format
    outf << h.format << '\n'
         << "%\n";

    // write index data
    for (int i = 0; i < h.ndim; i++)
    {
        outf << h.idx_data[i] << h.delim;
    }

    // write value data (last element in idx_data)
    outf << h.idx_data[h.ndim] << '\n';
}

bool parseExtractList(const std::string &filename, std::unordered_map<int32_t, int32_t> &m)
{
    // extract list is a {value, index} map
    // values are derived from the select file, index is the 0-index position
    std::ifstream inf(filename.c_str());
    int32_t ele;

    int i = 0;
    while (inf >> ele)
    {
        m.insert({ele, i});
        i++;
    }
    return true;
}
void printExtractList(std::unordered_map<int32_t, int32_t> &m)
{
    for (auto &i : m)
    {
        std::cout << i.first << ", " << i.second << std::endl;
    }
}

void printRecord(MTXRecord &r)
{
    std::cout << r.row << "\t" << r.col << "\t" << r.val << std::endl;
}