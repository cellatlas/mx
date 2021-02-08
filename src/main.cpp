#include "Common.hpp" // MX_opt is here
#include "mx_view.h"  // mx_view

#include <stdlib.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

static int verbose_flag;

void parseProgramOptions(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {0, 0, 0, 0}};

    int c;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'o':
            opt.output = optarg;
            break;
        case '?':
            std::cout << "error" << std::endl;
        default:
            break;
        }
    }
    if (verbose_flag)
    {
        std::cout << "Verbose flag is set" << std::endl;
    }

    // the rest of the arguments are files
    if (optind < argc)
    {
        while (optind < argc)
        {
            opt.files.push_back(argv[optind++]);
        }
    }
}

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
        parseProgramOptions(argc - 1, argv + 1, opt);
        mx_view(opt);
    }
    else if (cmd == "shape")
    {
        parseProgramOptions(argc - 1, argv + 1, opt);
        mx_shape(opt);
    }
    else
    {
        std::cerr << "mx command not found: " << cmd << std::endl;
        exit(1);
    }

    return 0;
}