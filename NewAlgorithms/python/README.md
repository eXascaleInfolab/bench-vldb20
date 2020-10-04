# Adding an algorithm in Python


This tutorial shows how to add a new imputation algorithm to the benchmark. We will illustrate the process by implementing ZeroImpute. If you want to include your own algorithm, then you need to perform the process and insert your own code when indicated.
___

## Prerequisites

- The benchmark needs to be executed once (see [execution section](https://github.com/eXascaleInfolab/bench-vldb20)). 
- Extra dependencies: any that can be installed on the same system which is a pre-requisite for benchmark and doesn't conflict with any of its depencencies.
- Algorithm input: receive a NumPy matrix where the columns are time series, rows are time points and the missing values are designated as NaN. Algorithm should output the same NumPy matrix with all NaN values imputed. 


## Algorithm implementation (an example with python)

- Choose a *long name* and a *short name* for your algorithm. We will use `ZeroImputePy` and `zeroimppy`, respectively.

- `cd NewAlgorithms/`

- Create a folder for your algorithm and enter it
    ```bash
    mkdir ZeroImputePy
    cd ZeroImputePy
    ```

- Populate the folder with necessary subfolders
    ```bash
    mkdir in
    mkdir out
    ```

- Copy the template that contains the entry point to the python script (from an implementation of Mean Imputation algorithm)
    - `cp ../python/recovery.py recovery.py
    
- Open `src/recovery.py`
    - In this file, we have a function `rmv_main` that handles reading the input/output files. The actual recovery function is `recover_matrix(matrix)`.
    - This function has only one requirement - read a numpy matrix with NaN as missing values and return a numpy matrix where those values are imputed.
    - Copy the code for Zero Imputation into the function `recover_matrix`
        ```python
        mask = np.isnan(matrix);
        matrix[mask] = 0.0;
        return matrix;
        ```
    - When you add your algorithm, it should be executable from the root folder and importable as a function using python `import`.
    - Replace the code inside the `recovery_function` function with either your algorithm, or import your algorithm and call it from the function.


## 2. TestingFramework

- Create the .cs file

```bash
cd ../../TestingFramework/
cp AlgoIntegration/MeanImputePyAlgorithm.cs AlgoIntegration/ZeroImputePyAlgorithm.cs
```

- Adjust the .cs file
    - Open `AlgoIntegration/ZeroImputePyAlgorithm.cs`
    - Rename the class and constructor names from `MeanImputePyAlgorithm` to `ZeroImputePyAlgorithm` on lines 9 and 12.

- Add the modified .cs file to the project
    - Open `TestingFramework.csproj`
    - On line 80, insert this statement `<Compile Include="AlgoIntegration\ZeroImputePyAlgorithm.cs" />`


- Add the key properties of the class to a package of executable algorithms.
    - Open `AlgoIntegration/AlgoPack.cs`
    - On line 231, insert the following block: 
        ```C#
        public partial class ZeroImputePyAlgorithm
        {
            public override string AlgCode => "zeroimppy";
            protected override string _EnvPath => $"{AlgoPack.GlobalNewAlgorithmsLocation}python/";
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
