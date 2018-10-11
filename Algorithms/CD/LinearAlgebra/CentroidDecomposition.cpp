//
// Created by Zakhar on 08.03.2017.
//

#include <cmath>
#include <iostream>
#include "CentroidDecomposition.h"

using namespace LinearAlgebra_Basic;

namespace LinearAlgebra_Algorithms {
//
// CentroidDecomposition constructors & desctructors
//

CentroidDecomposition::CentroidDecomposition(LinearAlgebra_Basic::Matrix *mx)
{
    Load = new Matrix(mx->dimensionN(), mx->dimensionM(), true);
    Rel =  new Matrix(mx->dimensionM(), mx->dimensionM(), true);

    signVector = std::vector<Vector *>();

    // i = 0:
    Vector *signV = new Vector(mx->dimensionN(), false);
    for (uint64_t j = 0; j < signV->dimension(); j++)
    {
        (*signV)[j] = 1.0;
    }
    signVector.push_back(signV);
    signVectorSteps.emplace_back(std::vector<Vector *>());

    // i in [1,m[
    for (uint64_t i = 1; i < mx->dimensionM(); ++i)
    {
        signVector.push_back(signV->copy());
        signVectorSteps.emplace_back(std::vector<Vector *>());
    }

    Src = mx;
    truncation = Src->dimensionM();
}

CentroidDecomposition::CentroidDecomposition(LinearAlgebra_Basic::Matrix *mx, uint64_t k) : CentroidDecomposition(mx)
{
    this->truncation = k;
}

CentroidDecomposition::~CentroidDecomposition()
{
    delete Load;
    delete Rel;

    for (Vector *sv : signVector)
    {
        delete sv;
    }
}

//
// CentroidDecomposition API
//

Matrix *CentroidDecomposition::getLoad()
{
    return Load;
}

Matrix *CentroidDecomposition::getRel()
{
    return Rel;
}

void CentroidDecomposition::performDecomposition(bool incremental, std::vector<double> *centroidValues, bool stopOnIncompleteRank /*= false*/, bool skipSSV)
{
    Matrix *Li_RiT; // container for L_i * R_i^T

    Matrix *X = Src->copy();

    Vector *Z;
    Vector *Rel_i;
    Vector *Load_i;

    if (!incremental && !skipSSV)
    {
        for (uint64_t i = 0; i < Src->dimensionM(); ++i)
        {
            for (uint64_t j = 0; j < Src->dimensionN(); j++)
            {
                (*signVector[i])[j] = 1.0;
            }
        }
    }

    for (uint64_t i = 0; i < truncation; i++)
    {
        Z = skipSSV
            ? signVector[i]
            //: findIncrementalSSV(X, i);
            : findIncrementalSSVPlus(X, i);

        // C_*i = X^T * Z
        Rel_i = (*X) ^ (*Z);

        // R_*i = C_*i / ||C_*i||
        if (centroidValues != nullptr) {
            double centroid = Rel_i->norm2();

            if (stopOnIncompleteRank && centroid < eps) // incomplete rank, don't even proceed with current iteration
            {
                delete Rel_i; // others are not yet allocated
                break;
            }

            centroidValues->emplace_back(centroid);
        }

        (void) Rel_i->normalize();//todo: avoid duplicate norm2() calculation, see Vector.cpp

        // R = Append(R, R_*i)
        (void) Rel->insertVectorAtColumn(i, Rel_i);

        // L_*i = X * R
        Load_i = (*X) * (*Rel_i);

        // L = Append(L, L_*i)
        (void) Load->insertVectorAtColumn(i, Load_i);

        // X := X - L_*i * R_*i^T
        Li_RiT = vector_outer(*Load_i, *Rel_i);
        *X -= *Li_RiT;

        delete Li_RiT;

        // Z is not to be deleted, it's a pointer from std::vector<Vector *> signVector;
        delete Rel_i;
        delete Load_i;
    }

    delete X;

    if (incremental) { addedRows = 0; }
    decomposed = true;
}

//
// CentroidDecomposition algorithm
//

void CentroidDecomposition::increment(Vector *vec)
{
    Src->append(vec);
    Load->append(vec); // doesn't matter, will be overwritten
    ++addedRows;

    for (uint64_t i = 0; i < Src->dimensionN(); ++i)
    {
        signVector[i]->append(1.0);
    }
}

void CentroidDecomposition::increment(std::vector<double> &vec)
{
    Src->append(vec);
    Load->append(vec); // ditto as ^
    ++addedRows;

    for (uint64_t i = 0; i < Src->dimensionM(); ++i)
    {
        signVector[i]->append(1.0);
    }
}

//
// Algorithm
//

LinearAlgebra_Basic::Vector *CentroidDecomposition::findIncrementalSSVPlus(LinearAlgebra_Basic::Matrix *mx, uint64_t k)
{
    // Scalable Sign Vector
    int64_t pos = -1;
    double val = 0.0;

    Vector &Z = *signVector[k]; // get a reference

#define ssv_reset_
#ifdef ssv_reset
    for (uint64_t j = 0; j < Z.dimension(); j++)
{
    Z[j] = 1.0;
}
#endif

    Vector *S = new Vector(Src->dimensionM(), false);
    Vector *V = new Vector(Src->dimensionN(), false);

    // pre-process rows of X
    std::vector<Vector *> x_ = std::vector<Vector *>();
    std::vector<double> XXT_ = std::vector<double>();

    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        x_.push_back(mx->extractRowVector(i, false));
        XXT_.push_back(vector_dot(*x_[i], *x_[i]));
    }


