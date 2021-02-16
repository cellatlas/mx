#ifndef MTX_H

#include <fstream>

// file format: https://math.nist.gov/MatrixMarket/formats.html

struct MTXHeader
{
    int nrows;
    int ncols;
    int nnzero;
    std::string format;
    MTXHeader() : nrows(0), ncols(0), nnzero(0), format("%%MatrixMarket matrix coordinate real general"){};
};

struct MTXRecord
{
    int row;
    int col;
    float val;
};

void parseHeader(std::istream &inf, MTXHeader &header);
void writeHeader(std::ostream &outf, MTXHeader &header);

#endif