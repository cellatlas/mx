#ifndef MX_H
#define MX_H

#include <fstream>

struct MXHeader
{
    uint32_t ndim = 2;                    // number of dimensions (ie how many idx)
    uint32_t idx_data[2 + 1] = {0, 0, 0}; // [[ nidx0, nidx1, nidx2... ] <-- len = ndim, nnzero]
    uint32_t object = 0;
    uint32_t format = 0;
    uint32_t field = 0;
    uint32_t symmetry = 0;
};

struct MXRecord
{
    int32_t value;
    uint32_t idx[2] = {0, 0};
};

void writeMXHeader(std::ostream &outf, MXHeader &header);
void readMXHeader(std::istream &inf, MXHeader &header);
void writeMXRecord(std::ostream &outf, MXRecord &record, MXHeader &header);
bool readMXRecord(std::istream &inf, MXRecord &record, MXHeader &header);
bool readMXRecord(std::istream &inf, MXRecord &record, MXHeader &header);
bool readManyMXRecords(std::istream &inf, MXRecord *&records, MXHeader &header, size_t &N);
void printMXRecord(std::ostream &outf, MXRecord &record, MXHeader &header);

#endif