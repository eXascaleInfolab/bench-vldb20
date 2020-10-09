# Adding an algorithm in MATLAB


This tutorial shows how to add a new imputation algorithm to the benchmark. We will illustrate the process by implementing ZeroImpute. If you want to include your own algorithm, then you need to follow the same steps and insert your own code when indicated.
___

## Prerequisites

- The benchmark needs to be executed once (see [execution section](https://github.com/eXascaleInfolab/bench-vldb20)). 
- Algorithm input: a MATLAB matrix where the columns are time series, rows are values, and the missing values are designated as NaN. Algorithm should output the same matrix where all NaN values are recovered. 


## 1. Code Integration

- Choose a *long name* and a *short name* for your algorithm. We will use `ZeroImputeML` and `zeroimpml`, respectively.

- Create a folder for your algorithm with the necessary structure from the root folder
    ```bash
    cd NewAlgorithms/matlab
    mkdir ZeroImputeML
    cd ZeroImputeML
    
    mkdir in
    mkdir out
    cp ../MeanImputeML/recovery.m recovery.m
    ```
    - The file `recovery.m` contains a function `recover_matrix(mx_mis)` that reads a matrix with missing values as NaN and returns a matrix where the missing values are recovered. This function should be used to call your own algorithm.
- Add your own imputation algorithm to the folder.
    - `touch zeroimpute.m`
    - Open `zeroimpute.m` and paste the following code there
        ```matlab
        function res = zeroimpute(matrix)
            mask = isnan(matrix);
            matrix(mask) = 0.0;
            res = matrix;
        end
        ```

    - Open `recovery.m` and change the statement on line 6 to `mx_rec = zeroimpute(mx_mis);`
    - **Remark**: your algorithm should be executable from the root folder and callable as a file-function so it can be used as above.


## 2. Testing Framework

- Create the .cs file

```bash
cd ../../../TestingFramework/
cp AlgoIntegration/MeanImputeMLAlgorithm.cs AlgoIntegration/ZeroImputeMLAlgorithm.cs
```

- Adjust the .cs file
    - Open `AlgoIntegration/ZeroImputeMLAlgorithm.cs`
    - Rename the class and constructor names from `MeanImputeMLAlgorithm` to `ZeroImputeMLAlgorithm` on lines 11 and 14.
    - By default the execution is done through octave. If you want to use MATLAB instead, go to line 51 and change the executable name from `octave-cli` to `matlab`.

- Add the modified .cs file to the project
    - Open `TestingFramework.csproj`
    - On line 80, insert this statement `<Compile Include="AlgoIntegration\ZeroImputeMLAlgorithm.cs" />`


- Add the key properties of the class to a package of executable algorithms.
    - Open `AlgoIntegration/AlgoPack.cs`
    - On line 230, insert the following block: 
        ```C#
        public partial class ZeroImputeMLAlgorithm
        {
            public override string AlgCode => "zeroimpml";
            protected override string _EnvPath => $"{AlgoPack.GlobalNewAlgorithmsLocation}matlab/ZeroImputeML/";
            protected override string SubFolderDataIn => "in/";
            protected override string SubFolderDataOut => "out/";
            
            // In case your algorithm is able to handle multiple incomplete time series, uncomment the following line 
            //public override bool IsMultiColumn => true;
        }
        ```
    
    - On line 31, insert this statement: `public static readonly Algorithm ZeroImpML = new ZeroImputeMLAlgorithm();`

    - Just below, add the name `ZeroImpML` to the array `ListAlgorithms`.

- Rebuild the code and execute ZeroImpute on 1 dataset (airq) using 1 scenario (miss_perc). Use your short name `zeroimpml` as an argument for `-alg` command.

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg zeroimpml -d airq -scen miss_perc
```

- The precision and runtime results will be added to the `Results` subfolder.
