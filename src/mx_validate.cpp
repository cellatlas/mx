#include "Common.hpp"
#include <string.h>
#include <sstream>
#include <utility> // std::pair, std::make_pair
#include <iostream>

bool validateProgramOptions_view(MX_opt &opt)
{
    bool ret = true;
    return ret;
}
bool validateProgramOptions_sum(MX_opt &opt)
{
    bool ret = true;
    return ret;
}
bool validateProgramOptions_sort(MX_opt &opt)
{
    bool ret = true;
    return ret;
}

bool validateProgramOptions_extract(MX_opt &opt)
{
    bool ret = true;

    if (opt.index.empty())
    {
        ret = false;
        std::cerr << "[error] no index specified" << std::endl;
    }
    else
    {
        char delimiter = '-';
        size_t pos = opt.index.find(delimiter);
        int lower, upper;

        // if delimiter not found
        if (pos == std::string::npos)
        {
            lower = atoi(opt.index.c_str());
            upper = lower + 1;
        }
        else // else split at delimiter and set bounds
        {
            if (pos == 0) // - at beginning of string ie "-1432"
            {
                lower = 0;
                upper = atoi(opt.index.substr(pos + 1, opt.index.size()).c_str());
            }
            else if (pos == opt.index.size() - 1) // the - is at end of string ie "12-"
            {
                lower = atoi(opt.index.substr(0, pos).c_str());
                upper = -1;
            }
            else // the - is in between two numbers 43-100
            {
                lower = atoi(opt.index.substr(0, pos).c_str());
                upper = atoi(opt.index.substr(pos + 1, opt.index.size()).c_str());
            }
        }

        if (upper == -1 || lower < upper)
        {
            opt.range = std::make_pair(lower, upper);
            // std::cout << lower << ", " << upper << '\n';
        }
        else
        {
            std::cerr << "[error] lower must be strictly smaller than upper" << std::endl;
            ret = false;
        }
    }
    return ret;
}