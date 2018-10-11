//
// Created by Zakhar on 07.03.2017.
//

#include "Vector.h"
#include <vector>
#include <string>

#ifndef CENTROIDDECOMPOSITION_MATRIX_H
#define CENTROIDDECOMPOSITION_MATRIX_H


namespace LinearAlgebra_Basic {

class Matrix {
    //
    // Data
    //
  private:
    uint64_t realCapacity;

  protected:
    double *dataContainer;
    uint64_t dimN;
    uint64_t dimM;

  public:
    double& operator() (uint64_t i, uint64_t j);
    double& operator() (uint64_t i, uint64_t j) const;

    uint64_t dimensionN() const;
    uint64_t dimensionM() const;

    Matrix *copy();
    std::string toString();

  private:
    void expandStorage();

    //
    // Constructors & destructors
    //
  public:
    Matrix(uint64_t n, uint64_t m, double *data);
    Matrix(uint64_t n, uint64_t m, bool init);
    Matrix(const std::vector<std::vector<double>> &data);

    ~Matrix();

    //
    // Pre-defined
    //
  public:
    static Matrix* identity(uint64_t dim);

    //
    // Unary operations
    //
  public:
    void destroy();
    double normF();
    Vector *extractRowVector(uint64_t i, bool copy);
    Vector *extractColumnVector(uint64_t j);

    Matrix &insertVectorAtRow(uint64_t i, Vector *vec);
    Matrix &insertVectorAtColumn(uint64_t j, Vector *vec);

    Matrix &append(std::vector<double> &newData);
    Matrix &append(Vector *newData);

    Matrix &operator+=(const Matrix &mxB);
    Matrix &operator-=(const Matrix &mxB);
}; // end class

//
// Binary operations
//
Matrix *operator+(const Matrix& mxA, const Matrix& mxB);
Matrix *operator-(const Matrix& mxA, const Matrix& mxB);

Matrix &operator*(Matrix& mx, const double scalar);
Matrix &operator*(const double scalar, Matrix& mx);

Matrix *operator*(const Matrix& mxA, const Matrix& mxB);
Matrix *matrix_mult_AT_B(const Matrix &mxA, const Matrix &mxB);
Matrix *matrix_mult_A_BT(const Matrix &mxA, const Matrix &mxB);

//
// Inter-type operations
//
Vector *operator* (const Matrix& mx, const Vector& vec);
Vector *operator^ (const Matrix& mx_t, const Vector& vec);

Matrix *vector_outer(const Vector &vecA, const Vector &vecB);

}// namespace LinearAlgebra_Basic

#endif //CENTROIDDECOMPOSITION_MATRIX_H
