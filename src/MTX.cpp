#include "MTX.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>     // pow
#include <algorithm> // min

size_t PAD = std::pow(2, 4);

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

template <typename integer>
void stringify(integer &val, std::string &sp)
{
    std::string m, s;
    m = "";
    s = std::to_string(val);
    size_t slen = s.length();

    // if the number is negative, increase len of string rep of number
    if (val < 0)
    {
        m = "-";
        s = std::to_string(std::abs((int32_t)val));
    };
    sp = m + std::string(PAD - std::min(PAD, slen), '0') + s;
}

// header

void readMTXHeader(std::istream &inf, MTXHeader &header)
{
    std::string line, magic, obj, form, fd, sym;
    int nl = 0;
    while (std::getline(inf, line))
    {
        if (nl == 0)
        {
            // TODO parse format_str into numbers associated with mtx header options
            header.format_str = line;
            std::istringstream iss(line);
            std::string item;

            // this splits the header into its consituent parts
            iss >> magic >> obj >> form >> fd >> sym;

            header.object = obj;
            header.format = fd;
            header.field = fd;
            header.symmetry = sym;

            nl++;
        }
        if (line[0] != '%' && nl > 0)
        {

            std::istringstream iss(line);
            std::string item;
            int ndim = 0;
            while (std::getline(iss, item, header.delim))
            {
                header.idx_data[ndim] = stoi(item);
                ndim++;
            }

            header.ndim = ndim - 1; // discounting the nnzero
            break;
        }
    }
}
void parseMTXHeader(std::istream &inf, MTXHeader &header)
{
    std::string line;
    int nl = 0;
    while (std::getline(inf, line))
    {
        if (nl == 0)
        {
            header.format_str = line;
            nl++;
        }
        if (line[0] != '%' && nl > 0)
        {

            std::istringstream iss(line);
            std::string item;
            int ndim = 0;
            while (std::getline(iss, item, header.delim))
            {
                header.idx_data[ndim] = stoi(item);
                ndim++;
            }

            header.ndim = ndim - 1; // discounting the nnzero
            break;
        }
    }
}

// Header

void writeMTXHeader(std::ostream &outf, MTXHeader &h)
{
    // in order to make the header editable (after down sampling or selecting)
    // we need to make the idx_data fields constant width.
    // write format
    // std::cout << "writing mtx header" << std::endl;
    // std::cout << h.object << h.delim
    //           << h.format << h.delim
    //           << h.field << h.delim
    //           << h.symmetry << "\n";
    // outf.write("hello", sizeof("hello"));
    outf
        << h.magic
        << h.object.c_str() << h.delim
        << h.format.c_str() << h.delim
        << h.field.c_str() << h.delim
        << h.symmetry.c_str() << "\n"
        << "%\n";

    // write index data
    // if the header is editable then we put the largest int so that we can go back and edit it
    std::string s, sp;
    for (int i = 0; i < h.ndim; i++)
    {
        // stringify(h.idx_data[i], sp);
        // outf << sp << h.delim;
        outf << h.idx_data[i] << h.delim;
    }

    // write value data (last element in idx_data)
    // stringify(h.idx_data[h.ndim], sp);
    // outf << sp << '\n';
    outf << h.idx_data[h.ndim] << '\n';
}

// Record

bool readMTXRecord(std::istream &inf, MTXRecord &r, MTXHeader &h)
{
    std::string line;
    std::getline(inf, line);
    std::istringstream iss(line);
    std::string item;
    if (line == "")
    {
        return false;
    }
    for (int i = 0; i < h.ndim; i++)
    {
        std::getline(iss, item, h.delim);
        r.idx[i] = stoi(item);
    }
    // last element is value
    std::getline(iss, item, h.delim);
    r.value = stof(item);
    return true;
}
void parseMTXRecord(std::string &line, MTXRecord &r, MTXHeader &h)
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

void writeMTXRecord(std::ostream &outf, MTXRecord &r, MTXHeader &h)
{
    std::string s, sp, m;
    for (int i = 0; i < h.ndim; i++)
    {
        // stringify(r.idx[i], sp);
        // outf << sp << h.delim;
        outf << r.idx[i] << h.delim;
    }

    // stringify(r.value, sp);
    // outf << sp << '\n';
    outf << r.value << '\n';
}

// extract list

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

// track index

void trackIndex(MTXRecord &r, MTXHeader &h, int &nw, int &axis)
{
    for (int i = 0; i < h.ndim; i++)
    {
        if (r.idx[i] > h.idx_data[i])
        {
            // update the tracker record
            h.idx_data[i] = r.idx[i];
        }
    }
    // increase number of nonzero elements
    h.idx_data[h.ndim]++;

    r.idx[axis] = nw;
}
