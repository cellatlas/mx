#include "Common.hpp" // MX_opt is here

// Program options
#include "mx_options.h"
#include "mx_validate.h"
#include "mx_display.h"

// commands
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

// Making a new sub-command:
/* 
Create new mx_subcmd.cpp and mx_subcmd.h file
in main.cpp add #include "mx_subcmd.h" at top
in mx_display.cpp and mx_display.h add displayProgramOptions_subcmd()
in mx_options.cpp and mx_options.h add parseProgramOptions_subcmd()
in mx_validate.cpp and mx_validate.h add validateProgramOptions_subcmd()
in main.cpp add an if statement for the subcmd
*/

int main(int argc, char *argv[])
{
    MX_opt opt;
    bool disp_help = argc == 2;
    if (argc == 1)
    {
        displayProgramOptions_MX();
        exit(0);
    }
    std::string cmd(argv[1]);
    // subcommand
    if (cmd == "view")
    {
        if (disp_help)
        {
            displayProgramOptions_view();
            exit(0);
        }
        // parse view
        parseProgramOptions_view(argc - 1, argv + 1, opt);
        if (validateProgramOptions_view(opt))
        {
            mx_view(opt);
        }
        else
        {
            displayProgramOptions_view();
            exit(1);
        }
    }
    else if (cmd == "shape")
    {
        if (disp_help)
        {
            displayProgramOptions_shape();
            exit(0);
        }
        parseProgramOptions_view(argc - 1, argv + 1, opt);

        if (validateProgramOptions_view(opt))
        {
            mx_shape(opt);
        }
        else
        {
            displayProgramOptions_shape();
            exit(1);
        }
    }
    else if (cmd == "sort")
    {
        if (disp_help)
        {
            displayProgramOptions_sort();
            exit(0);
        }
        parseProgramOptions_sort(argc - 1, argv + 1, opt);
        if (validateProgramOptions_sort(opt))
        {
            mx_sort(opt);
        }
        else
        {
            displayProgramOptions_sort();
            exit(1);
        }
    }
    else if (cmd == "sum")
    {
        if (disp_help)
        {
            displayProgramOptions_sum();
            exit(0);
        }
        parseProgramOptions_sum(argc - 1, argv + 1, opt);
        if (validateProgramOptions_sum(opt))
        {
            mx_sum(opt);
        }
        else
        {
            displayProgramOptions_sum();
            exit(1);
        }
    }
    else if (cmd == "extract")
    {
        if (disp_help)
        {
            displayProgramOptions_extract();
            exit(0);
        }
        parseProgramOptions_extract(argc - 1, argv + 1, opt);
        if (validateProgramOptions_extract(opt))
        {
            mx_extract(opt);
        }
        else
        {
            displayProgramOptions_extract();
            exit(1);
        }
    }
    else
    {
        std::cerr << "mx command not found: " << cmd << std::endl;
        displayProgramOptions_MX();
        exit(1);
    }

    return 0;
}