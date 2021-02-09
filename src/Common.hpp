#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <string>

struct MX_opt
{
    // view and shape
    std::vector<std::string> files;
    std::string output;

    // sum
    int axis;
};

#endif