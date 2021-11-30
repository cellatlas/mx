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

// TODO: fix input subcommands taking in multiple files
// vs just stdin, ie clean up code

// Setting up dictionary for easy command execution
// in our SubCommands type, we can reference a function by its name
typedef struct
{
    std::string subcmd;
    std::string description;
    void (*display)();
    void (*parse)(int, char *[], MX_opt &);
    bool (*validate)(MX_opt &);
    void (*command)(MX_opt &);
} SubCommands;

const SubCommands functions[]{
    {"text", "Convert .mx to .mtx", displayProgramOptions_text, parseProgramOptions_text, validateProgramOptions_text, mx_text},
    {"fromtext", "Convert .mtx to .mx", displayProgramOptions_text, parseProgramOptions_text, validateProgramOptions_text, mx_fromtext},
    {"shape", "Print shape of matrix", displayProgramOptions_shape, parseProgramOptions_text, validateProgramOptions_text, mx_shape},
    {"extract", "Extract elements", displayProgramOptions_extract, parseProgramOptions_extract, validateProgramOptions_extract, mx_extract},
    {"sum", "Sum elements", displayProgramOptions_sum, parseProgramOptions_sum, validateProgramOptions_sum, mx_sum},
    {"sort", "Sort elements", displayProgramOptions_sort, parseProgramOptions_sort, validateProgramOptions_sort, mx_sort},
};

const int function_count = sizeof(functions) / sizeof(*functions);

void displayProgramOptions_MX()
{
    std::cout << "mx " << MX_VERSION << std::endl
              << std::endl
              << "Usage: mx <cmd> [arguments] ..." << std::endl
              << "where <cmd> can be one of: " << std::endl
              << std::endl;
    std::string subcmd, subcmd_sp;
    for (auto &ele : functions)
    {
        // pretty print to align descriptions
        stringify(ele.subcmd, subcmd_sp);
        std::cout << subcmd_sp << ele.description << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Running mx <cmd> without arguments prints usage information for <cmd>" << std::endl
              << std::endl;
}

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