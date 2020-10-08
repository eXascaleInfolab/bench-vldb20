# Adding an algorithm in Python


This tutorial shows how to add a new imputation algorithm to the benchmark. We will illustrate the process by implementing MeanImpute. If you want to include your own algorithm, then you need to follow the same steps and insert your own code when indicated.
___

## Prerequisites

- The benchmark needs to be executed once (see [execution section](https://github.com/eXascaleInfolab/bench-vldb20)). 
- Algorithm input: a NumPy matrix where the columns are time series, rows are values, and the missing values are designated as NaN. Algorithm should output the same NumPy matrix where all NaN values are recovered. 


## 1. Code Integration

- Choose a *long name* and a *short name* for your algorithm. We will use `ZeroImputePy` and `zeroimppy`, respectively.

- Create a folder for your algorithm with the necessary structure
    ```bash
    cd NewAlgorithms/python
    mkdir ZeroImputePy
    cd ZeroImputePy
    
    mkdir in
    mkdir out
    touch __init__.py
    cp ../MeanImputePy/recovery.py recovery.py
    ```
    - The file recovery.py is used for integration with the Testing Framework. It contains a function `recover_matrix(matrix)` that reads a numpy matrix with missing values as NaN and returns a numpy matrix where the missing values are recovered. This function should be used to call your own algorithm.
- Add your own imputation algorithm to the folder.
    - `touch zeroimpute.py`
    - Open `zeroimpute.py` and paste the following code there
        ```python
        import numpy as np
        def zeroimpute_recovery(matrix):
            mask = np.isnan(matrix);
            matrix[mask] = 0.0;
            return matrix;
        ```

    - Open `recovery.py` and change the import statement to `from zeroimpute import zeroimpute_recovery` and the return statement on line 8 to `return zeroimpute_recovery(matrix);`
    - **Remark**: your algorithm should be executable from the root folder and importable as a function using python `import` so it can be used as above.


## 2. Testing Framework

- Create the .cs file

```bash
cd ../../../TestingFramework/
cp AlgoIntegration/MeanImputePyAlgorithm.cs AlgoIntegration/ZeroImputePyAlgorithm.cs
```

- Adjust the .cs file
    - Open `AlgoIntegration/ZeroImputePyAlgorithm.cs`
    - Rename the class and constructor names from `MeanImputePyAlgorithm` to `ZeroImputePyAlgorithm` on lines 11 and 14.

- Add the modified .cs file to the project
    - Open `TestingFramework.csproj`
    - On line 80, insert this statement `<Compile Include="AlgoIntegration\ZeroImputePyAlgorithm.cs" />`


- Add the key properties of the class to a package of executable algorithms.
    - Open `AlgoIntegration/AlgoPack.cs`
    - On line 230, insert the following block: 
        ```C#
        public partial class ZeroImputePyAlgorithm
        {
            public override string AlgCode => "zeroimppy";
            protected override string _EnvPath => $"{AlgoPack.GlobalNewAlgorithmsLocation}python/ZeroImputePy/";
            protected override string SubFolderDataIn => "in/";
            protected override string SubFolderDataOut => "out/";
            
            // In case your algorithm is able to handle multiple incomplete time series, uncomment the following line 
            //public override bool IsMultiColumn => true;
        }
        ```
    
    - On line 31, insert this statement: `public static readonly Algorithm ZeroImpPy = new ZeroImputePyAlgorithm();`

    - Just below, add the name `ZeroImpPy` to the array `ListAlgorithms`.

- Rebuild the code and execute ZeroImpute on 1 dataset (airq) using 1 scenario (miss_perc). Use your short name `zeroimp` as an argument for `-alg` command.

```bash
msbuild TestingFramework.sln
cd bin/Debug
mono TestingFramework.exe -alg zeroimppy -d airq -scen miss_perc
```

- The precision and runtime results will be added to the `Results` subfolder.
