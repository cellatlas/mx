#include "MX.h"

#include <cmath>
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

void sumMXRecords(int axis, int n_val, MXRecord *p, double **data, bool log)
{
    int idx_diff = 0;
    int pidx = 0;
    size_t j;
    for (size_t i = 0; i < n_val;)
    {
        data[pidx][0] += p[i].value;
        j = i + 1;
        for (; j < n_val; j++)
        {
            idx_diff = p[j].idx[axis] - p[i].idx[axis];
            if (idx_diff > 0)
            {
                // make the value log(x + 1)

                data[pidx][0] = log ? std::log1p(data[pidx][0]) : data[pidx][0];
                pidx += idx_diff;
                break;
            }
            data[pidx][0] += p[j].value;
        }
        // increase the index
        i = j;
    }
    // get the straggler
    data[pidx][0] = log ? std::log1p(data[pidx][0]) : data[pidx][0];
}