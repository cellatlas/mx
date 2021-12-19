#include "formats/MTX.h"
#include "formats/MX.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>

void file_direction(std::string &ifn, std::string &ofn, bool &stream_in, bool &stream_out, std::streambuf *&ibuf, std::streambuf *&obuf, std::ifstream &ifs, std::ofstream &ofs)
{
    // IN
    if (stream_in)
    {
        ibuf = std::cin.rdbuf();
    }
    else
    {
        ifs.open(ifn, std::ios::in);
        ibuf = ifs.rdbuf();
    }

    // OUT
    if (stream_out)
    {
        obuf = std::cout.rdbuf();
    }
    else
    {
        ofs.open(ofn);
        obuf = ofs.rdbuf();
    }
    return;
}

std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); } // correct
    );
    return s;
}

size_t PAD = std::pow(2, 4);
void stringify(const std::string &s, std::string &sp)
{
    std::string m;
    size_t slen = s.length();

    sp = s + std::string(PAD - std::min(PAD, slen), ' ');
}

void buildMTXHeaderOptions(std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> &hmaps)
{
    std::unordered_map<std::string, uint32_t> objects;
    objects["matrix"] = 0;
    objects["vector"] = 1;
    objects["directed graph"] = 2;

    hmaps["objects"] = objects;

    std::unordered_map<std::string, uint32_t> formats;
    formats["coordinate"] = 0;
    formats["array"] = 1;

    hmaps["formats"] = formats;

    std::unordered_map<std::string, uint32_t> fields;
    fields["real"] = 0;
    fields["complex"] = 1;
    fields["integer"] = 2;
    fields["pattern"] = 3;

    hmaps["fields"] = fields;

    std::unordered_map<std::string, uint32_t> symmetries;
    symmetries["general"] = 0;
    symmetries["symmetric"] = 1;
    symmetries["hermitian"] = 2;

    hmaps["symmetries"] = symmetries;
}
void buildMTXHeaderOptions_inv(std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>> &hmaps_inv)
{
    std::unordered_map<uint32_t, std::string> objects_inv;
    objects_inv[0] = "matrix";
    objects_inv[1] = "vector";
    objects_inv[2] = "directed graph";

    std::unordered_map<uint32_t, std::string> formats_inv;
    formats_inv[0] = "coordinate";
    formats_inv[1] = "array";

    std::unordered_map<uint32_t, std::string> fields_inv;
    fields_inv[0] = "real";
    fields_inv[1] = "complex";
    fields_inv[2] = "integer";
    fields_inv[3] = "pattern";

    std::unordered_map<uint32_t, std::string> symmetries_inv;
    symmetries_inv[0] = "general";
    symmetries_inv[1] = "symmetric";
    symmetries_inv[2] = "hermitian";

    hmaps_inv["objects"] = objects_inv;
    hmaps_inv["formats"] = formats_inv;
    hmaps_inv["fields"] = fields_inv;
    hmaps_inv["symmetries"] = symmetries_inv;
}

void MTX_to_MX_Header(MTXHeader &mtx_h, MXHeader &mx_h)
{
    std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> hmaps;
    buildMTXHeaderOptions(hmaps);

    mx_h.ndim = mtx_h.ndim;
    std::copy(std::begin(mtx_h.idx_data), std::end(mtx_h.idx_data), std::begin(mx_h.idx_data));
    mx_h.object = hmaps["objects"][mtx_h.object];
    mx_h.format = hmaps["formats"][mtx_h.format];
    mx_h.field = hmaps["fields"][mtx_h.field];
    mx_h.symmetry = hmaps["symmetries"][mtx_h.symmetry];

    // std::cout << "MX header (from mtx)" << std::endl;
    // std::cout << mx_h.object << mtx_h.delim
    //           << mx_h.format << mtx_h.delim
    //           << mx_h.field << mtx_h.delim
    //           << mx_h.symmetry << '\n';
}

void MTX_to_MX_Record(MTXRecord &mtx_r, MXRecord &mx_r)
{
    mx_r.value = mtx_r.value;
    std::copy(std::begin(mtx_r.idx), std::end(mtx_r.idx), std::begin(mx_r.idx));
}

void MX_to_MTX_Header(MXHeader &mx_h, MTXHeader &mtx_h)
{
    std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>> hmaps_inv;
    buildMTXHeaderOptions_inv(hmaps_inv);

    mtx_h.ndim = mx_h.ndim;
    std::copy(std::begin(mx_h.idx_data), std::end(mx_h.idx_data), std::begin(mtx_h.idx_data));
    mtx_h.object = hmaps_inv["objects"][mx_h.object];
    mtx_h.format = hmaps_inv["formats"][mx_h.format];
    mtx_h.field = hmaps_inv["fields"][mx_h.field];
    mtx_h.symmetry = hmaps_inv["symmetries"][mx_h.symmetry];
}

void MX_to_MTX_Record(MXRecord &mx_r, MTXRecord &mtx_r)
{
    mtx_r.value = mx_r.value;
    std::copy(std::begin(mx_r.idx), std::end(mx_r.idx), std::begin(mtx_r.idx));
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

void printMatrix(int &n_pts, int &n_dim, double **&data)
{
    for (int i = 0; i < n_pts; i++)
    {
        for (int j = 0; j < n_dim; j++)
        {
            std::cout << data[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// void writeFilterMeans(std::ostream &outf, )