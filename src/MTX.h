#ifndef MTX_H

#include <fstream>
#include <vector>
#include <unordered_map>

// file format: https://math.nist.gov/MatrixMarket/formats.html

struct MTXHeader
{
    int nrows;
    int ncols;
    int nnzero;
    int naxis;
    char delim;
    std::string format;
    std::vector<std::string> axis_data; // number of elements in each axis
    MTXHeader() : delim(' '), nrows(0), ncols(0), nnzero(0), naxis(0), format("%%MatrixMarket matrix coordinate real general"){};
};

struct MTXRecord
{
    int row;
    int col;
    std::vector<int> idx;
    float val;
};

// new format
struct newMTXHeader
{
    int ndim;        // number of dimensions (ie how many idx)
    int idx_data[4]; // [[ nidx0, nidx1, nidx2... ] <-- len = ndim, nnzero]
    char delim;
    std::string format;
    newMTXHeader() : ndim(2), delim(' '), format("%%MatrixMarket matrix coordinate real general"){};
};

struct newMTXRecord
{
    float value;
    int idx[4];
};

void parseHeader(std::istream &inf, MTXHeader &header);
void writeHeader(std::ostream &outf, MTXHeader &header);
void writeRecord(std::ostream &outf, MTXRecord &record);
void parseRecord(std::string &line, MTXRecord &record);
void printRecord(MTXRecord &record);

void writeNewRecord(std::ostream &outf, newMTXRecord &record, newMTXHeader &header);
void writeNewHeader(std::ostream &outf, newMTXHeader &header);
void parseNewRecord(std::string &line, newMTXRecord &record, newMTXHeader &header);
void parseNewHeader(std::istream &inf, newMTXHeader &header);

bool parseExtractList(const std::string &filename, std::unordered_map<int32_t, int32_t> &extractList);
void printExtractList(std::unordered_map<int32_t, int32_t> &extractList);

#endif