    // ITERATION #1

    // S = Sum(1:n) { z_i * (X_i* ^ T) }

    // i = 0
    {
        for (uint64_t j = 0; j < S->dimension(); ++j)
        {
            (*S)[j] = (*x_[0])[j] * Z[0];
        }
    }

    for (uint64_t i = 1; i < mx->dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < S->dimension(); ++j)
        {
            (*S)[j] += (*x_[i])[j] * Z[i];
        }
    }



    // v_i = z_i * (z_i * X_i* * S - X_i* * (X_i*)^T)
    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        (*V)[i] = Z[i] * (
                Z[i] * vector_dot(*x_[i], *S) - XXT_[i]
        );
    }

    // Search next element
    val = 0.0; pos = -1;

    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        if (Z[i] * (*V)[i] < 0)
        {
            if (fabs((*V)[i]) > val)
            {
                val = fabs((*V)[i]);
                pos = i;
            }
        }
    }

    // ITERATIONS 2+

    // main loop
    while (pos != -1)
    {
        // Search next element to flip
        val = eps; pos = -1;

        for (uint64_t i = 0; i < mx->dimensionN(); ++i)
        {
            if (Z[i] * (*V)[i] < 0)
            {
                if (fabs((*V)[i]) > val)
                {
                    val = fabs((*V)[i]);
                    pos = i;

                    // change sign
                    Z[pos] = Z[pos] * (-1.0);

                    double factor = Z[pos] + Z[pos];

                    // Determine V_k+1 from V_k

                    for (uint64_t l = 0; l < mx->dimensionN(); ++l)
                    {
                        (*V)[l] = (*V)[l] + factor * (l == static_cast<uint64_t>(pos) ? 0 : vector_dot(*x_[l], *x_[pos]) );
                    }
                }
            }
        }
        ++ssvIterations;
    }

    // both are reused inside the loop and never deconstructed in the process
    delete S;
    delete V;

    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        delete x_.at(i);
    }


    return signVector[k];
}

LinearAlgebra_Basic::Vector *CentroidDecomposition::findIncrementalSSV(LinearAlgebra_Basic::Matrix *mx, uint64_t k)
{
    // Scalable Sign Vector
    int64_t pos = -1;
    double val = 0.0;

    Vector &Z = *signVector[k]; // get a reference

#define ssv_reset_
#ifdef ssv_reset
    for (uint64_t j = 0; j < Z.dimension(); j++)
    {
        Z[j] = 1.0;
    }
#endif

    Vector *S = new Vector(Src->dimensionM(), false);
    Vector *V = new Vector(Src->dimensionN(), false);

    // pre-process rows of X
    std::vector<Vector *> x_ = std::vector<Vector *>();
    std::vector<double> XXT_ = std::vector<double>();

    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        x_.push_back(mx->extractRowVector(i, false));
        XXT_.push_back(vector_dot(*x_[i], *x_[i]));
    }


    // ITERATION #1

    // S = Sum(1:n) { z_i * (X_i* ^ T) }

    // i = 0
    {
        for (uint64_t j = 0; j < S->dimension(); ++j)
        {
            (*S)[j] = (*x_[0])[j] * Z[0];
        }
    }

    for (uint64_t i = 1; i < mx->dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < S->dimension(); ++j)
        {
            (*S)[j] += (*x_[i])[j] * Z[i];
        }
    }



    // v_i = z_i * (z_i * X_i* * S - X_i* * (X_i*)^T)
    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        (*V)[i] = Z[i] * (
                Z[i] * vector_dot(*x_[i], *S) - XXT_[i]
        );
    }

    // Search next element
    val = 0.0; pos = -1;

    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        if (Z[i] * (*V)[i] < 0)
        {
            if (fabs((*V)[i]) > val)
            {
                val = fabs((*V)[i]);
                pos = i;
            }
        }
    }

    // ITERATIONS 2+

    // main loop
    while (pos != -1)
    {
        if (decomposed && false)
        {
            signVectorSteps[k].push_back(Z.copy());
        }

        // change sign
        Z[pos] = Z[pos] * (-1.0);

        // Determine V_k+1 from V_k

        for (uint64_t i = 0; i < mx->dimensionN(); ++i)
        {
            (*V)[i] -= 2 * (i == static_cast<uint64_t>(pos) ? 0 : vector_dot(*x_[i], *x_[pos]) );
        }

        // Search next element to flip
        val = 0.0; pos = -1;

        for (uint64_t i = 0; i < mx->dimensionN(); ++i)
        {
            if (Z[i] * (*V)[i] < 0)
            {
                if (fabs((*V)[i]) > val)
                {
                    val = fabs((*V)[i]);
                    pos = i;
                }
            }
        }
        ++ssvIterations;
    }

    if (decomposed)
    {
        signVectorSteps[k].push_back(Z.copy());
    }

    // both are reused inside the loop and never deconstructed in the process
    delete S;
    delete V;

    for (uint64_t i = 0; i < mx->dimensionN(); ++i)
    {
        delete x_.at(i);
    }


    return signVector[k];
}

} // namespace LinearAlgebra_Algorithms
