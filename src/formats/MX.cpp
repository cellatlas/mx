#include "MX.h"

#include <iostream>

void readMXHeader(std::istream &inf, MXHeader &header)
{
    inf.read((char *)&header, sizeof(header));
}

void writeMXHeader(std::ostream &outf, MXHeader &header)
{
    outf.write((char *)&header, sizeof(header));
}

bool readMXRecord(std::istream &inf, MXRecord &record, MXHeader &header)
{
    inf.read((char *)&record, sizeof(record));
    return true;
}

bool readManyMXRecords(std::istream &inf, MXRecord *&records, MXHeader &header, size_t &N)
{
    inf.read((char *)records, N * sizeof(MXRecord));
    return true;
}

void writeMXRecord(std::ostream &outf, MXRecord &record, MXHeader &header)
{
    outf.write((char *)&record, sizeof(record));
}

void printMXRecord(std::ostream &outf, MXRecord &record, MXHeader &header)
{
    for (int i = 0; i < header.ndim; i++)
    {
        outf << record.idx[i] << " ";
    }
    outf << record.value << "\n";
}