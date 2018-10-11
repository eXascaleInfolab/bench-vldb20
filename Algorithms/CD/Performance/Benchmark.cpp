//
// Created by Zakhar on 16/03/2017.
//

#include <chrono>
#include <iostream>
#include <tuple>

#include "Benchmark.h"
#include "../LinearAlgebra/CentroidDecomposition.h"
#include "../MatrixReadWrite.h"
#include "../LinearAlgebra/MissingValueRecovery.h"

using namespace LinearAlgebra_Basic;
using namespace LinearAlgebra_Algorithms;

using namespace std;

namespace Benchmark {

uint64_t hamming(Vector &sv1, Vector &sv2);

std::tuple<LinearAlgebra_Basic::Matrix *, LinearAlgebra_Basic::Matrix *, std::vector<double>> benchmarkCDoutput(LinearAlgebra_Basic::Matrix *mx)
{
    CentroidDecomposition cd = CentroidDecomposition(mx);

    vector<double> centroidValues = vector<double>();

    centroidValues.reserve(mx->dimensionM());

    cd.performDecomposition(true, &centroidValues);

    return std::make_tuple<Matrix *, Matrix *, vector<double>>(cd.getLoad()->copy(), cd.getRel()->copy(), std::move(centroidValues));
}

std::vector<int64_t> *benchmarkCDtime(Matrix *mx, uint64_t istep)
{
    CentroidDecomposition cd = CentroidDecomposition(mx);
    vector<int64_t> *results = new vector<int64_t>();

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    cd.performDecomposition(true);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    //batchCD, original matrix
    results->push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    for ( ; istep > 0; --istep)
    {
        vector<double> *newData = MathIO::getNextValue(mx->dimensionM());
        cd.increment(*newData);
        delete newData;
    }

    {
        begin = std::chrono::steady_clock::now();

        //cd.updateCD();

        end = std::chrono::steady_clock::now();

        //updateCD, original matrix -> augmented matrix
        results->push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

        // CACHED CD

        begin = std::chrono::steady_clock::now();

        cd.performDecomposition(true);

        end = std::chrono::steady_clock::now();

        //cachedCD, augmented matrix
        results->push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

        // BATCH CD

        Matrix *mx2 = mx->copy(); // copy of augmented matrix
        CentroidDecomposition cd2 = CentroidDecomposition(mx2);

        begin = std::chrono::steady_clock::now();

        cd2.performDecomposition(false);

        end = std::chrono::steady_clock::now();

        //batchCD, augmented matrix
        results->push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    }

    return results;
}

std::vector<uint64_t> *benchmarkCDssviter(Matrix *mx, uint64_t istep)
{
    CentroidDecomposition cd = CentroidDecomposition(mx);
    vector<uint64_t> *results = new vector<uint64_t>();

    cd.performDecomposition(true);

    for ( ; istep > 0; --istep)
    {
        vector<double> *newData = MathIO::getNextValue(mx->dimensionM());
        cd.increment(*newData);
        delete newData;
    }

    // while ()

    // UPDATED CD

    cd.ssvIterations = 0;

    //cd.updateCD();

    results->push_back(cd.ssvIterations);

    // CACHED CD

    cd.ssvIterations = 0;

    cd.performDecomposition(true);

    results->push_back(cd.ssvIterations);

    // BATCH CD

    Matrix *mx2 = mx->copy(); // copy of augmented matrix
    CentroidDecomposition cd2 = CentroidDecomposition(mx2);

    cd2.performDecomposition(false);

    results->push_back(cd2.ssvIterations);

    return results;
}

MemoryPrecisionBenchmark *benchmarkCDmemoryprecision(Matrix *mx)
{
    Matrix *mx_c = mx->copy();
    CentroidDecomposition cd = CentroidDecomposition(mx);

    cd.performDecomposition(false);

    MemoryPrecisionBenchmark *benchmark = new MemoryPrecisionBenchmark();
    //benchmark->memory = MemoryMeasurement::get_maximum();

    Matrix &L = *cd.getLoad();
    Matrix &R = *cd.getRel();

    Matrix *reconstructedX = matrix_mult_A_BT(L, R);

    *reconstructedX -= *mx_c;

    benchmark->precision = reconstructedX->normF();

    return benchmark;
}

int64_t benchmarkCDMVRTime(Matrix *mx, std::vector<MissingBlock> *missingblocks, uint64_t k, bool useCache, uint32_t optimization)
{
    int64_t result;

    MissingValueRecovery mvr = MissingValueRecovery(mx, 100, 1E-6);

    if (missingblocks->empty()) // MBs not provided
    {
        mvr.autoDetectMissingBlocks();
        missingblocks->reserve(mvr.missingBlocks.size());

        for (auto mblock : mvr.missingBlocks) //deepcopy
        {
            missingblocks->push_back(mblock);
        }
    }
    else
    {
        for (auto mb : *missingblocks)
        {
            mvr.addMissingBlock(mb);
        }
    }

    for (auto mblock : mvr.missingBlocks)
    {
        Vector v = Vector(mblock.blockSize, mblock.extractBlock(), true);
        //std::cout << "MB_" << mblock.column << "," << mblock.startingIndex << "," << mblock.blockSize << " =" << v.toString() << std::endl;
    }

    // test

    mvr.setReduction(k);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    (void) mvr.performRecovery(k == 0, useCache, optimization);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    result = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "Time : " << result << std::endl;

    return result;
}

int64_t benchmarkIncCDMVRTime(Matrix *mx, std::vector<MissingBlock> *missingblocks, uint64_t k, uint64_t istep)
{
    int64_t result;

    MissingValueRecovery mvr = MissingValueRecovery(mx, 100, 1E-6);

    mvr.setReduction(k);
    mvr.decomposeOnly(); // populate initial signvectors in mvr->cd->signVectors

    for ( ; istep > 0; --istep)
    {
        vector<double> *newData = MathIO::getNextValue(mx->dimensionM());
        mvr.increment(*newData); // add new data with NAN values
        delete newData;
    }

    if (!missingblocks->empty()) // MBs not provided
    {
        mvr.autoDetectMissingBlocks(); // populate the MBs vector after the added data
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    mvr.performRecovery(k == 0, true); // recover missing values. SVs will be there already
                                        // call to cd->performDecomposition doesn't mess with anything because useCache=true
                                        // all iterations rely on inital SVs, one RMV iteration will fix SVs from added data

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    result = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "Time : " << result << std::endl;

    return result;
}

} // namespace Benchmark