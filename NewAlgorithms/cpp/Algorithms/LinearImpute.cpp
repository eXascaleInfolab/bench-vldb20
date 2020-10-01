#include <iostream>
#include "LinearImpute.h"

namespace Algorithms
{

void LinearImpute::LinearImpute_Recovery(arma::mat &input)
{
    std::vector<arma::uvec> missing;

    for (uint64_t i = 0; i < input.n_cols; ++i)
    {
        missing.emplace_back(arma::find_nonfinite(input.col(i)));
    }

    for (uint64_t i = 0; i < input.n_cols; ++i)
    {
        arma::uvec &current = missing[i];
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
                val1 = input.at(block_start - 1, i);
            }
            if (block_end < input.n_rows)
            {
                val2 = input.at(block_end + 1, i);
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
                input.at(block_start + j, i) = val1 + step * (double)(j + 1);
            }
        }
    }
}

// any other functions go here

} // namespace Algorithms