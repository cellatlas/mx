#include "getopt.h"
#include "Common.hpp"
#include <iostream>

static int verbose_flag;

void parseProgramOptions_view(int argc, char *argv[], MX_opt &opt)
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

void parseProgramOptions_sum(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:a:";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"axis", required_argument, 0, 'a'},
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
        case 'a':
            opt.axis = atoi(optarg);
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

void parseProgramOptions_sort(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:a:";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"axis", required_argument, 0, 'a'},
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
        case 'a':
            opt.axis = atoi(optarg);
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