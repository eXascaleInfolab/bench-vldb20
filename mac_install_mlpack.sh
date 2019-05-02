#!/bin/bash
# Script taken & modified from https://gist.github.com/darcyliu/1835d861f78b2893ac1134280ab78c5f
git clone https://github.com/mlpack/mlpack mlpack
mkdir mlpack/build
cd mlpack/build
cmake ..
make
make install
cd ../..
rm -rf mlpack