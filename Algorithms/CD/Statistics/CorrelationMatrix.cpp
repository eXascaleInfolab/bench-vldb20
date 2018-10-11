//
// Created by zakhar on 13.03.18.
//

#include <cstring>
#include <cmath>
#include <tuple>
#include "CorrelationMatrix.h"
#include "../LinearAlgebra/SVDecomposition.h"

using namespace LinearAlgebra_Basic;

namespace Stats {
//
// Constructors & destructors
//

CorrelationMatrix::CorrelationMatrix(LinearAlgebra_Basic::Matrix *mx)
        : Src(mx),
          correlation(LinearAlgebra_Basic::Matrix::identity(mx->dimensionM())),
          mean(std::vector<double>(Src->dimensionM())),
          stddev(std::vector<double>(Src->dimensionM()))
{
    std::memset(mean.data(), 0, Src->dimensionM());
    std::memset(stddev.data(), 0, Src->dimensionM());
}

//
// API
//
LinearAlgebra_Basic::Matrix *Stats::CorrelationMatrix::getCorrelationMatrix()
{
    // step 1 - mean and stddev
    setMeanAndStdDev();

    // step 2 - calculate covariance
    for (uint64_t j1 = 0; j1 < Src->dimensionM() - 1; ++j1)
    {
        for (uint64_t j2 = j1 + 1; j2 < Src->dimensionM(); ++j2)
        {
            (*correlation)(j1, j2) = (*correlation)(j2, j1) = getCorrelation(j1, j2);
        }
    }

    return correlation;
}

LinearAlgebra_Basic::Vector *Stats::CorrelationMatrix::getSingularValuesOfCM()
{
    if (correlation == nullptr)
    {
        correlation = getCorrelationMatrix();
    }
    Vector *sigma = new Vector(correlation->dimensionN(), false);
    Matrix svd_v(correlation->dimensionM(), correlation->dimensionM(), false);

    LinearAlgebra_Algorithms::SVDecomposition::SVDecompose(*correlation, *sigma, svd_v);

    uint64_t n = sigma->dimension();

    for (uint64_t shell = n/2; shell > 0; shell /= 2)//todo:replace with insertion, sigma is almost sorted
    {
        for (uint64_t i = shell; i < n; ++i)
        {
            double temp = (*sigma)[i];
            uint64_t j;
            for (j = i; j >= shell && (*sigma)[j - shell] < temp; j -= shell) {
                (*sigma)[j] = (*sigma)[j - shell];
            }
            (*sigma)[j] = temp;
        }
    }

    return sigma;
}


void CorrelationMatrix::normalizeMatrix()
{
    // step 1 - mean and stddev
    setMeanAndStdDev();

    // step 2 - normalize
    Matrix &mx = *Src;
    for (uint64_t i = 0; i < mx.dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < mx.dimensionM(); ++j)
        {
            mx(i,j) = (mx(i,j) - mean[j]) / stddev[j];
        }
    }
}


void CorrelationMatrix::deNormalizeMatrix()
{
    Matrix &mx = *Src;
    for (uint64_t i = 0; i < mx.dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < mx.dimensionM(); ++j)
        {
            mx(i,j) = (mx(i,j) * stddev[j]) + mean[j];
        }
    }
}

//
// Algorithm
//

void Stats::CorrelationMatrix::setMeanAndStdDev()
{
    std::vector<double> shift = std::vector<double >();

    for (uint64_t j = 0; j < Src->dimensionM(); ++j)
    {
        shift.emplace_back(
                ((*Src)(j, 0)
                 + (*Src)(j, Src->dimensionM() - 1)
                 + (*Src)(j, (Src->dimensionM() - 1) / 2)
                ) / 3
        );
    }

    for (uint64_t i = 0; i < Src->dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < Src->dimensionM(); ++j)
        {
            double entry = (*Src)(i, j) - shift[j];
            mean[j] += entry;
            stddev[j] += entry * entry;
        }
    }

    for (uint64_t j = 0; j < Src->dimensionM(); ++j)
    {
        stddev[j] -= (mean[j] * mean[j]) / Src->dimensionN();
        stddev[j] /= Src->dimensionN() - 1;
        stddev[j] = std::sqrt(stddev[j]);

        mean[j] /= Src->dimensionN();
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

    for (uint64_t i = 0; i < Src->dimensionN(); ++i)
    {
        double val1 = (*Src)(i, col1);
        double val2 = (*Src)(i, col2);

        cov += ((val1 - mean1) / stddev1) * ((val2 - mean2) / stddev2);
    }

    return cov / (Src->dimensionN() - 1);
}



} // namespace Stats



