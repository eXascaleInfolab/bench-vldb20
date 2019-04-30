# Imputation of Missing Values in Time Series Benchmark vldb19

#### Repository structure
- Algorithms - missing value recovery algorithms: InCD, ST-MVL, TRMF, TKCM, SPIRIT, TeNMF, GROUSE, SVDImpute, SoftImpute, SVT, ROSL, DynaMMo.
- Datasets - different datasets and time series from different sources.
- Testing Framework - a program to run automated suite of tests on the datasets with the algorithms mentioned above.

#### List of dependencies

- Ubuntu 16 and higher (or Ubuntu derivatives like Xubuntu)
- Sudo rights on the user.

```bash
    $ git clone https://github.com/eXascaleInfolab/bench-vldb19.git
```

##### Dependencies

- C/C++ compilers and linear algebra libraries:
```bash
    $ sudo apt-get install build-essential cmake libopenmpi-dev libopenblas-dev liblapack-dev libarmadillo-dev libmlpack-dev
```
- GNU Octave with C++ interop libraries, R to enable calculation of errors (MSE/RMSE, correlation), Gnuplot to enable recovery visualization and MSE plots:
```bash
    $ sudo apt-get install octave-pkg-dev r-base gnuplot
```
- Mono Runtime and Compiler: follow step 1 from the installation guide in https://www.mono-project.com/download/stable/ for your Ubuntu version and afterwards do:

```bash
    $ sudo apt-get install mono-devel
    $ sudo apt-get update
    $ sudo apt-get upgrade
```

#### Build & tests

- Restart the terminal window after all the dependencies are installed. Open it in the root folder of the repository.
- Build all the algorithms and Testing Framework using a script in the root folder:
```bash
    $ python linux_build.py
```
- Run the benchmark:
```bash
    $ cd TestingFramework/bin/Debug/
    $ mono TestingFramework.exe
```
- Test suite will go over datasets one by one and executes all the scenarios for them with both precision test and runtime test. Plots folder in the root of the repository will be populated with the results.
- Remark: full test suite with the default setup will take a sizeable amount of time to run (up to 2 days depending on the hardware) and will produce up to 20GB of output files with all recovered data and plots unless stopped early.

#### Custom datasets

To add a dataset to the benchmark
- import the file to `TestingFramework/bin/Debug/data/{name}/{name}_normal.txt`
- - Requirements: >= 10 columns, >= 1'000 rows, column separator - empty space, row separator - newline
- add `{name}` to the list of datasets in `TestingFramework/config.cfg`
- `mono TestingFramework.exe`
