#include "../Common.hpp"
// #include "GMM/GMM.h"
#include "getopt.h"

#include <fstream>  // std::ifstream
#include <string>   // string
#include <sstream>  // stringstream
#include <iostream> // std::cout
#include <vector>

// display
void displayProgramOptions_filter()
{
    std::cout << "Usage: mx filter [options] mx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to filter" << std::endl
              << "-p, --pipe            Pipe output to standard out" << std::endl
              << std::endl;
}
// parse
static int verbose_flag;

void parseProgramOptions_filter(int argc, char *argv[], MX_opt &opt)
{
    const char *optstring = "o:a:p";
    static struct option long_options[] =
        {
            {"verbose", no_argument, &verbose_flag, 1},
            {"output", required_argument, 0, 'o'},
            {"axis", required_argument, 0, 'a'},
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
        case 'a':
            opt.axis = atoi(optarg);
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
bool validateProgramOptions_filter(MX_opt &opt)
{
    bool ret = true;
    return ret;
}

// function

void mx_filter(MX_opt &opt)
{
    std::cerr << "[warn] .mx file must be sorted along axis " << opt.axis << std::endl;

    // file direction
    std::ifstream ifs;
    std::ofstream ofs;
    std::streambuf *ibuf, *obuf = nullptr;
    file_direction(opt.files[0], opt.output, opt.stream_in, opt.stream_out, ibuf, obuf, ifs, ofs);
    std::istream inf(ibuf);
    std::ostream outf(obuf);

    // read in header
    MXHeader h;
    MXRecord prev_r, r;
    readMXHeader(inf, h);

    // set axis along which to filter
    int axis = opt.axis;

    // filter variables
    std::string type_covariance = "full"; // <-> "diagonal"
    int n_pts = h.idx_data[axis];         // number of points
    int n_dim = 1;                        // dimension of space
    int n_itr = 200;                      // number of EM iterations
    int n_ctr = 2;                        // number of centroids
    double **data = new double *[n_pts];  // container for points
    for (int i = 0; i < n_pts; i++)
    {
        data[i] = new double[n_dim]; // make space for the data
        data[i][0] = 0;
    }
    double ll;
    double sum = 0.0;
    int pidx = 0;
    size_t j = 0;
    bool first = true;
    int idx_diff = 0;
    std::string means_fn = opt.output + "means.txt";
    std::string assignments_fn = opt.output + "assignments.txt";

    // setup bulk parsing
    std::string line;
    size_t N = 1000;
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

            // main code change here
            for (size_t i = 0; i < rc;)
            {
                data[pidx][0] += p[i].value;
                j = i + 1;
                for (; j < rc; j++)
                {
                    idx_diff = p[j].idx[axis] - p[i].idx[axis];
                    // printMXRecord(std::cout, r, h);
                    if (idx_diff > 0)
                    {
                        pidx += idx_diff;
                        break;
                    }
                    data[pidx][0] += p[j].value;
                }
                // increase the index
                i = j;
            }
        }

        else
        {
            break;
        }
    }
    for (int k = 0; k < pidx + 1; k++)
    {
        printf("(%d, %f)\n", k, data[k][0]);
    }

    // std::cout << "matrix: " << std::endl;
    // printMatrix(n_pts, n_dim, data);

    // GaussianMixtureModel GMM = GaussianMixtureModel(type_covariance, n_dim, n_ctr);
    // // initialize with kmeans
    // std::cerr << "[assign] initialization" << std::endl;
    // GMM.Initialize(n_pts, data);

    // std::cerr << "[assign] expectation maximization" << std::endl;
    // double eps = 0.001;
    // double diff = 100;
    // int i = 0;
    // double prev_ll = 0;
    // while (diff > eps && i < n_itr)
    // {
    //     // actually do the EM algorithm
    //     ll = GMM.ExpectationMaximization(n_pts, data);
    //     std::cerr << "[assign] iter: " << i + 1 << "\t"
    //               << "ll: " << ll << std::endl;
    //     diff = std::abs(ll - prev_ll);
    //     prev_ll = ll;
    //     i++;
    // }

    // // // print centroid means
    // for (int i = 0; i < n_ctr; i++)
    // {
    //     for (int j = 0; j < n_dim; j++)
    //     {
    //         printf("%lf ", GMM.means[i][j]);
    //     }
    //     printf("\n");
    // }

    // for (int j = 0; j < n_ctr; j++)
    // {
    //     for (int i = 0; i < n_pts; i++)
    //     {
    //         if (GMM.Classify(data[i]) == j)
    //         {
    //             outf << GMM.Classify(data[i]) << " " << data[i][0] << " " << data[i][1] << std::endl;
    //         }
    //     }
    // }

    for (int i = 0; i < n_pts; i++)
    {
        delete[] data[i];
    }
    delete[] data;

    // close ostream
    if (!opt.stream_out)
    {
        ofs.close();
    }
    // std::cerr << "Parsed " << nr << " records" << std::endl;
}