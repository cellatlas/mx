#ifndef MTX_H

#include <fstream>
#include <vector>

// file format: https://math.nist.gov/MatrixMarket/formats.html

struct MTXHeader
{
    int nrows;
    int ncols;
    int nnzero;
    int naxis;
    std::string format;
    std::vector<std::string> axis_data; // number of elements in each axis
    MTXHeader() : nrows(0), ncols(0), nnzero(0), naxis(0), format("%%MatrixMarket matrix coordinate real general"){};
};

struct MTXRecord
{
    int row;
    int col;
    std::vector<int> idx;
    float val;
};

void parseHeader(std::istream &inf, MTXHeader &header);
void parseNewHeader(std::istream &inf, MTXHeader &header);
void writeHeader(std::ostream &outf, MTXHeader &header);
void writeRecord(std::ostream &outf, MTXRecord &record);
void parseRecord(std::string &line, MTXRecord &record);

#endif