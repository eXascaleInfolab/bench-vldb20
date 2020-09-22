//
// Created by zakhar on 13.03.18.
//

#include <cmath>

#include "Correlation.h"

namespace Stats
{
//
// Constructors & destructors
//

CorrelationMatrix::CorrelationMatrix(arma::mat &mx)
        : matrix(mx),
          mean(std::vector<double>(matrix.n_cols)),
          stddev(std::vector<double>(matrix.n_cols))
{
    correlation = new arma::mat();
    correlation->eye(mx.n_cols, mx.n_cols);
}

CorrelationMatrix::~CorrelationMatrix()
{
    delete correlation;
}

//
// API
//
arma::mat &Stats::CorrelationMatrix::getCorrelationMatrix()
{
    // step 1 - mean and stddev
    setMeanAndStdDev();
    
    // step 2 - calculate covariance
    for (uint64_t j1 = 0; j1 < matrix.n_cols - 1; ++j1)
    {
        for (uint64_t j2 = j1 + 1; j2 < matrix.n_cols; ++j2)
        {
            (*correlation)(j1, j2) = (*correlation)(j2, j1) = getCorrelation(j1, j2);
        }
    }
    
    corrCalculated = true;
    
    return *correlation;
}

arma::vec Stats::CorrelationMatrix::getSingularValuesOfCM()
{
    if (!corrCalculated)
    {
        getCorrelationMatrix();
    }
    
    arma::vec sigma(correlation->n_rows);
    arma::mat svd_v(correlation->n_cols, correlation->n_cols);
    arma::mat svd_U(correlation->n_rows, correlation->n_cols);
    
    arma::svd_econ(svd_U, sigma, svd_v, *correlation);
    
    uint64_t n = sigma.n_elem;
    
    for (uint64_t shell = n / 2; shell > 0; shell /= 2)
    {
        for (uint64_t i = shell; i < n; ++i)
        {
            double temp = sigma[i];
            uint64_t j;
            for (j = i; j >= shell && sigma[j - shell] < temp; j -= shell)
            {
                sigma[j] = sigma[j - shell];
            }
            sigma[j] = temp;
        }
    }
    
    return sigma;
}


void CorrelationMatrix::normalizeMatrix()
{
    // step 1 - mean and stddev
    setMeanAndStdDev();
    
    // step 2 - normalize
    for (uint64_t i = 0; i < matrix.n_rows; ++i)
    {
        for (uint64_t j = 0; j < matrix.n_cols; ++j)
        {
            matrix(i, j) = (matrix(i, j) - mean[j]) / stddev[j];
        }
    }
}


void CorrelationMatrix::deNormalizeMatrix()
{
    for (uint64_t i = 0; i < matrix.n_rows; ++i)
    {
        for (uint64_t j = 0; j < matrix.n_cols; ++j)
        {
            matrix(i, j) = (matrix(i, j) * stddev[j]) + mean[j];
        }
    }
}

//
// Algorithm
//

void Stats::CorrelationMatrix::setMeanAndStdDev()
{
    std::vector<double> shift = std::vector<double>(matrix.n_cols);
    
    for (uint64_t j = 0; j < matrix.n_cols; ++j)
    {
        mean[j] = 0.0;
        stddev[j] = 0.0;
        
        shift[j]  = (matrix(0, j)
                     + matrix(matrix.n_rows - 1, j)
                     + matrix((matrix.n_rows - 1) / 2, j)
                    ) / 3;
    }
    
    for (uint64_t i = 0; i < matrix.n_rows; ++i)
    {
        for (uint64_t j = 0; j < matrix.n_cols; ++j)
        {
            double entry = matrix(i, j) - shift[j];
            mean[j] += entry;
            stddev[j] += entry * entry;
        }
    }
    
    for (uint64_t j = 0; j < matrix.n_cols; ++j)
    {
        stddev[j] -= (mean[j] * mean[j]) / (double)matrix.n_rows;
        stddev[j] /= (double)(matrix.n_rows - 1);
        stddev[j] = std::sqrt(stddev[j]);
    
        mean[j] /= (double)matrix.n_rows;
        mean[j] += shift[j];
    }
}

double Stats::CorrelationMatrix::getCorrelation(uint64_t col1, uint64_t col2)
{
    double mean1 = mean[col1];
    double mean2 = mean[col2];
    
    double stddev1 = stddev[col1];
    double stddev2 = stddev[col2];
    
    double cov = 0;
    
    for (uint64_t i = 0; i < matrix.n_rows; ++i)
    {
        double val1 = matrix(i, col1);
        double val2 = matrix(i, col2);
        
        cov += ((val1 - mean1) / stddev1) * ((val2 - mean2) / stddev2);
    }
    
    return cov / (double)(matrix.n_rows - 1);
}

const std::vector<double> &CorrelationMatrix::getMean() const
{
    return mean;
}

const std::vector<double> &CorrelationMatrix::getStddev() const
{
    return stddev;
}


} // namespace Stats



