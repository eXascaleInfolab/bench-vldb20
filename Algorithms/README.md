# Adding new algorithms

___

## Adding a C++ algorithm that uses armadillo

### Introduction

We will show how to add an algorithm that is written in C++ (using arma::) to the benchmark. The process is done in two primary steps: adding the code of the algorithm to AlgoCollection and importing it into the TestingFramework.

The process will be illustrated on an example algorithm that we call MeanImpute, but while you follow the guide you can replace the names that are used with your own algorithm as you see fit, so long as they remain consistent.

### Prerequisites for the algorithm

- Language: C++
- Algebra library: armadillo
- Extra dependencies: any, provided they are compatible with C++14 and don't conflict with Armadillo, MLPACK, openBLAS, LAPACK, ARPACK
- [optional, but recommended] Code has to compile under strict compiler flags `-Wall -Werror -Wextra -pedantic -Wconversion -Wsign-conversion`

- Input: take an arma::mat& class instance where columns are time series and rows are time points, and the missing values are designated as NaN
- Output: missing values are imputed in the same arma::mat instance as input (it's passed by reference) and the matrix doesn't contain any NaNs or Infs

### AlgoCollection

The first part is about adding the algorithm to the collection. We assume that the benchmark itself was succesfully ran at least once with any of the examples.

In the guide we use commands of the form `code file_name` to signal that we're ow working with a specific file in editing mode, which means line numbers refer to the file that was last "opened".

```bash
cd Algorithms/AlgoCollection
touch Algorithms/MeanImpute.h
touch Algorithms/MeanImpute.cpp
```

We create the files and now we will need to add them to the build script.

```bash
code Makefile
```

On lines 2 and 5, go to their end. Before the first linkage statement (`-lopenblas` on line 2, `-L/usr/local/opt/openblas/lib` on line 5) insert the name of the source file of the new algorithm `Algorithms/MeanImpute.cpp` next to the other cpp files.

```bash
code Algorithms/MeanImpute.h
```

Open the header file and copy the following code there.

```C++
#pragma once

#include <armadillo>

namespace Algorithms
{

class MeanImpute
{
  public:
    static void MeanInpute_Recovery(arma::mat &input);
    
  // other function signatyures go here
};

} // namespace Algorithms
```

For simplicity we will only have one function that takes the matrix. If your algorithm is split across multiple functions, declare them inside the class and implement those functions in the following source file.

```bash
code MeanImpute.cpp
```

Open the source file and copy the code there. Input will be received in this function as `arma::mat &`, missing values are designated as NaN and function arma::is_finite(double) can check for those.

```C++
#include <iostream>
#include "MeanImpute.h"

namespace Algorithms
{

void MeanImpute::MeanInpute_Recovery(arma::mat &input)
{
    // your algorithm code goes here
}

// any other functions go here

} // namespace Algorithms
```

If you want to have a functional MeanImpute algorithm, you can replace the comment in the body of `MeanImpute::MeanInpute_Recovery` function with the contents of [this file](https://raw.githubusercontent.com/eXascaleInfolab/bench-vldb20/master/Algorithms/mean_impute_code.txt).

Now that our imlpementation is ready, we need to call it with the input given by the tester.

```bash
code Performance/Benchmark.cpp
```

First, we have to go to the end of the file and in the last function `int64_t Recovery()` go to line 330 and add another `else if` block.

```C++
    else if (algorithm == "meanimp")
    {
        return Recovery_MeanImpute(mat);
    }
```

Here `meanimp` is the short code of the algorithm which we will use in part 2 to identify our algorithm. No we have to create a function that we call from here.

The function we are about to add has to contain time measurement functionality which here is done with std::chrono and return the time in microseconds. It also has to verify the output with the call before the return statement. It replaces all the invalid values in the matrix (like NaN or Inf) with a very big number to inflate MSE/RMSE to signal that the algorithm didn't return a valid recovery. If validation is not performed and the matrix contains invalid values - tester will crash.

Go to line 283 and copy the code of this function there directly after similar ones.

```C++
int64_t Recovery_MeanImpute(arma::mat &mat)
{
    // Local
    int64_t result;
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    
    // Recovery
    begin = std::chrono::steady_clock::now();
    MeanImpute::MeanInpute_Recovery(mat);
    end = std::chrono::steady_clock::now();
    
    result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "Time (MeanImpute): " << result << std::endl;
    
    verifyRecovery(mat);
    return result;
}
```

And finally, we have to include the header of our algorithm. Go to line 23 and insert the include statement.

```C++
#include "../Algorithms/MeanImpute.h"
```

Now those are the final changes to the collection, we have to rebuild it now.

```bash
    [on Linux] make all
    [on macOS] make mac
```

### TestingFramework

In the second part we will integrate this new algorithm from the collection into the tester.

```bash
cd ../..
cd TestingFramework
touch Algorithms/MeanImputeAlgorithm.cs
```

First we created a new file, now we add it to the project.

```bash
code TestingFramework.csproj
```

On line 62 insert an extra line with our file we just created (note: path separation in this file uses backslash, not forward slash).

```xml
    <Compile Include="Algorithms\MeanImputeAlgorithm.cs" />
```

Now open the new file.

```bash
code Algorithms/MeanImputeAlgorithm.cs
```

And copy the following code into it. Most of it consists of service functions and follows a rather standard template. We just give the class a proper name and put the algorithm code that we deicided "meanimp" at lines 49 and 66.

```C#
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class MeanImputeAlgorithm : Algorithm
    {
        private static bool _init = false;
        public MeanImputeAlgorithm() : base(ref _init)
        { }

        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}.txt" };
        }
        
        private static string Style => "linespoints lt 8 dt 2 lw 3 pt 1 lc rgbcolor \"black\" pointsize 1.2";

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms()
        {
            return new[] { new SubAlgorithm($"{AlgCode}", String.Empty, Style) };
        }

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms(int tcase)
        {
            return new[] { new SubAlgorithm($"{AlgCode}", $"{AlgCode}{tcase}", Style) };
        }
        
        protected override void PrecisionExperiment(ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
            RunAlgortithm(GetProcess(data, tcase));
        }
        
        private Process GetProcess(DataDescription data, int len)
        {
            Process proc = new Process();
            
            proc.StartInfo.WorkingDirectory = EnvPath;
            proc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            proc.StartInfo.UseShellExecute = false;

            proc.StartInfo.Arguments = $"-alg meanimp -test o -n {data.N} -m {data.M} " +
                                         $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                         $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return proc;
        }
        
        private Process GetRuntimeProcess(DataDescription data, int len)
        {
            Process proc = new Process();
            
            proc.StartInfo.WorkingDirectory = EnvPath;
            proc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            proc.StartInfo.UseShellExecute = false;

            proc.StartInfo.Arguments = $"-alg meanimp -test rt -n {data.N} -m {data.M} " +
                                             $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                             $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return proc;
        }
        private void RunAlgortithm(Process proc)
        {
            proc.Start();
            proc.WaitForExit();
                
            if (proc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] MeanImpute returned code {proc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {proc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunAlgortithm(GetRuntimeProcess(data, tcase));
        }

        public override void GenerateData(string sourceFile, string code, int tcase, (int, int, int)[] missingBlocks,
            (int, int) rowRange, (int, int) columnRange)
        {
            sourceFile = DataWorks.FolderData + sourceFile;
            
            (int rFrom, int rTo) = rowRange;
            (int cFrom, int cTo) = columnRange;
            
            double[][] res = DataWorks.GetDataLimited(sourceFile, rTo - rFrom, cTo - cFrom);
            
            int n = rTo > res.Length ? res.Length : rTo;
            int m = cTo > res[0].Length ? res[0].Length : cTo;
            
            var data = new StringBuilder();

            for (int i = rFrom; i < n; i++)
            {
                string line = "";

                for (int j = cFrom; j < m; j++)
                {
                    if (Utils.IsMissing(missingBlocks, i, j))
                    {
                        line += "NaN" + " ";
                    }
                    else
                    {
                        line += res[i][j] + " ";
                    }
                }
                data.Append(line.Trim() + Environment.NewLine);
            }

            string destination = EnvPath + SubFolderDataIn + $"{code}_m{tcase}.txt";
            
            if (File.Exists(destination)) File.Delete(destination);
            File.AppendAllText(destination, data.ToString());
        }
    }
}
```

After that we need to add the key properties of the class to a package of executable algorithms.

```bash
code Algorithms/AlgoPack.cs
```

First, we need to specify the codename (meanimp) and environmental variables. Since it's part of the collection, those are all the same across the board. On line 200 insert the following block.

```C#
public partial class MeanImputeAlgorithm
{
    public override string AlgCode => "meanimp";
    protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
    protected override string SubFolderDataIn => "in/";
    protected override string SubFolderDataOut => "out/";
}
```

Next, we have to instantiate the algorithm and add it to the global list. On line 28 add the following statement.

```C#
public static readonly Algorithm MeanImp = new MeanImputeAlgorithm();
```

Then, add the name MeanImp to the array "ListAlgorithms" just below. If your algorithm is capable of imputing values in all time series, not just one, add it also to the "ListAlgorithmsMulticolumn" array to signal that it doesn't have the restriciton of only imputing a single time series.

We're done editing the code and now we just have to rebuild the project and try to run it on a simple example (1 scenario and 1 dataset).

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg meanimp -d airq -scen miss_perc
```

Then, in the Results folder here you can find precision and runtime results from running MeanImpute.

### Limitations

- Do not invoke `-algx` with your algorithm name - the tester will crash.

- If your C++ code doesn't compile under the restrictive flags and it's too much work to change it - you have to disable them in the Makefile, the flags are in the beginning of the build command.

- If your algorithm expects matrix in the tranposed form (i.e. where time series are rows and not columns), please see `Performance/Benchmark.cpp`, the function around ~180 for DynaMMo algorithms handles it to transpose the matrix before feeding it in, and back after the output **outside** of time measurement.

___

## Adding a C/C++ algorithm using a different algebra library (or STL/cstdlib only)

[Under construction]

___

## Adding an algorithm written in other languages

[Under construction]
