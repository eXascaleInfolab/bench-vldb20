//
// Created by Zakhar on 07.03.2017.
//

#include <vector>
#include <string>

#ifndef CENTROIDDECOMPOSITION_VECTOR_H
#define CENTROIDDECOMPOSITION_VECTOR_H

namespace LinearAlgebra_Basic {

class Vector;

class Vector {
    friend class Matrix;
    //
    // Data
    //
  private:
    uint64_t realCapacity;
    bool referenced = false;

  protected:
    double *dataContainer;
    uint64_t dim;

  public:
    uint64_t dimension() const;
    double& operator[] (uint64_t i);
    double& operator[] (uint64_t i) const;

    Vector *copy();
    std::string toString();

  private:
    void expandStorage();

    //
    // Constructors and destructors
    //
  public:
    /// Default constructor - creates a vector of n 0.0 values
    /// @param [in] n vector size
    /// @param [in] init if constructor is to zerofill the vector
    Vector(uint64_t n, bool init);

    /// Construct a vector from existing data
    /// @param [in] n vector size
    /// @param [in] arr data to construct a vector from
    /// @param [in] copy to copy the data or to keep the provided pointer
    Vector(uint64_t n, double *arr, bool copy);

    /// Construct a vector from std::vector
    /// @param [in] data std::vector of data to construct a vector from
    explicit Vector(const std::vector<double> &data);

    /// Destroys the vector.
    /// Frees the memory of data container if it wasn't a reference
    ~Vector();

    //
    // Pre-defined vectors
    //
  public:
    static Vector *canonical(uint64_t dim, uint64_t pos);

    //
    // Vector operations
    //
  public:
    double norm2();
    Vector &normalize();

    Vector &append(double value);

}; // end class

//
// Operators
//

Vector *operator+(const Vector& vecA, const Vector& vecB);
Vector *operator-(const Vector &vecA, const Vector &vecB);

Vector &operator* (Vector& vec, const double scalar);
Vector &operator* (const double scalar, Vector& vec);

Vector &operator/ (Vector& vec, const double scalar);

double vector_dot(const Vector &vec1, const Vector &vec2);

} // namespace LinearAlgebra_Basic

#endif //CENTROIDDECOMPOSITION_VECTOR_H
