//
// Created by zakhar on 20/01/19.
//

#include <cassert>
#include "Auxiliary.h"

namespace Algebra
{
namespace Operations
{

void insert_vector_at_column(arma::mat &matrix, uint64_t j, const arma::vec &vector)
{
    assert(matrix.n_rows == vector.n_elem);
    
    for (uint64_t i = 0; i < matrix.n_rows; ++i)
    {
        matrix.at(i, j) = vector[i];
    }
}

void insert_vector_at_row(arma::mat &matrix, uint64_t i, const arma::vec &vector)
{
    assert(matrix.n_cols == vector.n_elem);
    
    for (uint64_t j = 0; j < matrix.n_rows; ++j)
    {
        matrix.at(i, j) = vector[j];
    }
}

void increment_matrix(arma::mat &matrix, const std::vector<double> &vector)
{
    matrix.insert_rows(matrix.n_rows, arma::rowvec(vector));
}

void increment_matrix(arma::mat &matrix, const arma::vec &vector)
{
    matrix.insert_rows(matrix.n_rows, vector.t());
}

void increment_vector(arma::vec &vector, double val)
{
    vector.resize(vector.n_elem + 1);
    vector[vector.n_elem - 1] = val;
}

arma::mat std_to_arma(const std::vector<std::vector<double>> &matrix)
{
    arma::mat ret_mat(matrix.size(), matrix[0].size());
    
    for (uint64_t i = 0; i < matrix.size(); ++i)
    {
        for (uint64_t j = 0; j < matrix[i].size(); ++j)
        {
            ret_mat.at(i, j) = matrix[i][j];
        }
    }
    
    return ret_mat;
}

arma::vec std_to_arma(const std::vector<double> &vector)
{
    arma::vec ret_vec(vector.size());
    
    for (uint64_t i = 0; i < vector.size(); ++i)
    {
        ret_vec.at(i) = vector[i];
    }
    
    return ret_vec;
}

} // namespace Operations

namespace Algorithms
{

// defaults: horizontal_interp = false, inbound_missing = nullptr
void interpolate(arma::mat &X, bool horizontal_interp, std::vector<arma::uvec> *inbound_missing)
{
    std::vector<arma::uvec> local_missing;
    
    if (inbound_missing == nullptr)
    {
        if (horizontal_interp)
        {
            for (uint64_t i = 0; i < X.n_rows; ++i)
            {
                local_missing.emplace_back(arma::find_nonfinite(X.row(i)));
            }
        }
        else
        {
            for (uint64_t i = 0; i < X.n_cols; ++i)
            {
                local_missing.emplace_back(arma::find_nonfinite(X.col(i)));
            }
        }
    }
    
    std::vector<arma::uvec> &missing = (inbound_missing == nullptr) ? local_missing : *inbound_missing;
    
    if (horizontal_interp)
    {
        for (uint64_t i = 0; i < X.n_rows; ++i)
        {
            arma::uvec current = missing[i];
            auto iter = current.begin();
            
            for (;;)
            {
                // find block
                if (iter == current.end())
                {
                    break;// no more blocks
                }
                
                uint64_t block_start = *iter;
                uint64_t block_end = block_start;
                
                uint64_t last_index = block_start;
                ++iter;
                
                for (;; ++iter)
                {
                    if (iter == current.end() || *iter > last_index + 1)
                    {
                        block_end = last_index;
                        break; // iterator is now either at end() or at the start of the next block
                    }
                    last_index = *iter;
                }
                
                // block found
                
                double val1 = NAN, val2 = NAN;
                if (block_start > 0)
                {
                    val1 = X.at(i, block_start - 1);
                }
                if (block_end < X.n_cols)
                {
                    val2 = X.at(i, block_end);
                }
                
                double step;
                
                if (std::isnan(val1) && std::isnan(val2))
                {
                    val1 = 0.0;
                    step = 0;
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
                    step = (val2 - val1) / (double)(block_end - block_start + 2); // +1 to get block size, +1 to account for both ends
                }
                
                for (uint64_t j = 0; j < block_end - block_start + 1; ++j)
                {
                    X.at(i, block_start + j) = val1 + step * (double)(j + 1);
                }
            }
        }
    }
    else
    {
        for (uint64_t i = 0; i < X.n_cols; ++i)
        {
            arma::uvec current = missing[i];
            auto iter = current.begin();
        
            for (;;)
            {
                // find block
                if (iter == current.end())
                {
                    break;// no more blocks
                }
            
                uint64_t block_start = *iter;
                uint64_t block_end = block_start;
            
                uint64_t last_index = block_start;
                ++iter;
            
                for (;; ++iter)
                {
                    if (iter == current.end() || *iter > last_index + 1)
                    {
                        block_end = last_index;
                        break; // iterator is now either at end() or at the start of the next block
                    }
                    last_index = *iter;
                }
            
                // block found
            
                double val1 = NAN, val2 = NAN;
                if (block_start > 0)
                {
                    val1 = X.at(block_start - 1, i);
                }
                if (block_end < X.n_cols)
                {
                    val2 = X.at(block_end, i);
                }
            
                double step;
            
                if (std::isnan(val1) && std::isnan(val2))
                {
                    val1 = 0.0;
                    step = 0;
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
                    step = (val2 - val1) / (double)(block_end - block_start + 2); // +1 to get block size, +1 to account for both ends
                }
            
                for (uint64_t j = 0; j < block_end - block_start + 1; ++j)
                {
                    X.at(block_start + j, i) = val1 + step * (double)(j + 1);
                }
            }
        }
    }
}

} // namespace Algorithms
} // namespace Algebra
