//
// Created by Zakhar on 05.03.2017.
//

#include <cstdlib>
#include <cstring>
#include <cmath>

#include <iomanip>
#include <sstream>

#include "Vector.h"

namespace LinearAlgebra_Basic {

//
// Vector data
//

double &Vector::operator[](uint64_t i)
{
    return dataContainer[i];
}

double &Vector::operator[](uint64_t i) const
{
    return dataContainer[i];
}

uint64_t Vector::dimension() const { return dim; }

Vector *Vector::copy()
{
    return new Vector(dim, dataContainer, true);
}

std::string Vector::toString()
{
    std::stringstream strbuffer;

    strbuffer << std::fixed << std::setprecision(2) << "(";
    for (uint64_t i = 0; i < dim - 1; i++)
    {
        strbuffer << operator[](i) << "; \t";
    }
    strbuffer << operator[](dim-1) << ")^T" << std::endl;

    return strbuffer.str();
}

void Vector::expandStorage()
{
    uint64_t newDim = dim + 1;

    if (newDim > realCapacity)
    {
        // reallocate memory
        realCapacity = static_cast<size_t>(std::round(realCapacity * 1.4));
        size_t size = realCapacity * sizeof(*dataContainer);
        void *newData = std::realloc(dataContainer, size);
        dataContainer = static_cast<double *>(newData);
    }

    dim = newDim;
}

//
// Vector constructors
//

Vector::Vector(uint64_t n, bool init)
{
    dim = n;
    realCapacity = n;

    size_t size = dim * sizeof(*dataContainer);
    void *data = std::malloc(size);

    if (init)
    {
        std::memset(data, 0, size);
    }

    dataContainer = static_cast<double *>(data);
}

Vector::Vector(uint64_t n, double *arr, bool copy)
{
    dim = n;
    realCapacity = n;

    if (copy)
    {
        size_t size = dim * sizeof(*dataContainer);
        void *data = std::malloc(size);
        std::memcpy(data, arr, size);

        dataContainer = static_cast<double *>(data);
    }
    else
    {
        referenced = true;

        dataContainer = arr;
    }
}

Vector::Vector(const std::vector<double> &data)
{
    dim = data.size();
    realCapacity = dim;

    dataContainer = static_cast<double *>(std::malloc(dim * sizeof(*dataContainer)));

    for (uint64_t i = 0; i < dim; i++)
    {
        dataContainer[i] = data[i];
    }
}

Vector::~Vector()
{
    if (!referenced)
    {
        std::free(dataContainer);
    }
}

//
// Pre-defined vectors
//

Vector *Vector::canonical(uint64_t dim, uint64_t pos)
{
    Vector* vec = new Vector(dim, true);

    (*vec)[pos] = 1.0;

    return vec;
}

//
// Vector operations
//

double Vector::norm2()
{
    return sqrt(vector_dot(*this, *this));
}

Vector &Vector::normalize()
{
    return (*this) / norm2();// todo: wtf is that????
}

Vector &Vector::append(double value)
{
    expandStorage();

    (*this)[dim - 1] = value;

    return *this;
}

//
// Vector operators
//

Vector *operator+(const Vector &vecA, const Vector &vecB)
{
    Vector *newVec = new Vector(vecA.dimension(), false);

    for (uint64_t i = 0; i < vecA.dimension(); ++i)
    {
        (*newVec)[i] = vecA[i] + vecB[i];
    }

    return newVec;
}

Vector *operator-(const Vector &vecA, const Vector &vecB)
{
    Vector *newVec = new Vector(vecA.dimension(), false);

    for (uint64_t i = 0; i < vecA.dimension(); ++i)
    {
        (*newVec)[i] = vecA[i] - vecB[i];
    }

    return newVec;
}

Vector &operator*(Vector &vec, const double scalar)
{
    for (uint64_t i = 0; i < vec.dimension(); ++i)
    {
        vec[i] *= scalar;
    }
    return vec;
}

Vector &operator/ (Vector& vec, const double scalar)
{
    for (uint64_t i = 0; i < vec.dimension(); ++i)
    {
        vec[i] /= scalar;
    }
    return vec;
}

Vector &operator*(const double scalar, Vector &vec)
{
    return vec * scalar;
}

double vector_dot(const Vector &vec1, const Vector &vec2)
{
    double dot = 0.0;

    for (uint64_t i = 0; i < vec1.dimension(); ++i)
    {
        dot += vec1[i] * vec2[i];
    }

    return dot;
}

} // namespace LinearAlgebra_Basic