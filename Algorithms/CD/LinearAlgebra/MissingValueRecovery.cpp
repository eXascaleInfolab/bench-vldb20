//
// Created by zakhar on 05.03.18.
//

#include <cmath>
#include "MissingValueRecovery.h"
#include "../Statistics/CorrelationMatrix.h"

#include <iostream>

using namespace LinearAlgebra_Basic;

namespace LinearAlgebra_Algorithms {

    MissingValueRecovery::MissingValueRecovery(LinearAlgebra_Basic::Matrix *Src,
                                               uint64_t maxIterations,
                                               double eps)
            : maxIterations(maxIterations),
              epsPrecision(eps) //defaults are hardcoded in MVR.h
    {
        this->Src = Src;
        k = Src->dimensionM() - 1;

        this->cd = new CentroidDecomposition(Src, k);

        missingBlocks = std::vector<MissingBlock>();
    }

    uint64_t MissingValueRecovery::getReduction() {
        return k;
    }

    void MissingValueRecovery::setReduction(uint64_t k) {
        this->k = k;
        cd->truncation = k;
    }

    void MissingValueRecovery::addMissingBlock(uint64_t col, uint64_t start, uint64_t size) {
        missingBlocks.emplace_back(MissingBlock(col, start, size, Src));
    }

    void MissingValueRecovery::addMissingBlock(MissingBlock mb) {
        missingBlocks.emplace_back(mb);
    }

    void MissingValueRecovery::autoDetectMissingBlocks(double val) {
        Matrix &mx = *Src;
        for (uint64_t j = 0; j < mx.dimensionM(); ++j)
        {
            bool missingBlock = false;
            uint64_t start = 0;

            for (uint64_t i = 0; i < mx.dimensionN(); ++i)
            {
                if ((std::isnan(val) && std::isnan(mx(i, j))) || (!std::isnan(val) && mx(i, j) == val))
                {
                    if (!missingBlock)
                    {
                        missingBlock = true;
                        start = i;
                    }
                }
                else
                {
                    if (missingBlock)
                    {
                        //finalize block
                        missingBlock = false;
                        addMissingBlock(j, start, i - start);
                    }
                }
            }

            if (missingBlock)
            {
                addMissingBlock(j, start, mx.dimensionN() - start);
            }
        }
    }

    //
    // Algorithm
    //
    void MissingValueRecovery::decomposeOnly()
    {
        this->cd->performDecomposition(true, nullptr);
    }

