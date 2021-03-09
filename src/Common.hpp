#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <string>

#define MX_VERSION "0.0.1"

struct MX_opt
{
    //
    bool stream_in, stream_out;
    // view and shape
    std::vector<std::string> files;
    std::string output;

    // sum
    int axis;

    // extract
    std::string index;
    std::pair<int, int> range;
    std::vector<int> elements;

    // sample
    int k_samples;

    MX_opt() : axis(0), k_samples(1){};
};

#endif