#include <iostream>
#include "ZeroImpute.h"

namespace Algorithms
{

  void ZeroImpute::ZeroImpute_Recovery(arma::mat &input)
{
       for (uint64_t j = 0; j < input.n_cols; ++j)
    {
        for (uint64_t i = 0; i < input.n_rows; ++i)
        {
            if (!arma::is_finite(input(i, j)))
            {
                input(i, j) = 0.0;
            }
        }
    } 
}    

// any other functions go here

} // namespace Algorithms
