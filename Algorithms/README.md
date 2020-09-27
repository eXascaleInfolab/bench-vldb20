# Adding new algorithms

___

## Adding a C++ algorithm that uses armadillo
This tutorial shows how to add a new imputation algorithm to the benchmark. We will illustrate the process by implementing ZeroImpute. If you want to include your own algorithm, then you need to add the corresponding .cpp and .h files, follow the same steps, and rebuild the program at the end.


<!---
The process is done in two main steps: 1) add the code of the algorithm to AlgoCollection and 2) import it into the TestingFramework. 
The process will be illustrated on an example algorithm that we call MeanImpute, but while you follow the guide you can replace the names that are used with your own algorithm as you see fit, so long as they remain consistent. The algorithm is already implemented, so you can use its files as a template.
 --->

### Prerequisites

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
    - `sed -i 's/MeanImpute/NewAlg/g' Algorithms/NewAlgAlgorithm.cs`
    - `sed -i 's/meanimp/nalg/g' Algorithms/NewAlgAlgorithm.cs`
    - If your algorithm assumes that the matrix structure has time series as rows instead of columns - uncomment statements `mat = mat.t();` in the function (one before the call, one after).
    
```bash
cd Algorithms/NewAlgorithms/cpp
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


### 1. AlgoCollection

- Choose a *long name* and a *short name* for your algorithm. We will use `ZeroImpute` and `zeroimp`, respectively.

- cd `Algorithms/NewAlgorithms/`

- Create the files ZeroImpute.h and ZeroImpute.cpp in `cpp/Algorithms`. We have added an example of the two files in the same folder
    - `ZeroImpute.cpp` contains a header named `ZeroImpute` and a recovery function named  `ZeroImpute_Recovery()`
    - `ZeroImpute.h` contains class `ZeroImpute_Recovery`

- Add the .cpp file to the build script
    - Open `cpp/Makefile`
    - Insert `Algorithms/ZeroImpute.cpp` right before `-lopenblas` (at the end of the line)

- Call the algorithm with the input given by the tester
    - Open `cpp/Performance/Benchmark.cpp`
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

### 2. TestingFramework

- Create the .cs file

```bash
cd ../../TestingFramework/
cp Algorithms/MeanImputeAlgorithm.cs Algorithms/ZeroImputeAlgorithm.cs
```

- Adjust  the .cs file
    - Open `Algorithms/ZeroImputeAlgorithm.cs`
    - Rename the class and constructor names from `MeanImputeAlgorithm` to `ZeroImputeAlgorithm` on lines 10 and 13.
    - Change the algorithm code from `meanimp` into `zeroimp` on lines 49 and 66 in the cli arguments next to `-alg`.

- Add the modified .cs file to the project
    - Open `TestingFramework.csproj`
    - On line 64, insert this statement `<Compile Include="Algorithms\ZeroImputeAlgorithm.cs" />`


- Add the key properties of the class to a package of executable algorithms.
    - Open `Algorithms/AlgoPack.cs`
    - On line 219, insert the following block: 
        ```C#
        public partial class ZeroImputeAlgorithm
        {
            public override string AlgCode => "zeroimp";
            protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}NewAlgorithms/cpp/_data/";
            protected override string SubFolderDataIn => "in/";
            protected override string SubFolderDataOut => "out/";
        }
        ```
    - On line 30, insert this statement: `public static readonly Algorithm ZeroImp = new ZeroImputeAlgorithm();`

    - Just below, add the name `ZeroImp` to the array `ListAlgorithms` and to the array `"ListAlgorithmsMulticolumn`(if your algorithm is capable of imputing values in multiple time series).

- Rebuild the code and execute ZeroImpute using 1 scenario (miss_perc) on 1 dataset (airq). Use your short name `zeroimp` as an argument for `-alg` command.

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg zeroimp -d airq -scen miss_perc
```

- The precision and runtime results will be added to the `Results` subfolder.

___

## Adding a C/C++ algorithm using a different algebra library (or STL/cstdlib only)

[Under construction]

___

## Adding an algorithm written in other languages

[Under construction]
