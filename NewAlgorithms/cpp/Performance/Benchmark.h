//
// Created by Zakhar on 16/03/2017.
//

#pragma once

#include <mlpack/core.hpp>

#include "../MathIO/MatrixReadWrite.h"
#include "../Algebra/MissingBlock.hpp"

namespace Performance
{

int64_t
Recovery(arma::mat &mat, uint64_t truncation,
         const std::string &algorithm, const std::string &xtra);


} // namespace Performance
