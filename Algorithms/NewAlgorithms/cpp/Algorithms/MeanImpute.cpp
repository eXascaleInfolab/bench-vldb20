#include <iostream>
#include "MeanImpute.h"

namespace Algorithms
{

void MeanImpute::MeanInpute_Recovery(arma::mat &input)
{
    arma::vec mean = arma::zeros<arma::vec>(input.n_cols);
    arma::uvec values = arma::zeros<arma::uvec>(input.n_cols);
    for (uint64_t j = 0; j < input.n_cols; ++j)
    {
        for (uint64_t i = 0; i < input.n_rows; i++)
        {
            if (arma::is_finite(input(i, j)))
            {
                mean[j] += input(i, j);
                values[j]++;
            }
        }
        
        if (values[j] == 0) mean[j] = 0.0; // full column is missing, impute with 0
        else mean[j] /= (double)values[j];
    }
    for (uint64_t j = 0; j < input.n_cols; ++j)
    {
        // nothing is missing
        if (values[j] == input.n_rows) continue;
        for (uint64_t i = 0; i < input.n_rows; i++)
        {
            if (!arma::is_finite(input(i, j))) input(i, j) = mean[j];
        }
    }
}

// any other functions go here

} // namespace Algorithms