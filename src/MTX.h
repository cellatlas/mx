#ifndef MTX_H
#define MTX_H

#include <fstream>
#include <vector>
#include <unordered_map>

// file format: https://math.nist.gov/MatrixMarket/formats.html

// new format
struct MTXHeader
{
    uint32_t ndim;        // number of dimensions (ie how many idx)
    uint32_t idx_data[2 + 1]; // [[ nidx0, nidx1, nidx2... ] <-- len = ndim, nnzero]
    char delim;
    std::string magic = "%%MatrixMarket ";
    std::string format_str;
    std::string object = "matrix";
    std::string format = "coordinate";
    std::string field = "integer";
    std::string symmetry = "general";
    MTXHeader() : ndim(2), delim(' '), format_str("%%MatrixMarket matrix coordinate real general"){};
};

struct MTXRecord
{
    int32_t value;
    uint32_t idx[2]; // same size as ndim
};

void trackIndex(MTXRecord &record, MTXHeader &header, int &numWritten, int &axis);

void writeMTXHeader(std::ostream &outf, MTXHeader &header);
void readMTXHeader(std::istream &inf, MTXHeader &header);
void parseMTXHeader(std::istream &inf, MTXHeader &header);

bool readMTXRecord(std::istream &inf, MTXRecord &record, MTXHeader &header);
void writeMTXRecord(std::ostream &outf, MTXRecord &record, MTXHeader &header);
void parseMTXRecord(std::string &line, MTXRecord &record, MTXHeader &header);

void printHeader(MTXHeader &header);

bool parseExtractList(const std::string &filename, std::unordered_map<int32_t, int32_t> &extractList);
void printExtractList(std::unordered_map<int32_t, int32_t> &extractList);

#endif