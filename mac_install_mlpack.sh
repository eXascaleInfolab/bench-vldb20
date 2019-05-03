#!/bin/bash
# Script taken & modified from https://gist.github.com/darcyliu/1835d861f78b2893ac1134280ab78c5f
git clone https://github.com/mlpack/mlpack mlpack
mkdir mlpack/build
cd mlpack/build
cmake -D CMAKE_CXX_COMPILER=/usr/local/opt/llvm/clang++ -D CMAKE_C_COMPILER=/usr/local/opt/llvm/clang -D USE_OPENMP=ON -D BUILD_PYTHON_BINDINGS=OFF -D BUILD_TESTS=OFF -D BUILD_CLI_EXECUTABLES=OFF -D CMAKE_CXX_FLAGS= -I =/usr/local/opt/llvm/include -L =/usr/local/opt/llvm/lib ..
make
make install
cd ../..
rm -rf mlpack