    void MissingValueRecovery::increment(std::vector<double> &newData)
    {
        cd->increment(newData);
    }

#define RECOVERY_VERBOSE_
#define determine_reduction_nonstat
    uint64_t MissingValueRecovery::performRecovery(bool determineReduction /*= false*/, bool useCache /*= false*/, uint32_t optimization)
    {
        uint64_t totalMBSize = 0;

        for (auto mblock : missingBlocks)
        {
            totalMBSize += mblock.blockSize;
        }

        initialize();

        uint64_t iter = 0;
        double delta = 99.0;

        Stats::CorrelationMatrix cm(Src);
        //cm.normalizeMatrix();

        if (determineReduction)
        {
            this->determineReduction();
        }

        while (++iter <= maxIterations && delta >= epsPrecision)
        {
            std::vector<double> centroidValues = std::vector<double>();

            if (optimization > 0)
            {
                if (iter != 1) {
                    std::cout << "iteration #" << iter << ", delta=" << delta << std::endl;
                }

                bool skipSSV;

                switch (optimization)
                {
                    case 100:
                        this->cd->performDecomposition(true, &centroidValues, false, false);
                        break;

                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        skipSSV = iter >= (optimization + 1);
                        this->cd->performDecomposition(false, &centroidValues, false, skipSSV);
                        break;

                    case 101:
                    case 102:
                    case 103:
                    case 104:
                    case 105:
                    case 106:
                    case 107:
                        skipSSV = iter >= (optimization + 1);
                        this->cd->performDecomposition(true, &centroidValues, false, skipSSV);
                        break;

                    case 10:
                        skipSSV = delta <= 1E-2;
                        this->cd->performDecomposition(false, &centroidValues, false, skipSSV);
                        break;

                    case 110:
                        skipSSV = delta <= 1E-2;
                        this->cd->performDecomposition(true, &centroidValues, false, skipSSV);
                        break;

                    case 20:
                        skipSSV = iter != 1 && iter != 7;
                        this->cd->performDecomposition(false, &centroidValues, false, skipSSV);
                        break;

                    case 120:
                        skipSSV = iter != 1 && iter != 7;
                        this->cd->performDecomposition(true, &centroidValues, false, skipSSV);
                        break;

                    case 25:
                        skipSSV = iter > 3 && iter % 2 == 0; //skips 4, 6, 8 ...
                        this->cd->performDecomposition(false, &centroidValues, false, skipSSV);
                        break;

                    case 125:
                        skipSSV = iter > 3 && iter % 2 == 0;
                        this->cd->performDecomposition(true, &centroidValues, false, skipSSV);
                        break;

                    case 301:
                    case 302:
                    case 303:
                    case 304:
                    case 305:
                    case 306:
                    case 307:
                        skipSSV = iter >= (optimization + 1);
                        this->cd->performDecomposition(skipSSV, &centroidValues, false, false);
                        break;

                    case 310:
                        skipSSV = delta <= 1E-2;
                        this->cd->performDecomposition(skipSSV, &centroidValues, false, false);
                        break;

                    case 325:
                        skipSSV = iter % 2 == 0;
                        this->cd->performDecomposition(skipSSV, &centroidValues, false, false);
                        break;

                    case 330:
                        skipSSV = iter != 1 && iter != 5;
                        this->cd->performDecomposition(skipSSV, &centroidValues, false, false);
                        break;

                    case 331:
                        skipSSV = iter != 1 && iter != 4 && iter != 7;
                        this->cd->performDecomposition(skipSSV, &centroidValues, false, false);
                        break;

                    default:
                        std::cout << "Unrecognized optimization type " << optimization << " for RCD";
                        exit(-1);
                }
            }
            else
            {
                this->cd->performDecomposition(useCache, &centroidValues, false, false);
            }

#ifdef RECOVERY_VERBOSE
            std::cout << "CValues: ";
            for (auto a : centroidValues)
            {
                std::cout << a << " ";
            }
            std::cout << std::endl << std::endl;
#endif

            Matrix &L = *cd->getLoad();
            Matrix &R = *cd->getRel();

            Matrix *partialReconstruction = matrix_mult_A_BT(L, R);

            delta = 0.0;

            for (auto mblock : missingBlocks)
            {
                for (uint64_t i = mblock.startingIndex; i < mblock.startingIndex + mblock.blockSize; ++i)
                {
                    Matrix &orig = *Src;
                    Matrix &recover = *partialReconstruction;

                    double diff = orig(i, mblock.column) - recover(i, mblock.column);
                    delta += fabs(diff);

                    orig(i, mblock.column) = recover(i, mblock.column) - (iter != 1 && iter < 15 ? diff * 0.0 : 0);
                }

                Vector v = Vector(mblock.blockSize, mblock.extractBlock(), true);
#ifdef RECOVERY_VERBOSE
                std::cout << "MB_" << iter << " =" << v.toString() << std::endl;
#endif
            }

            delta = delta / totalMBSize;

            delete partialReconstruction;

            //std::cout << "X_" << iter << " =" << std::endl << Src->toString() << std::endl;
#ifdef RECOVERY_VERBOSE
            std::cout << "delta_" << iter << " =" << delta << std::endl << std::endl;
#endif
        }

        //cm.deNormalizeMatrix();

        //std::cout << "X_fin =" << std::endl << Src->toString() << std::endl;

#ifdef RECOVERY_VERBOSE
        for (auto mblock : missingBlocks) {
            Vector v = Vector(mblock.blockSize, mblock.extractBlock(), true);

            std::cout << "MB_recovery" << " =" << v.toString() << std::endl;
        }
#endif

        std::cout << "total iter count: " << iter-1 << "; ";

        return iter-1;
    }

