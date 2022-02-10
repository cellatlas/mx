#include "../Common.hpp"
// #include "GMM/GMM.h"
#include "getopt.h"

#include <fstream>  // std::ifstream
#include <string>   // string
#include <sstream>  // stringstream
#include <iostream> // std::cout
#include <vector>

#include "mlpack/core.hpp"
#include "mlpack/methods/gmm/gmm.hpp"
#include "mlpack/methods/gmm/no_constraint.hpp"
#include "mlpack/core/math/log_add.hpp"

// display
void displayProgramOptions_filter()
{
    std::cout << "Usage: mx filter [options] mx-files" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "-o, --output          File for output" << std::endl
              << "-a, --axis=<integer>  Axis along which to filter" << std::endl
              << std::endl;
}
// parse
static int verbose_flag;

void parseProgramOptions_filter(int argc, char *argv[], MX_opt &opt)
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
    readMXHeader(inf, h);

    // set axis along which to filter
    int axis = opt.axis;

    // filter variables
    std::string type_covariance = "full"; // <-> "diagonal"
    int n_val = h.idx_data[h.ndim];       // number of mx records
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

    int pidx = 0;
    size_t j = 0;
    int idx_diff = 0;
    std::string means_fn = opt.output + "means.txt";
    std::string assignments_fn = opt.output + "assignments.txt";

    // setup bulk parsing
    std::string line;
    size_t N = h.idx_data[h.ndim];
    size_t nr = 0;
    int rc = 0;
    MXRecord *p = new MXRecord[N];
    bool readr = false;

    // read in all of the records
    readr = readManyMXRecords(inf, p, h, N);

    // loop through all of the records and sum the elements data
    sumMXRecords(axis, n_val, p, data, true);

    // for (int k = 0; k < n_pts; k++)
    // {
    //     printf("(%d, %f)\n", k, data[k][0]);
    // }

    arma::mat mat(n_dim, n_pts, arma::fill::zeros);
    // armadillo expects gene x cell
    int i = 0;
    for (int i = 0; i < n_pts; i++)
    {
        // mat is 1 x n_pts in size
        mat.col(i) = data[i][0];
    }
    // mat.print("matrix:");

    // // // model the data as a diagonal GMM with 2 Gaussians
    // Initialize GMM.
    mlpack::gmm::GMM *gmm = NULL;
    int km_itr = 1;
    double tol = 1e-10;
    int trials = 1; // number of times to run the gmm (not number of iter)
    double likelihood = 0.0;
    double prob = 0.0;

    gmm = new mlpack::gmm::GMM(n_ctr, n_dim);
    mlpack::kmeans::KMeans<> k(km_itr);
    mlpack::gmm::EMFit<mlpack::kmeans::KMeans<>, mlpack::gmm::PositiveDefiniteConstraint> em(n_itr, tol, k);
    likelihood = gmm->Train(mat, trials, false, em);
    // mlpack::distribution::GaussianDistribution c = gmm->Component(0);
    printf("(%f,%f)\n", gmm->Component(0).Probability(mat.col(0)), gmm->Component(1).Probability(mat.col(0)));

    // maybe probability for each component
    // doesnt normalize so its just the probability
    // of seeing that point given that distribution
    // but we need to normalize it by the weight of the
    // gaussian.
    arma::vec lps;
    lps.set_size(mat.n_cols);

    // Store log-probability value in a matrix.
    arma::mat logProb(mat.n_cols, n_ctr);
    arma::vec w = gmm->Weights();
    // Assign value to the matrix.
    for (size_t i = 0; i < n_ctr; i++)
    {
        arma::vec temp(logProb.colptr(i), mat.n_cols, false, true);
        gmm->Component(i).LogProbability(mat, temp);
        // temp.print("temp:");
    }
    // Save log(weights) as a vector.
    arma::vec logWeights = arma::log(w);

    // Compute log-probability.
    logProb += repmat(logWeights.t(), logProb.n_rows, 1);
    logProb.print("Logprob:");
    mlpack::math::LogSumExp(logProb, lps);
    logProb.print("hello:");
    // arma::mat lpp = arma::exp(logProb);
    // lpp.print("lpp:");
    // arma::vec w = gmm->Weights();

    // for (int i = 0; i < n_pts; i++)
    // {
    //     for (int j = 0; j < n_ctr; j++)
    //     {
    //         prob = gmm->LogProbability(mat.unsafe_col(i), j);
    //         w = gmm->Weights()[j];
    //         printf("%f ", prob);
    //     }
    //     printf("\n");
    //     break;
    // }

    printf("(%d, %d)\n", (int)mat.n_rows, (int)mat.n_cols);

    for (int i = 0; i < n_pts; i++)
    {
        for (int j = 0; j < n_ctr; j++)
        {
            prob = gmm->LogProbability(mat.unsafe_col(i), j);
            printf("%f ", prob);
        }
        printf("\n");
        break;
    }

    // arma::gmm_full model;
    // // .learn(data, n_gaus, dist_mode, seed_mode, km_iter, em_iter, var_floor, print_mode)
    // bool status = model.learn(
    //     mat,
    //     n_ctr,
    //     arma::maha_dist,
    //     arma::static_subset,
    //     1,
    //     n_itr,
    //     1e-10,
    //     false);

    // if (status == false)
    // {
    //     std::cout << "learning failed" << std::endl;
    // }

    // // model.means.print("means:");
    // for (int i = 0; i < n_ctr; i++)
    // {
    //     std::cout << std::exp(model.means[i]) - 1 << std::endl;
    // }
    // // need to assign the points and get the mahalanobis distances
    // // then compute the entropies
    // // then find where entropy is maximized

    // arma::urowvec gaus_ids = model.assign(mat, arma::prob_dist);

    // for (int i = 0; i < n_pts; i++)
    // {
    //     std::cout << gaus_ids[i] << std::endl;
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