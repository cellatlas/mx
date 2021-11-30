#include "Common.hpp" // MX_opt is here

// commands
#include "mx_view.h"     // mx_view
#include "mx_sum.h"      // mx_sum
#include "mx_sort.h"     // mx_sort
#include "mx_extract.h"  // mx_extract
#include "mx_sample.h"   // mx_sample
#include "mx_multiply.h" // mx_multiply
#include "mx_text.h"     // mx_text

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
in mx_subcmd.cpp and mx_subcmd.h add
    - void displayProgramOptions_subcmd();
    - void parseProgramOptions_subcmd(int argc, char *argv[], MX_opt &opt);
    - bool validateProgramOptions_subcmd(MX_opt &opt);
    - void mx_subcmd(MX_opt &opt);

Extra options for MX_opt can be added in Common.hpp

in main.cpp add #include "mx_subcmd.h" at top
in main.cpp add subcmd to functions list
    and add 1 to function_count
*/

void displayProgramOptions_MX()
{
    std::cout << "mx " << MX_VERSION << std::endl
              << std::endl
              << "Usage: mx <cmd> [arguments] ..." << std::endl
              << "where <cmd> can be one of: " << std::endl
              << std::endl
              << "view              View matrix" << std::endl
              << "shape             Print matrix shape" << std::endl
              << "sort              Sort matrix" << std::endl
              << "sum               Sum elements" << std::endl
              << "multiply          Multiply elements" << std::endl
              << "extract           Extract elements" << std::endl
              << "sample            Sample elements" << std::endl
              << "text              Print matrix as text" << std::endl
              << std::endl
              << "Running mx <cmd> without arguments prints usage information for <cmd>"
              << std::endl
              << std::endl;
}

// TODO: fix input subcommands taking in multiple files
// vs just stdin, ie clean up code

// Setting up dictionary for easy command execution
// in our SubCommands type, we can reference a function by its name
typedef struct
{
    std::string subcmd;
    void (*display)();
    void (*parse)(int, char *[], MX_opt &);
    bool (*validate)(MX_opt &);
    void (*command)(MX_opt &);
} SubCommands;

const SubCommands functions[]{
    {"view", displayProgramOptions_view, parseProgramOptions_view, validateProgramOptions_view, mx_view},
    {"shape", displayProgramOptions_text, parseProgramOptions_text, validateProgramOptions_text, mx_shape},
    {"sort", displayProgramOptions_sort, parseProgramOptions_sort, validateProgramOptions_sort, mx_sort},
    {"sum", displayProgramOptions_sum, parseProgramOptions_sum, validateProgramOptions_sum, mx_sum},
    {"extract", displayProgramOptions_extract, parseProgramOptions_extract, validateProgramOptions_extract, mx_extract},
    {"sample", displayProgramOptions_sample, parseProgramOptions_sample, validateProgramOptions_sample, mx_sample},
    {"multiply", displayProgramOptions_multiply, parseProgramOptions_multiply, validateProgramOptions_multiply, mx_multiply},
    {"text", displayProgramOptions_text, parseProgramOptions_text, validateProgramOptions_text, mx_text},
    {"fromtext", displayProgramOptions_text, parseProgramOptions_text, validateProgramOptions_text, mx_fromtext},
};

const int function_count = sizeof(functions) / sizeof(*functions);

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