    void MissingValueRecovery::initialize()
    {
        Matrix &mx = *Src;

        // init missing blocks
        for (auto mblock : missingBlocks)
        {
            // linear interpolation
            double val1 = NAN, val2 = NAN;
            if (mblock.startingIndex > 0)
            {
                val1 = mx(mblock.startingIndex - 1, mblock.column);
            }
            if (mblock.startingIndex + mblock.blockSize < Src->dimensionN())
            {
                val2 = mx(mblock.startingIndex + mblock.blockSize, mblock.column);
            }

            double step;

            // fallback case - no 2nd value for interpolation
            if (std::isnan(val1) && std::isnan(val2))
            {
                // starting conditions violation
                abort();
            }
            else if (std::isnan(val1)) // start block is missing
            {
                val1 = val2;
                step = 0;
            }
            else if (std::isnan(val2)) // end block is missing
            {
                step = 0;
            }
            else
            {
                step = (val2 - val1) / (mblock.blockSize + 1);
            }

            for (uint64_t i = 0; i < mblock.blockSize; ++i)
            {
                (*Src)(mblock.startingIndex + i, mblock.column) = val1 + step * (i + 1);
            }
        }
    }

    void MissingValueRecovery::determineReduction()
    {
        std::cout << "determining reduction..." << std::endl << std::endl;

        // override
        // 0 = determined; 1+ = new
        uint64_t override = 0;

#ifdef determine_reduction_nonstat
        // step 1 - do full CD to determine rank

        std::vector<double> centroidValues = std::vector<double>();
        centroidValues.reserve(Src->dimensionM());
        setReduction(Src->dimensionM());
        cd->performDecomposition(false, &centroidValues, true);

        uint64_t rank = centroidValues.size();

        double squaresum = 0.0;

        std::cout << "CValues (rank=" << rank << "): ";
        for (auto a : centroidValues)
        {
            squaresum += a*a;
            std::cout << a << " ";
        }
        std::cout << std::endl;

        // step 2 [ALT] - entropy

        std::vector<double> relContribution = std::vector<double>();
        relContribution.reserve(rank);
        for (auto a : centroidValues)
        {
            relContribution.emplace_back(a*a / squaresum);
        }

        double entropy = 0.0;
        for (auto a : relContribution)
        {
            entropy += a * std::log(a);
        }
        entropy /= -std::log(rank);

        uint64_t red;
        double contributionSum = relContribution[0];
        for (red = 1; red < rank - 1; ++red)
        {
            if (contributionSum >= entropy) { break; }
            contributionSum += relContribution[red];
        }

        std::cout << "Auto-reduction [entropy] detected as: "
                  << red << " in [1..." << rank-1 << "], with  sum(contrib)=" << contributionSum
                  << " entropy=" << entropy << std::endl << std::endl;

        // cleanup - we will have less dimensions later
        cd->getLoad()->destroy();
        cd->getRel()->destroy();
#else

        Stats::CorrelationMatrix cm(Src);

            //cm.normalizeMatrix();

            Matrix *cormat = cm.getCorrelationMatrix();

            std::cout << "Corr(X) =" << std::endl << cormat->toString() << std::endl;

            Vector *sigma = cm.getSingularValuesOfCM();

            std::cout << "Sigma(Corr(X)) =" << sigma->toString() << std::endl;

            uint64_t rank = 0;
            double squaresum = 0.0;

            for (uint64_t i = 0; i < sigma->dimension(); ++i)
            {
                if ((*sigma)[i] < CentroidDecomposition::eps)
                {
                    rank = i;
                    break;
                }
                else
                {
                    squaresum += (*sigma)[i] * (*sigma)[i];
                }
            }
            if (rank == 0) rank = sigma->dimension();

            std::vector<double> relContribution = std::vector<double>();
            relContribution.reserve(rank);
            for (uint64_t i = 0; i < sigma->dimension(); ++i)
            {
                double a = (*sigma)[i];
                relContribution.emplace_back(a*a / squaresum);
            }

            double entropy = 0.0;
            for (auto a : relContribution)
            {
                entropy += a * std::log(a);
            }
            entropy /= -std::log(rank);

            uint64_t red;
            double contributionSum = relContribution[0];
            for (red = 1; red < rank - 1; ++red)
            {
                if (contributionSum >= entropy) { break; }
                contributionSum += relContribution[red];
            }

            std::cout << "Auto-reduction [entropy] detected as: "
                      << red << " in [1..." << rank-1 << "], with  sum(contrib)=" << contributionSum
                      << " entropy=" << entropy << std::endl << std::endl;
#endif

        //override above
        setReduction(override > 0 ? override : red);
    }


} // namespace LinearAlgebra_Algorithms
