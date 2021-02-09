#ifndef MTX_H

#include <fstream>

struct MTXHeader
{
    int nrows;
    int ncols;
    int nnzero;
    std::string format;
};

struct MTXRecord
{
    int row;
    int col;
    float val;
};

void parseHeader(std::ifstream &inf, MTXHeader &header);

#endif