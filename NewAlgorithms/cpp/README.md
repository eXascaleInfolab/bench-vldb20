# Adding an algorithm in C++ (using armadillo library) [Preferred]

This tutorial shows how to add a new imputation algorithm to the benchmark. We will illustrate the process by implementing ZeroImpute. If you want to include your own algorithm, then you need to add your own .cpp and .h files when indicated, follow the same steps, and rebuild the program at the end.
___

<!---
The process is done in two main steps: 1) add the code of the algorithm to AlgoCollection and 2) import it into the TestingFramework. 
The process will be illustrated on an example algorithm that we call MeanImpute, but while you follow the guide you can replace the names that are used with your own algorithm as you see fit, so long as they remain consistent. The algorithm is already implemented, so you can use its files as a template.
 --->

## Prerequisites

- The benchmark needs to be executed once (see [execution section](https://github.com/eXascaleInfolab/bench-vldb20)). 
- Extra dependencies: any, provided they are compatible with C++14 and do not conflict with Armadillo, MLPACK, openBLAS, LAPACK, ARPACK.
- Algorithm input: take an arma::mat& class instance where the missing values are designated as NaN. Algorithm output: missing values are imputed in the same arma::mat instance as input (it is passed by reference). 



<!---
You can choose any other names as long as they are used consistently. 
because different parts of the benchmark can use those to communicate between each other. 
In the following guide we will use `NewAlg` as a primary name and `nalg` as a short name.


In the guide we use commands of the form `vim file_name` to denote that we are now working with a specific file in editing mode, which means line numbers refer to the file that was last "opened".

On lines 2 and 5, go to their end. Before the first linkage statement (`-lopenblas` on line 2, `-L/usr/local/opt/openblas/lib` on line 5) insert the name of the source file of the new algorithm (i.e., `Algorithms/NewAlg.cpp`) next to the other cpp files.

If your algorithm requires linking extra libraries, add all the `-l` and `-L` statement at the end of the lines 2 and 5

and set the class name to `NewAlgAlgorithm` and AlgCode field to `nalg`.

    - `sed -i 's/MeanImpute/NewAlg/g' Algorithms/NewAlg.h`
    - `sed -i 's/MeanImpute/NewAlg/g' Algorithms/NewAlg.cpp`
    - `sed -i 's/MeanImpute/NewAlg/g' AlgoIntegration/NewAlgAlgorithm.cs`
    - `sed -i 's/meanimp/nalg/g' AlgoIntegration/NewAlgAlgorithm.cs`
    - If your algorithm assumes that the matrix structure has time series as rows instead of columns - uncomment statements `mat = mat.t();` in the function (one before the call, one after).
    
```bash
cd NewAlgorithms/cpp
cp Algorithms/MeanImpute.h Algorithms/ZeroImpute.h
cp Algorithms/MeanImpute.cpp Algorithms/ZeroImpute.cpp
```
    
- Adjust the .h file
    - Open `Algorithms/ZeroImpute.h`
    - Rename the class into `ZeroImpute` and the function into `ZeroImpute_Recovery`.
    - If your algorithm is split across multiple functions, declare them inside the class.    

- Add the implementation to the source file
    - Open `Algorithms/ZeroImpute.cpp`
    - on Line 2, rename the header name to `ZeroImpute`.
    - Replace the function `MeanImpute_Recovery()` by the code of [ZeroImpute](https://github.com/eXascaleInfolab/bench-vldb20/blob/master/Algorithms/NewAlgorithms/ZeroImpute.txt). **If you want to add your own algorithm, then your code should go here**.

--->


## 1. Code Integration 

- Choose a *long name* and a *short name* for your algorithm. We will use `ZeroImpute` and `zeroimp`, respectively.

- `cd NewAlgorithms/cpp/`

- Create the files .h and .cpp inside folder `Algorithms`. We have added an example of ZeroImpute.h and ZeroImpute.cpp to the folder.
    - `ZeroImpute.h` contains the class `ZeroImpute`
    - `ZeroImpute.cpp`contains the recovery function  `ZeroImpute::ZeroImpute_Recovery()`

- Add the .cpp file to the build script
    - Open `Makefile`
    - Insert `Algorithms/ZeroImpute.cpp` right before `-lopenblas` (at the end of the line)

- Call the algorithm with the input given by the testing framework
    - Open `Performance/Benchmark.cpp`
    - On line 94, insert the following block to the last function `int64_t Recovery()`
        ```C++
        else if (algorithm == "zeroimp")
        {
            return Recovery_ZeroImpute(mat);
        }
    - Copy and paste the function `Recovery_MeanImpute` on lines 34-56 and rename the function name to `Recovery_ZeroImpute`. Then, replace  `MeanImpute::MeanImpute_Recovery` by `ZeroImpute::ZeroImpute_Recovery` and `Time (MeanImpute)` by `Time (ZeroImpute)`
    - Include the header of the algorithm. Go to line 14 and insert the statement `#include "../Algorithms/ZeroImpute.h"`
     
- Rebuild the project.
    ```bash
        make all
    ```

## 2. Testing Framework

- Create the .cs file

```bash
cd ../../TestingFramework/
cp AlgoIntegration/MeanImputeAlgorithm.cs AlgoIntegration/ZeroImputeAlgorithm.cs
```

- Adjust  the .cs file
    - Open `AlgoIntegration/ZeroImputeAlgorithm.cs`
    - Rename the class and constructor names from `MeanImputeAlgorithm` to `ZeroImputeAlgorithm` on lines 9 and 12.
    - Change the algorithm code from `meanimp` into `zeroimp` on lines 46 and 63 in the cli arguments next to `-alg`.

- Add the modified .cs file to the project
    - Open `TestingFramework.csproj`
    - On line 80, insert this statement `<Compile Include="AlgoIntegration\ZeroImputeAlgorithm.cs" />`


- Add the key properties of the class to a package of executable algorithms.
    - Open `AlgoIntegration/AlgoPack.cs`
    - On line 231, insert the following block: 
        ```C#
        public partial class ZeroImputeAlgorithm
        {
            public override string AlgCode => "zeroimp";
            protected override string _EnvPath => $"{AlgoPack.GlobalNewAlgorithmsLocation}cpp/_data/";
            protected override string SubFolderDataIn => "in/";
            protected override string SubFolderDataOut => "out/";
            
            // In case your algorithm is able to handle multiple incomplete time series, uncomment the following line 
            //public override bool IsMultiColumn => true;
        }
        ```
    
    - On line 31, insert this statement: `public static readonly Algorithm ZeroImp = new ZeroImputeAlgorithm();`

    - Just below, add the name `ZeroImp` to the array `ListAlgorithms`.

- Rebuild the code and execute ZeroImpute on 1 dataset (airq) using 1 scenario (miss_perc). Use your short name `zeroimp` as an argument for `-alg` command.

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg zeroimp -d airq -scen miss_perc
```

- The precision and runtime results will be added to the `Results` subfolder.

___

## C/C++ algorithm using any math library (or STL/cstdlib only)

- `cd NewAlgorithms/cpp/`

- In this example we use C++ STL with `vector<vector<double>>`, but the process is the same for any other library (e.g. Eigen3 with `Eigen::Matrix<double>`). It's also possible to use C pointers, but it has to be able to be compiled with a C++ compiler.

- Modify the .h and .cpp files inside folder `Algorithms`.
    - Open `ZeroImpute.h` and add all the headers your implementation needs. For C++ STL add the standard library header `#include <vector>`. Then replace the argument type in the function `ZeroImpute_Recovery` from `arma::mat &` to `std::vector<std::vector<double>> &`.
    - Open `ZeroImpute.cpp` and update the signature of `ZeroImpute::ZeroImpute_Recovery` to match the header file. Replace the function body to work with the different matrix type:
        ```C++
        for (uint64_t i = 0; i < input.size(); ++i)
        {
            for (uint64_t j = 0; j < input[i].size(); ++j)
            {
                if (std::isnan(input[i][j])))
                {
                    input[i][j] = 0.0;
                }
            }
        }
        ```

- Call the new version of the algorithm with the input given by the testing framework:
    - Open `Performance/Benchmark.cpp`
    - In the function `Recovery_ZeroImpute` add the conversion from armadillo matrix to the type that your algorithm uses. `mat.n_rows` and `mat.n_cols` is matrix dimensions, `mat(i, j)` is element access for i-th row, j-th column.
    - Before `begin = ...` add the following code that creates a two-dimensional `std::vector`, then the loop copies the contents of the armadillo matrix into the new instance:
        ```C++
        std::vector<std::vector<double>> mat_stl(mat.n_rows);
        
        for (uint64_t i = 0; i < mat.n_rows; ++i)
        {
            mat_stl[i] = std::vector<double>(mat.n_cols);
            for (uint64_t j = 0; j < mat.n_cols; ++j)
            {
                mat_stl[i][j] = mat(i, j);
            }
        }
    - Remark: be sure to never re-allocate the memory during copying procedure (with auto-resizable lists or similar), as this can lead to a significant slowdown during testing routines on larger datasets. Matrix dimensions are known here, which is enough to be able to know in advance all the data type sizes to pre-allocate them.
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

- Testing Framework doesn't require any modifications, the same `mono` command will run modified version of the algorithm.
