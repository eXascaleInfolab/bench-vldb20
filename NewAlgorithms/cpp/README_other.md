# Adding an algorithm in C++ (using armadillo library)

This tutorial shows how to add a new imputation algorithm to the benchmark. We will illustrate the process by implementing ZeroImpute. If you want to include your own algorithm, then you need to add your own .cpp and .h files when indicated, follow the same steps, and rebuild the program at the end.

___


## Prerequisites

- The benchmark needs to be executed once (see [execution section](https://github.com/eXascaleInfolab/bench-vldb20)). 
- Extra dependencies: any, provided they are compatible with C++14 and do not conflict with Armadillo, MLPACK, openBLAS, LAPACK, ARPACK.
- Algorithm input: take matrix where the missing values are designated as NaN. Algorithm output: missing values are imputed and the matrix is returned.

- Follow the guide for [armadillo algorithm](https://github.com/eXascaleInfolab/bench-vldb20/tree/master/NewAlgorithms/cpp) first, most of the process is the same, this guide will exaplain what has to be changed.

## 1. Code Integration 

- Choose a *long name* and a *short name* for your algorithm. We will use `ZeroImpute` and `zeroimp`, respectively.

- `cd NewAlgorithms/cpp/`

- In this example we use C++ STL with `vector<vector<double>>`, but the process is the same for any other library (e.g. Eigen3 with `Eigen::Matrix<double>`). It's also possible to use C pointers, but it has to be able to be compiled with `g++` or `clang++` (you can replace the compiler in the Makefile).

- Modify the .h and .cpp files inside folder `Algorithms`. We will show this on an example of ZeroImpute.h and ZeroImpute.cpp from the armadillo guide.
    - Open `ZeroImpute.h` and add all the headers your implementation needs. For C++ STL add the standard lilbrary header `#include <vector>`. Then replace the function `ZeroImpute_Recovery` argument type from `arma::mat &` to `std::vector<std::vector<double>> &`.
    - Open `ZeroImpute.cpp` and update the function signature `ZeroImpute::ZeroImpute_Recovery()` to match the header file. Replace the function body to treat input as STL type instead of armadillo:
        ```C++
        for (uint64_t i = 0; i < input.size(); ++i)
        {
            for (uint64_t j = 0; j < input[i].size(); ++j)
            {
                if (!std::isnan(input[i][j])))
                {
                    input[i][j] = 0.0;
                }
            }
        }
        ```

- Call the new version of the algorithm with the input given by the testing framework
    - Open `Performance/Benchmark.cpp`
    - In the function `Recovery_ZeroImpute` (that you should have already created) add the conversion from armadillo matrix to the type that your algorithm uses.
    - Before `begin = ...` add the following code that creates a two-dimensional `std::vector`, then add a loop that copies the contents of the armadillo matrix into the new instance:
        ```C++
        std::vector<std::vector<double>> mat_stl(mat.n_rows);
        
        for (uint64_t i = 0; i < mat.n_rows; ++i)
        {
            mat_stl.emplace_back(std::vector<double>(mat.n_cols));
            for (uint64_t j = 0; j < mat.n_cols; ++j)
            {
                mat_stl[i].emplace_back(mat(i, j));
            }
        }
    - Remark: be sure to never re-allocate the memory during copying procedure (with auto-resizable lists or similar), as this can lead to a significant slowdown during testing routines on larger datasets. `mat.n_rows` and `mat.n_cols` are matrix dimensions, which is enough to be able to know in advance all the data type sizes to pre-allocate them.
    - Update the call to `Recovery_ZeroImpute` to use `mat_stl` variable instead of `mat`.
    - Afterwards, add the code that will fill the values from the other type back into the armadillo matrix after the `std::cout << ...` statement
        ```C++
        for (uint64_t i = 0; i < mat.n_rows; ++i)
        {
            for (uint64_t j = 0; j < mat.n_cols; ++j)
            {
                mat(i, j) = mat_stl[i][j];
            }
        }
        ```

- Rebuild the project.
    ```bash
        make all
    ```

## 2. Testing Framework

- If you followed the armadillo guide there's nothing else that should be done.

- Rebuild the code and execute ZeroImpute on 1 dataset (airq) using 1 scenario (miss_perc). Use your short name `zeroimp` as an argument for `-alg` command.

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg zeroimp -d airq -scen miss_perc
```

- The precision and runtime results will be added to the `Results` subfolder.
