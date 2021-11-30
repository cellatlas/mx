#ifndef COMMON_HPP
#define COMMON_HPP

#include "MTX.h"
#include "MX.h"

#include <vector>
#include <string>

#define MX_VERSION "0.0.1"

struct MX_opt
{
    //
    bool stream_in, stream_out = false;

    // view and shape
    std::vector<std::string> files;
    std::string output;

    // sum
    int axis = 0;

    // multiply
    float value;
    std::string file;
    bool inverse;

    // extract
    std::string index;
    std::pair<int, int> range;
    std::vector<int> elements;

    // sample
    int k_samples;

    MX_opt() : axis(0), value(1.0), k_samples(1){};
};

void file_direction(std::string &ifn, std::string &ofn, bool &stream_in, bool &stream_out, std::streambuf *&ibuf, std::streambuf *&obuf, std::ifstream &ifs, std::ofstream &ofs);
std::string str_tolower(std::string s);

void stringify(const std::string &s, std::string &sp);
void MTX_to_MX_Header(MTXHeader &mtx_h, MXHeader &mx_h);
void MTX_to_MX_Record(MTXRecord &mtx_r, MXRecord &mx_r);

void MX_to_MTX_Header(MXHeader &mx_h, MTXHeader &mtx_h);
void MX_to_MTX_Record(MXRecord &mx_r, MTXRecord &mtx_r);
void buildMTXHeaderOptions(std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> &hmaps);
void buildMTXHeaderOptions_inv(std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>> &hmaps_inv);
#endif