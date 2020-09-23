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

- The benchmark needs to be executed once (see execution section in the main repo). 
- Extra dependencies: any, provided they are compatible with C++14 and do not conflict with Armadillo, MLPACK, openBLAS, LAPACK, ARPACK.
- Algorithm input: take an arma::mat& class instance where the missing values are designated as NaN. Algorithm output: missing values are imputed in the same arma::mat instance as input (it is passed by reference). 


### 1. AlgoCollection

- You need to choose two names for the algorithm: a long name and a short name. We use `NewAlg` and  `nalg` respectively as placeholders.

<!---
You can choose any other names as long as they are used consistently. 
because different parts of the benchmark can use those to communicate between each other. 
In the following guide we will use `NewAlg` as a primary name and `nalg` as a short name.


In the guide we use commands of the form `vim file_name` to denote that we are now working with a specific file in editing mode, which means line numbers refer to the file that was last "opened".

On lines 2 and 5, go to their end. Before the first linkage statement (`-lopenblas` on line 2, `-L/usr/local/opt/openblas/lib` on line 5) insert the name of the source file of the new algorithm (i.e., `Algorithms/NewAlg.cpp`) next to the other cpp files.

If your algorithm requires linking extra libraries, add all the `-l` and `-L` statement at the end of the lines 2 and 5

and set the class name to `NewAlgAlgorithm` and AlgCode field to `nalg`.

--->

- Copy the Mean Impute files into the new ones (using the long name):

```bash
cd Algorithms/NewAlgorithms/cpp
cp Algorithms/MeanImpute.h Algorithms/NewAlg.h
cp Algorithms/MeanImpute.cpp Algorithms/NewAlg.cpp
```

- Add the copied files to the build script
    - Open `Makefile`
    - Insert `Algorithms/NewAlg.cpp` right before `-lopenblas` and `-L/usr/local/opt/openblas/lib`



- Adjust the header file
    - Open `Algorithms/NewAlg.h`
    - Rename the class into `NewAlg` and the function into `NewAlg_Recovery`.
    - If your algorithm is split across multiple functions, declare them inside the class.

- Add the implementation to the source file
    - Open `Algorithms/NewAlg.cpp`
    - Rename the header name on Line 2 into `NewAlg` and the function into `NewAlg::NewAlg_Recovery`.
    - <span style="color:red">*This function should contain the code of your algorithm.*</span>


- Call the algorithm with the input given by the tester
    - Open `Performance/Benchmark.cpp`
    - On line 65, insert the following block to the last function `int64_t Recovery()`
        ```C++
        else if (algorithm == "nalg")
        {
            return Recovery_NewAlg(mat);
        }
    - Copy and paste the function `Recovery_MeanImpute` on lines 33-55 and rename the function name to `Recovery_NewAlg`. Then, replace the name in the call between the assignments of `begin` and `end` variables from `MeanImpute::MeanImpute_Recovery` to `NewAlg::NewAlg_Recovery`
    - If your algorithm assumes that the matrix structure has time series as rows instead of columns - uncomment statements `mat = mat.t();` in the function (one before the call, one after).
    - Include the header of the algorithm. Go to line 13 and insert the statement `  #include "../Algorithms/NewAlg.h"`
     
- Rebuild the project.
    ```bash
        make all
    ```

### 2. TestingFramework

- Go to the tester's folder and copy the sample file from MeanImpute into a new file with the name `NewAlgAlgorithm.cs`.

```bash
cd ../../../TestingFramework/
cp Algorithms/MeanImputeAlgorithm.cs Algorithms/NewAlgAlgorithm.cs
```

- Adjust the algorithm file.
    - Open `Algorithms/NewAlgAlgorithm.cs`
    - Rename the class and constructor names from `MeanImputeAlgorithm` to `NewAlgAlgorithm` on lines 10 and 13.
    - Change the algorithm code from `meanimp` into your `nalg` at lines 49 and 66 in the cli arguments next to `-alg`.

- Add the copied file to the project
    - Open `TestingFramework.csproj`
    - On line 62, insert this statement `<Compile Include="Algorithms\NewAlgAlgorithm.cs" />`


- Add the key properties of the class to a package of executable algorithms.
    - Open `Algorithms/AlgoPack.cs`
    - On line 201, insert the following block: 
        ```C#
        public partial class NewAlgAlgorithm
        {
            public override string AlgCode => "nalg";
            protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}NewAlgorithms/cpp/_data/";
            protected override string SubFolderDataIn => "in/";
            protected override string SubFolderDataOut => "out/";
        }
        ```
    - On line 29, insert this statement: `public static readonly Algorithm NewAlg = new NewAlgAlgorithm();`

    - Just below, add the name `NewAlg` to the array `ListAlgorithms` and to the array `"ListAlgorithmsMulticolumn`, if your algorithm is capable of imputing values in multiple time series.

- The editing part is done! Now we just need to rebuild the project and try to run it on a simple example (1 scenario and 1 dataset). Use your short name `nalg` as an argument for `-alg` command.

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg nalg -d airq -scen miss_perc
```

- The precision and runtime results will be added to the `Results` subfolder.

___

## Adding a C/C++ algorithm using a different algebra library (or STL/cstdlib only)

[Under construction]

___

## Adding an algorithm written in other languages

[Under construction]
