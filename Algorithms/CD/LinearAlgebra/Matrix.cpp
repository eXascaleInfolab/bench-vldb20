//
// Created by Zakhar on 03.03.2017.
//

#include <cstdlib>
#include <cstring>
#include <cmath>

#include <vector>
#include <iomanip>
#include <sstream>

#include "Matrix.h"

namespace LinearAlgebra_Basic {

//
// Matrix data
//

double& Matrix::operator() (uint64_t i, uint64_t j)
{
    return dataContainer[dimM * i + j];
}

double& Matrix::operator() (uint64_t i, uint64_t j) const
{
    return dataContainer[dimM * i + j];
}

uint64_t Matrix::dimensionN() const { return dimN; }
uint64_t Matrix::dimensionM() const { return dimM; }

Matrix *Matrix::copy()
{
    size_t size = dimN * dimM * sizeof(*dataContainer);
    void *newData = std::malloc(size);
    std::memcpy(newData, dataContainer, size);

    return new Matrix(dimN, dimM, static_cast<double *>(newData));
}

std::string Matrix::toString()
{
    std::stringstream strbuffer;

    strbuffer << std::fixed << std::setprecision(2);
    for (uint64_t i = 0; i < dimN; i++)
    {
        for (uint64_t j = 0; j < dimM; j++)
        {
            strbuffer << operator()(i, j) << "; \t";
        }
        strbuffer << std::endl;
    }

    return strbuffer.str();
}

void Matrix::expandStorage()
{
    uint64_t newDim = dimN + 1;

    if (newDim > realCapacity)
    {
        // reallocate memory
        realCapacity = static_cast<size_t>(std::round(realCapacity * 1.4));
        size_t size = realCapacity * dimM * sizeof(*dataContainer);
        void *newData = std::realloc(dataContainer, size);
        dataContainer = static_cast<double *>(newData);
    }

    dimN = newDim;
}

//
// Matrix constructors & destructors
//

Matrix::Matrix(uint64_t n, uint64_t m, double *data)
{
    dimN = n;
    dimM = m;
    realCapacity = n;
    dataContainer = data;
}

Matrix::Matrix(uint64_t n, uint64_t m, bool init)
{
    dimN = n;
    dimM = m;
    realCapacity = n;
    size_t size = dimN * dimM * sizeof(*dataContainer);
    void *data = std::malloc(size);
    if (init)
    {
        std::memset(data, 0, size);
    }

    dataContainer = static_cast<double *>(data);
}

Matrix::Matrix(const std::vector<std::vector<double>> &data)
{
    dimN = data.size();
    dimM = data[0].size();
    realCapacity = dimN;
    dataContainer = static_cast<double *>(std::malloc(dimN * dimM * sizeof(*dataContainer)));

    for (uint64_t i = 0; i < dimN; ++i)
    {
        for (uint64_t j = 0; j < dimM; ++j)
        {
            dataContainer[dimM * i + j] = data[i][j];
        }
    }
}

Matrix::~Matrix()
{
    std::free(dataContainer);
}

//
// Pre-defined matrices
//
Matrix *Matrix::identity(uint64_t dim)
{
    Matrix *mx = new Matrix(dim, dim, true);

    for (uint64_t i = 0; i < dim; ++i)
    {
        (*mx)(i, i) = 1.0;
    }

    return mx;
}

//
// Matrix operations
//

void Matrix::destroy()
{
    size_t size = dimN * dimM * sizeof(*dataContainer);
    std::memset(dataContainer, 0, size);
}

double Matrix::normF()
{
    double norm = 0.0;

    for (uint64_t i = 0; i < dimN; ++i)
    {
        for (uint64_t j = 0; j < dimM; ++j)
        {
            norm += operator()(i, j) * operator()(i, j);
        }
    }

    return sqrt(norm);
}

Vector *Matrix::extractRowVector(uint64_t i, bool copy)
{
    Vector *newVector = new Vector(dimM, (dataContainer + i * dimM), copy);

    return newVector;
}

Vector *Matrix::extractColumnVector(uint64_t j)
{
    Vector *newVector = new Vector(dimN, false);

    for (uint64_t i = 0; i < dimN; ++i)
    {
        (*newVector)[i] = operator()(i, j);
    }

    return newVector;
}

Matrix &Matrix::insertVectorAtRow(uint64_t i, Vector* vec)
{
    std::memcpy(dataContainer + i * dimM, vec->dataContainer, (vec->dim) * sizeof(*dataContainer));

    return *this;
}

Matrix &Matrix::insertVectorAtColumn(uint64_t j, Vector* vec)
{
    for (uint64_t i = 0; i < vec->dim; ++i)
    {
        operator()(i, j) = (*vec)[i];
    }

    return *this;
}

Matrix &Matrix::append(std::vector<double> &newData)
{
    expandStorage();

    for (uint64_t i = 0; i < dimM; ++i)
    {
        (*this)(dimN - 1, i) = newData[i];
    }

    return *this;
}

Matrix &Matrix::append(Vector *newData)
{
    expandStorage();

    (void)insertVectorAtRow(dimN - 1, newData);

    return *this;
}

Matrix &Matrix::operator+=(const Matrix &mxB)
{
    for (uint64_t i = 0; i < dimN; ++i)
    {
        for (uint64_t j = 0; j < dimM; ++j)
        {
            (*this)(i, j) += mxB(i, j);
        }
    }

    return *this;
}

Matrix &Matrix::operator-=(const Matrix &mxB)
{
    for (uint64_t i = 0; i < dimN; ++i)
    {
        for (uint64_t j = 0; j < dimM; ++j)
        {
            (*this)(i, j) -= mxB(i, j);
        }
    }

    return *this;
}

//
// Matrix binary operations
//

Matrix *operator+(const Matrix &mxA, const Matrix &mxB)
{
    Matrix *newMx = new Matrix(mxA.dimensionN(), mxA.dimensionM(), false);

    for (uint64_t i = 0; i < mxA.dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < mxA.dimensionM(); ++j)
        {
            (*newMx)(i, j) =
                    mxA(i, j) + mxB(i, j);
        }
    }

