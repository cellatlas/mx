#include "Common.hpp" // MX_opt is here

// Program options
#include "mx_options.h"
#include "mx_validate.h"

#include "mx_view.h"    // mx_view
#include "mx_sum.h"     // mx_sum
#include "mx_sort.h"    // mx_sort
#include "mx_extract.h" // mx_extract

#include <stdlib.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

int main(int argc, char *argv[])
{
    MX_opt opt;
    if (argc == 1)
    {
        exit(0);
    }
    std::string cmd(argv[1]);
    // subcommand
    if (cmd == "view")
    {
        // parse view
        parseProgramOptions_view(argc - 1, argv + 1, opt);
        mx_view(opt);
    }
    else if (cmd == "shape")
    {
        parseProgramOptions_view(argc - 1, argv + 1, opt);
        mx_shape(opt);
    }
    else if (cmd == "sort")
    {
        parseProgramOptions_sort(argc - 1, argv + 1, opt);
        mx_sort(opt);
    }
    else if (cmd == "sum")
    {
        parseProgramOptions_sum(argc - 1, argv + 1, opt);
        mx_sum(opt);
    }
    else if (cmd == "extract")
    {
        parseProgramOptions_extract(argc - 1, argv + 1, opt);
        if (validateProgramOptions_extract(opt))
        {
            mx_extract(opt);
        }
        else
        {
            exit(1);
        }
    }
    else
    {
        std::cerr << "mx command not found: " << cmd << std::endl;
        exit(1);
    }

    return 0;
}