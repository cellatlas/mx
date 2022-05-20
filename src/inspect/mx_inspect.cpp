#include "../Common.hpp"
#include "getopt.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional> // std::function
#include <algorithm>  // std::sort

#include "../formats/Matrix.h"

std::string to_json(const std::string &id, const std::string &val, bool quote,
                    bool comma, int level)
{
    std::string out;

    for (auto i = 0; i < level; ++i)
    {
        out += "\t";
    }

    out += '"';
    out += id;
    out += "\": ";
    if (quote)
    {
        out += '"';
    }
    out += val;
    if (quote)
    {
        out += '"';
    }
    if (comma)
    {
        out += ',';
    }

    return out;
}

// display
void displayProgramOptions_inspect()
{
    std::cout << "Usage: mx inspect [options] sorted mx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to inspect (default = 0)" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}

// program options
static int verbose_flag;

void parseProgramOptions_inspect(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:p";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"pipe", no_argument, 0, 'p'},
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
        case 'p':
            opt.stream_out = true;
            break;
        default:
            break;
        }
    }

    if (verbose_flag)
    {
        std::cout << "Verbose flag is set" << std::endl;
    }

    // the rest of the arguments are files

    while (optind < argc)
    {
        opt.files.push_back(argv[optind++]);
    }

    if (opt.files.size() == 1 && opt.files[0] == "-")
    {
        opt.stream_in = true;
    }
}

// validate
bool validateProgramOptions_inspect(MX_opt &opt)
{
    bool ret = true;
    return ret;
}
// function

void mx_inspect(MX_opt &opt)
{
    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    // read in header
    MXHeader h;
    MXRecord r;
    readMXHeader(inf, h);

    // metrics derived from header
    uint32_t ncells = h.idx_data[0];
    uint32_t ngenes = h.idx_data[1];
    uint32_t nvals = h.idx_data[h.ndim];
    double density = (double)nvals / (double)(ncells * ngenes);

    // metrics derived from values
    uint32_t total_counts = 0; // to sum values

    double avg_per_cell = 0;
    double avg_per_gene = 0;

    uint32_t med_per_cell = 0;
    uint32_t med_per_gene = 0;

    uint32_t min_cell = 999999;
    uint32_t max_cell = 0;

    uint32_t min_gene = 999999;
    uint32_t max_gene = 0;

    double cov_cell_count = 0;
    double cov_gene_variance = 0;
    double overdispersion = 0;

    Matrix m(ncells, ngenes);

    // setup bulk parsing
    std::string line;
    size_t N = 256;
    size_t nr = 0;
    int rc = 0;
    MXRecord *p = new MXRecord[N];
    bool readr = false;
    while (true)
    {
        // read in records
        readr = readManyMXRecords(inf, p, h, N);
        if (readr)
        {
            rc = inf.gcount() / sizeof(MXRecord);
            nr += rc;
            if (rc == 0)
            {
                break;
            }
            for (int i = 0; i < rc; i++)
            {
                // add to matrix here
                m(p[i].idx[0] - 1, p[i].idx[1] - 1) = p[i].value;
                total_counts += p[i].value;
            }

            // main code change here
        }

        // no more records or error out then break
        else
        {
            break;
        }
    }

    // compute metrics here
    // double avg_per_cell = 0;
    // double avg_per_gene = 0;

    // uint32_t med_per_cell = 0;
    // uint32_t med_per_gene = 0;

    // uint32_t min_cell = 0;
    // uint32_t max_cell = 0;

    // uint32_t min_gene = 0;
    // uint32_t max_gene = 0;

    // cell metrics
    Matrix csum(ncells, 1);

    for (int i = 0; i < ncells; i++)
    {
        for (int j = 0; j < ngenes; j++)
        {
            csum(i, 0) += m(i, j);
        }
        avg_per_cell += csum(i, 0) / ncells;
        if (csum(i, 0) < min_cell)
        {
            min_cell = csum(i, 0);
        }

        if (csum(i, 0) > max_cell)
        {
            max_cell = csum(i, 0);
        }
    }

    // CV cell depth = mean/variance

    // gene metrics
    Matrix gsum(1, ngenes);
    for (int j = 0; j < ngenes; j++)
    {
        for (int i = 0; i < ncells; i++)
        {
            gsum(0, j) += m(i, j);
        }
        avg_per_gene += gsum(0, j) / ngenes;
        if (gsum(0, j) < min_gene)
        {
            min_gene = gsum(0, j);
        }

        if (gsum(0, j) > max_gene)
        {
            max_gene = gsum(0, j);
        }
    }

    // ncells
    // ngenes
    // nvals
    // density
    // total_counts
    // avg_per_cell
    // avg_per_gene
    // med_per_cell
    // med_per_gene
    // min_cell
    // max_cell
    // min_gene
    // max_gene
    // cov_cell_count
    // cov_gene_variance
    // overdispersion

    outf << "{" << std::endl
         << to_json("ncells", std::string(std::to_string(ncells)), false, true, 1) << std::endl
         << to_json("ngenes", std::string(std::to_string(ngenes)), false, true, 1) << std::endl
         << to_json("nvals", std::string(std::to_string(nvals)), false, true, 1) << std::endl
         << to_json("density", std::string(std::to_string(density)), false, true, 1) << std::endl
         << to_json("total_counts", std::string(std::to_string(total_counts)), false, true, 1) << std::endl
         << to_json("avg_per_cell", std::string(std::to_string(avg_per_cell)), false, true, 1) << std::endl
         << to_json("avg_per_gene", std::string(std::to_string(avg_per_gene)), false, true, 1) << std::endl
         //  << to_json("med_per_cell", std::string(std::to_string(med_per_cell)), false, true, 1) << std::endl
         //  << to_json("med_per_gene", std::string(std::to_string(med_per_gene)), false, true, 1) << std::endl
         << to_json("min_cell", std::string(std::to_string(min_cell)), false, true, 1) << std::endl
         << to_json("max_cell", std::string(std::to_string(max_cell)), false, true, 1) << std::endl
         << to_json("min_gene", std::string(std::to_string(min_gene)), false, true, 1) << std::endl
         << to_json("max_gene", std::string(std::to_string(max_gene)), false, true, 1) << std::endl
         << to_json("cov_cell_count", std::string(std::to_string(cov_cell_count)), false, true, 1) << std::endl
         << to_json("cov_gene_variance", std::string(std::to_string(cov_gene_variance)), false, true, 1) << std::endl
         << to_json("overdispersion", std::string(std::to_string(overdispersion)), false, false, 1) << std::endl
         << "}" << std::endl;

    // update the header
    // close ostream
    if (!opt.stream_out)
    {
        ofs.close();
    }
    std::cerr << nr << " records inspected" << std::endl;
}