#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <string>

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

    MX_opt() : axis(0){};
};

#endif