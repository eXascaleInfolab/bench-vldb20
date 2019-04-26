//
// Created by zakhar on 14.01.19.
//

#include "NMFMissingValueRecovery.h"

#include <iostream>

#include <mlpack/methods/amf/amf.hpp>

namespace Algorithms
{

void NMFMissingValueRecovery::doNMFRecovery(arma::mat &matrix, uint64_t truncation)
{
    arma::arma_rng::set_seed(18931); // code here doesn't use RNG, but AMF<> uses randomized start
    
    arma::mat input(matrix);
    
    std::vector<arma::uvec> indices;

    for (uint64_t i = 0; i < input.n_cols; ++i)
    {
        indices.emplace_back(arma::find_nonfinite(input.col(i)));
    }

    // step 1 : calculate shift

    double shift = std::abs(input.min()) + 1.0;

    for (uint64_t j = 0; j < input.n_cols; ++j)
    {
        for (uint64_t i = 0; i < input.n_rows; ++i)
        {
            if (!std::isnan(input.at(i, j)))
            {
                input.at(i, j) += shift;
            }
        }
    }

    // step 2 : init missing values with 0

    for (uint64_t i = 0; i < input.n_cols; ++i)
    {
        for (uint64_t j : indices[i])
        {
            input.at(j, i) = 0.0; // init at 0 for recovery to converge in one direction for all elements
        }
    }

    // step 3 : recovery loop

    arma::mat w;
    arma::mat h;

    mlpack::amf::AMF<mlpack::amf::SimpleToleranceTermination<arma::mat>,
            mlpack::amf::RandomInitialization,
            mlpack::amf::NMFALSUpdate> nmf;
    
    // termination information is set to be the same like it was used in sklearn in the original implementations
    mlpack::amf::SimpleToleranceTermination<arma::mat> term = nmf.TerminationPolicy();
    term.Tolerance() = 1E-4;
    term.MaxIterations() = 200;
    
    arma::mat reconstruction;

    double err = 99.0;
    uint64_t iter = 0;

    while (err >= 1E-6 && iter++ < 100)
    {
        nmf.Apply((const arma::mat &)input, truncation, w, h);

        reconstruction = w * h;

        err = 0.0;

        for (uint64_t j = 0; j < input.n_cols; ++j)
        {
            for (uint64_t i : indices[j])
            {
                double lastVal = input.at(i, j);
                double newVal = reconstruction.at(i, j);

                err += (lastVal - newVal) * (lastVal - newVal);

                input.at(i, j) = newVal;
            }
        }

        err = std::sqrt(err);
    }

    // step X : roll back the shift and put the values back into the original
    
    for (uint64_t j = 0; j < input.n_cols; ++j)
    {
        for (uint64_t i : indices[j])
        {
            matrix.at(i, j) = input.at(i, j) - shift;
        }
    }

    // nothing to do here, values are imputed directly into arma::mat &input
}

} // namespace Algorithms
