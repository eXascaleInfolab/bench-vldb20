//
// Created by Zakhar on 16/03/2017.
//

#include <chrono>
#include <iostream>
#include <tuple>

#include "Benchmark.h"
#include <cassert>

#include "../Algorithms/MeanImpute.h"
#include "../Algorithms/LinearImpute.h"

using namespace Algorithms;

namespace Performance
{

void verifyRecovery(arma::mat &mat)
{
    for (uint64_t j = 0; j < mat.n_cols; ++j)
    {
        for (uint64_t i = 0; i < mat.n_rows; ++i)
        {
            if (std::isnan(mat.at(i, j)))
            {
                mat.at(i, j) = std::sqrt(std::numeric_limits<double>::max() / 100000.0);
            }
        }
    }
}

int64_t Recovery_MeanImpute(arma::mat &mat)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    //mat = mat.t();
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    MeanImpute::MeanImpute_Recovery(mat);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (MeanImpute): " << result << std::endl;
    
    //mat = mat.t();
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_LinearImpute(arma::mat &mat)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    //mat = mat.t();
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    LinearImpute::LinearImpute_Recovery(mat);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (LinearImpute): " << result << std::endl;
    
    //mat = mat.t();
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery(arma::mat &mat, uint64_t truncation,
                 const std::string &algorithm, const std::string &xtra)
{
	(void)truncation; (void)xtra;
    if (algorithm == "meanimp")
    {
        return Recovery_MeanImpute(mat);
    }
    else if (algorithm == "linimp")
    {
        return Recovery_LinearImpute(mat);
    }
    else
    {
        std::cout << "Algorithm name '" << algorithm << "' is not valid" << std::endl;
        abort();
    }
}

} // namespace Performance
