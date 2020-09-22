# Adding new algorithms

___

## Adding a C++ algorithm that uses armadillo

### Introduction

This tutorial shows how to add a new algorithm written in C++ (using armadillo) to the benchmark. We will illustrate the process by copying the code of MeanImpute algorithm.

<!---
The process is done in two main steps: 1) add the code of the algorithm to AlgoCollection and 2) import it into the TestingFramework. 
The process will be illustrated on an example algorithm that we call MeanImpute, but while you follow the guide you can replace the names that are used with your own algorithm as you see fit, so long as they remain consistent. The algorithm is already implemented, so you can use its files as a template.
 --->

### Prerequisites

- Extra dependencies: any, provided they are compatible with C++14 and do not conflict with Armadillo, MLPACK, openBLAS, LAPACK, ARPACK
- Algorithm input: take an arma::mat& class instance where columns are time series and rows are time points, and the missing values are designated as NaN
- Algorithm output: missing values are imputed in the same arma::mat instance as input (it's passed by reference) and the matrix doesn't contain any NaNs or Infs
- We assume that the benchmark was succesfully ran at least once. In the guide we use commands of the form `vim file_name` to denote that we are now working with a specific file in editing mode, which means line numbers refer to the file that was last "opened".

### 1. AlgoCollection

- You need to choose two names for the algorithm: a long name and a short name. We use `NewAlg` and  `nalg` respectively as placeholders.

<!---
You can choose any other names as long as they are used consistently. 
because different parts of the benchmark can use those to communicate between each other. 
In the following guide we will use `NewAlg` as a primary name and `nalg` as a short name.

--->

- Copy the Mean Impute files into the new ones (using your primary name):

```bash
cd Algorithms/NewAlgorithms/cpp
cp Algorithms/MeanImpute.h Algorithms/NewAlg.h
cp Algorithms/MeanImpute.cpp Algorithms/NewAlg.cpp
```

- Add the copied files to the build script

    - `vim Makefile`
    - On lines 2 and 5, go to their end. Before the first linkage statement (`-lopenblas` on line 2, `-L/usr/local/opt/openblas/lib` on line 5) insert the name of the source file of the new algorithm `Algorithms/NewAlg.cpp` next to the other cpp files.

- Adjust the header file

    - `vim Algorithms/NewAlg.h`
    - Rename the class into your primary name and the function there into the `NewAlg_Recovery`.
    - If your algorithm is split across multiple functions, declare them inside the class.

- Add the implementation to the source file
    - `vim Algorithms/NewAlg.cpp`
    - Rename the header name in the include statement on line 2
    - Rename the function into `NewAlg::NewAlg_Recovery` as it was in the header file.
    - The function contains an implementation which you have to delete and replace with your own code.
    - Input argument of this function is `arma::mat &`, missing values are designated as NaN and function arma::is_finite(double) can check for those.

- Call the algorithm with the input given by the tester
    - `vim Performance/Benchmark.cpp`
    - At the end of the file and in the last function `int64_t Recovery()` go to line 65 and add an `else if` block which looks like the following:
        ```C++
        else if (algorithm == "nalg")
        {
            return Recovery_NewAlg(mat);
        }
    - Now we have to create a function that we call from here.
    - The function we are about to add has to contain time measurement functionality which here is done with std::chrono and return the time in microseconds. It also has to verify the output with the call before the return statement. It replaces all the invalid values in the matrix (like NaN or Inf) with a very big number to inflate MSE/RMSE to signal that the algorithm didn't return a valid recovery.
    - Copy the function on lines 33-55 and paste it directly afterwards. Then rename the function name to `Recovery_NewAlg` and then replace the name in the call between the assignments of `begin` and `end` variables from MeanImpute to how you named the function before `NewAlg::NewAlg_Recovery`
    - If your algorithm assumes that the matrix structure has time series as rows instead of columns - uncomment statements `mat = mat.t();` in the function (one before the call, one after).
    - Include the header of our algorithm. Go to line 13 and insert the include statement `#include "../Algorithms/NewAlg.h"`

- Rebuild the project.
    ```bash
        [on Linux] make all
        [on macOS] make mac
    ```

### 2. TestingFramework

- In the second part we will integrate the new algorithm from the collection into the tester.

- Start by going into the correct folder and copying the sample file from MeanImpute into a new file with the name `NewAlgAlgorithm.cs`.

```bash
cd ../../..
cd TestingFramework
cp Algorithms/MeanImputeAlgorithm.cs Algorithms/NewAlgAlgorithm.cs
```

- Add the copied file to the project
    - `vim TestingFramework.csproj`
    - On line 62 insert an extra line with our file we just created (note: path separation in this file uses backslash, not forward slash).
    ```xml
    <Compile Include="Algorithms\NewAlgAlgorithm.cs" />
    ```

- Adjust the algorithm file.
    - `vim Algorithms/NewAlgAlgorithm.cs`
    - Most of the file is service functions and it follows a rather standard template. We just have change a few things.
    - Rename the class and constructor names from `MeanImputeAlgorithm` to `NewAlgAlgorithm` on lines 10 and 13.
    - Change the algorithm code from `meanimp` into your `nalg` at lines 49 and 66 in the cli arguments next to `-alg`.

- Add the key properties of the class to a package of executable algorithms.
    - `vim Algorithms/AlgoPack.cs`
    - On line 200 insert the following block and set the class name to `NewAlgAlgorithm` and AlgCode field to `nalg`.
    ```C#
    public partial class NewAlgAlgorithm
    {
        public override string AlgCode => "nalg";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}NewAlgorithms/cpp/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
    }
    ```
    - Specify the codename (meanimp) and environmental variables. Since it's part of the collection, those are all the same across the board.
    - Next, we have to instantiate the algorithm and add it to the global list. On line 28 add the following statement.
    ```C#
    public static readonly Algorithm NewAlg = new NewAlgAlgorithm();
    ```
    - Add the name `NewAlg` to the array `ListAlgorithms` just below. If your algorithm is capable of imputing values in all time series, not just one, add it also to the "ListAlgorithmsMulticolumn" array.

- We are done editing the code and now we just have to rebuild the project and try to run it on a simple example (1 scenario and 1 dataset). Use your short name `nalg` as an argument for `-alg` command.

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg newalg -d airq -scen miss_perc
```

- Then, in the Results folder here you can find precision and runtime results from running your algorithm.

___

## Adding a C/C++ algorithm using a different algebra library (or STL/cstdlib only)

[Under construction]

___

## Adding an algorithm written in other languages

[Under construction]
