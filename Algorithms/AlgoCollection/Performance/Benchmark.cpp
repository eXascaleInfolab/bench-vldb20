//
// Created by Zakhar on 16/03/2017.
//

#include <chrono>
#include <iostream>
#include <tuple>

#include "Benchmark.h"
#include <cassert>

#include "../Algorithms/CDMissingValueRecovery.h"
#include "../Algorithms/TKCM.h"
#include "../Algorithms/ST_MVL.h"
#include "../Algorithms/SPIRIT.h"
#include "../Algorithms/GROUSE.h"
#include "../Algorithms/NMFMissingValueRecovery.h"
#include "../Algorithms/DynaMMo.h"
#include "../Algorithms/SVT.h"
#include "../Algorithms/ROSL.h"
#include "../Algorithms/IterativeSVD.h"
#include "../Algorithms/SoftImpute.h"

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

int64_t Recovery_CD(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    CDMissingValueRecovery rmv(mat);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    rmv.setReduction(truncation);
    rmv.disableCaching = false;
    rmv.useNormalization = false;
    
    begin = std::chrono::steady_clock::now();
    rmv.autoDetectMissingBlocks();
    rmv.performRecovery(truncation == mat.n_cols);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (CDRec): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_TKCM(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    Algorithms::TKCM tkcm(mat, truncation);
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    // Recovery

    begin = std::chrono::steady_clock::now();
    tkcm.performRecovery();
    end = std::chrono::steady_clock::now();

    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (TKCM): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_ST_MVL(arma::mat &mat, uint64_t truncation, const std::string &latlong)
{
    // Local
    int64_t result;

    double alpha = ((double)truncation) / 1000.0;
    ST_MVL stmvl(mat, latlong, alpha, 0.85, 7);

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    // Recovery

    begin = std::chrono::steady_clock::now();
    stmvl.Run(true);
    end = std::chrono::steady_clock::now();

    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (STMVL): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_SPIRIT(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    SPIRIT::doSpirit(mat, truncation, 6, 1.0);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (SPIRIT): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_GROUSE(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    // Recovery

    mat = mat.t();

    begin = std::chrono::steady_clock::now();
    GROUSE::doGROUSE(mat, truncation);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (GROUSE): " << result << std::endl;
    
    mat = mat.t();
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_NNMF(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    NMFMissingValueRecovery::doNMFRecovery(mat, truncation);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (TeNMF): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_DynaMMo(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    
    mat = mat.t();
    
    begin = std::chrono::steady_clock::now();
    DynaMMo::doDynaMMo(mat, truncation, 100, true);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (DynaMMo): " << result << std::endl;
    
    mat = mat.t();
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_SVT(arma::mat &mat, const std::string &xtra)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    double tauScale = std::stod(xtra);
    // Recovery
    begin = std::chrono::steady_clock::now();
    SVT::doSVT(mat, tauScale);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (SVT): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_ROSL(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    ROSL::ROSL_Recovery(mat, truncation, 0.6);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (ROSL): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_IterativeSVD(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    IterativeSVD::recoveryIterativeSVD(mat, truncation);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (SVDImpute): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery_SoftImpute(arma::mat &mat, uint64_t truncation)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    SoftImpute::doSoftImpute(mat, truncation);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (SoftImpute): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}

int64_t Recovery(arma::mat &mat, uint64_t truncation,
                 const std::string &algorithm, const std::string &xtra)
{
    if (algorithm == "cd")
    {
        return Recovery_CD(mat, truncation);
    }
    else if (algorithm == "tkcm")
    {
        return Recovery_TKCM(mat, truncation);
    }
    else if (algorithm == "st-mvl")
    {
        return Recovery_ST_MVL(mat, truncation, xtra);
    }
    else if (algorithm == "spirit")
    {
        return Recovery_SPIRIT(mat, truncation);
    }
    else if (algorithm == "grouse")
    {
        return Recovery_GROUSE(mat, truncation);
    }
    else if (algorithm == "nnmf")
    {
        return Recovery_NNMF(mat, truncation);
    }
    else if (algorithm == "dynammo")
    {
        return Recovery_DynaMMo(mat, truncation);
    }
    else if (algorithm == "svt")
    {
        return Recovery_SVT(mat, xtra);
    }
    else if (algorithm == "rosl")
    {
        return Recovery_ROSL(mat, truncation);
    }
    else if (algorithm == "itersvd")
    {
        return Recovery_IterativeSVD(mat, truncation);
    }
    else if (algorithm == "softimpute")
    {
        return Recovery_SoftImpute(mat, truncation);
    }
    else
    {
        std::cout << "Algorithm name '" << algorithm << "' is not valid" << std::endl;
        abort();
    }
}

} // namespace Performance