    return newMx;
}

Matrix *operator-(const Matrix &mxA, const Matrix &mxB)
{
    Matrix *newMx = new Matrix(mxA.dimensionN(), mxA.dimensionM(), false);

    for (uint64_t i = 0; i < mxA.dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < mxA.dimensionM(); ++j)
        {
            (*newMx)(i, j) =
                    mxA(i, j) - mxB(i, j);
        }
    }

    return newMx;
}

Matrix &operator*(const double scalar, Matrix &mx)
{
    return mx * scalar;
}

Matrix &operator*(Matrix &mx, const double scalar)
{
    for (uint64_t i = 0; i < mx.dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < mx.dimensionM(); ++j)
        {
            mx(i, j) *= scalar;
        }
    }

    return mx;
}

Matrix *operator*(const Matrix &mxA, const Matrix &mxB)
{
    Matrix *newMx = new Matrix(mxA.dimensionN(), mxB.dimensionM(), false);
    uint64_t sharedDim = mxA.dimensionM();
    double temp;

    for (uint64_t i = 0; i < newMx->dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < newMx->dimensionM(); ++j)
        {
            temp = 0.0;

            for (uint64_t k = 0; k < sharedDim; ++k)
            {
                temp += mxA(i, k) * mxB(k, j);
            }

            (*newMx)(i, j) = temp;
        }
    }

    return newMx;
}

Matrix *matrix_mult_AT_B(const Matrix &mxA, const Matrix &mxB)
{
    Matrix *newMx = new Matrix(mxA.dimensionM(), mxB.dimensionM(), false);
    uint64_t sharedDim = mxA.dimensionN();
    double temp;

    for (uint64_t i = 0; i < newMx->dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < newMx->dimensionM(); ++j)
        {
            temp = 0.0;

            for (uint64_t k = 0; k < sharedDim; ++k)
            {
                temp += mxA(k, i) * mxB(k, j);
            }

            (*newMx)(i, j) = temp;
        }
    }

    return newMx;
}

Matrix *matrix_mult_A_BT(const Matrix &mxA, const Matrix &mxB)
{
    Matrix *newMx = new Matrix(mxA.dimensionN(), mxB.dimensionN(), false);
    uint64_t sharedDim = mxA.dimensionM();
    double temp;

    for (uint64_t i = 0; i < newMx->dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < newMx->dimensionM(); ++j)
        {
            temp = 0.0;

            for (uint64_t k = 0; k < sharedDim; ++k)
            {
                temp += mxA(i, k) * mxB(j, k);
            }

            (*newMx)(i, j) = temp;
        }
    }

    return newMx;
}

//
// Matrix x Vector operations
//

Vector *operator*(const Matrix &mx, const Vector &vec)
{
    // M * v
    Vector *res = new Vector(mx.dimensionN(), false);

    for (uint64_t i = 0; i < mx.dimensionN(); ++i)
    {
        (*res)[i] = mx(i, 0) * vec[0];
        for (uint64_t j = 1; j < mx.dimensionM(); ++j)
        {
            (*res)[i] += mx(i, j) * vec[j];
        }
    }

    return res;
}

Vector *operator^(const Matrix &mx_t, const Vector &vec)
{
    // M^T * v
    Vector *res = new Vector(mx_t.dimensionM(), true);

    // maintain row-first iteration order for matrix
    for (uint64_t i = 0; i < mx_t.dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < mx_t.dimensionM(); ++j)
        {
            (*res)[j] += mx_t(i, j) * vec[i];
        }
    }

    return res;
}

Matrix *vector_outer(const Vector &vecA, const Vector &vecB)
{
    //outer product, aka a * b^T
    Matrix *mx = new Matrix(vecA.dimension(), vecB.dimension(), false);

    for (uint64_t i = 0; i < vecA.dimension(); ++i)
    {
        for (uint64_t j = 0; j < vecB.dimension(); ++j)
        {
            (*mx)(i, j) = vecA[i] * vecB[j];
        }
    }

    return mx;
}

} // namespace LinearAlgebra_Basic