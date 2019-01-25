//
// Created by zakhar on 20/01/19.
//

#pragma once

#include <armadillo>

namespace Algebra
{

namespace Operations
{

void insert_vector_at_column(arma::mat &matrix, uint64_t column, const arma::vec &vector);

void insert_vector_at_row(arma::mat &matrix, uint64_t column, const arma::vec &vector);

void increment_matrix(arma::mat &matrix, const std::vector<double> &vector);

void increment_matrix(arma::mat &matrix, const arma::vec &vector);

void increment_vector(arma::vec &vector, double val);

arma::mat std_to_arma(const std::vector<std::vector<double>> &matrix);

arma::vec std_to_arma(const std::vector<double> &vector);

} // namespace Operations

namespace Algorithms
{

void interpolate(arma::mat &X, bool horizontal_interp = false, std::vector<arma::uvec> *inbound_missing = nullptr);


} // namespace Algorithms
} // namespace Algebra
