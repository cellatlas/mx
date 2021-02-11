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

// TODO: fix input subcommands taking in multiple files
// vs just stdin, ie clean up code

// Setting up dictionary for easy command execution
typedef struct
{
    std::string subcmd;
    void (*display)();                      // now in our FunctionMap type, we can reference a function by its name
    void (*parse)(int, char *[], MX_opt &); // now in our FunctionMap type, we can reference a function by its name
    bool (*validate)(MX_opt &);             // now in our FunctionMap type, we can reference a function by its name
    void (*command)(MX_opt &);              // now in our FunctionMap type, we can reference a function by its name
} FunctionMap;

const int function_count = 5;
const FunctionMap functions[function_count]{
    {"view", displayProgramOptions_view, parseProgramOptions_view, validateProgramOptions_view, mx_view},
    {"shape", displayProgramOptions_shape, parseProgramOptions_view, validateProgramOptions_view, mx_shape},
    {"sort", displayProgramOptions_sort, parseProgramOptions_sort, validateProgramOptions_sort, mx_sort},
    {"sum", displayProgramOptions_sum, parseProgramOptions_sum, validateProgramOptions_sum, mx_sum},
    {"extract", displayProgramOptions_extract, parseProgramOptions_extract, validateProgramOptions_extract, mx_extract},
};

int main(int argc, char *argv[])
{
    MX_opt opt;
    bool disp_help = argc == 2;
    if (argc == 1)
    {
        displayProgramOptions_MX();
        exit(0);
    }

    // subcommand
    std::string cmd(argv[1]);
    bool command_found = false;
    for (int nc = 0; nc < function_count; nc++)
    {
        if (functions[nc].subcmd == cmd)
        {
            command_found = true;
            if (disp_help)
            {
                functions[nc].display();
                exit(0);
            }
            functions[nc].parse(argc - 1, argv + 1, opt);
            if (functions[nc].validate(opt))
            {
                functions[nc].command(opt);
                exit(0);
            }
            else
            {
                functions[nc].display();
                exit(1);
            }
            break;
        }
    }
    if (!command_found)
    {
        std::cerr << "mx command not found: " << cmd << std::endl;
        displayProgramOptions_MX();
        exit(1);
    }

    return 0;
}