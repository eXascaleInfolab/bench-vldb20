# InCD_benchmark

#### Repository structure
- Algorithms - missing value recovery algorithms: InCD, ST-MVL, TRMF, TKCM, SPIRIT, TeNMF, GROUSE, SVDImpute, SoftImpute, SVT, ROSL, DynaMMo.
- Datasets - different datasets and time series from different sources.
- Testing Framework - a program to run automated suite of tests on the datasets with the algorithms mentioned above.

Each directory contains its own README file with the descriptions of details.

#### List of dependencies

- Ubuntu 16 and higher (or Ubuntu derivatives like Xubuntu)
- Sudo rights on the user.
- Download the repository as zip and extract.

or
- Open terminal in a folder you want to put the project to:
```bash
    $ git init
    $ git clone https://github.com/eXascaleInfolab/InCD_bench-19.git
```

##### Dependencies

- C/C++ compilers:
```bash
    $ sudo apt-get install build-essential
```
- C/C++ linear algebra libraries:
```bash
    $ sudo apt-get install cmake
    $ sudo apt-get install libopenmpi-dev
    $ sudo apt-get install libopenblas-dev
    $ sudo apt-get install liblapack-dev
    $ sudo apt-get install libarmadillo-dev
    $ sudo apt-get install libmlpack-dev
```
- Mono Runtime and Compiler: follow step 1 from the installation guide in https://www.mono-project.com/download/stable/ for your Ubuntu version and afterwards do:

```bash
    $ sudo apt-get install mono-devel
    $ sudo apt-get update
    $ sudo apt-get upgrade
```
- GNU Octave with C++ interop libraries:
```bash
    $ sudo apt-get install octave-pkg-dev
```
- R to enable calculation of errors (MSE/RMSE, correlation):
```bash
    $ sudo apt-get install r-base
```
- Gnuplot to enable recovery visualization (doesn't require R) and MSE plots (requires R as well):
```bash
    $ sudo apt-get install gnuplot
```

#### Build & tests

- Restart the terminal window after all the dependencies are installed. Open it in the root folder of the repository.
- Build all the algorithms and Testing Framework using a script in the root folder (takes around ~1min):
```bash
    $ python linux_build.py
```
- If you want to change the destination of plots and results (default is a folder one level up from the root of repository names "Plots"), enter TestingFramework/ folder, open config.cfg file in a text editor and specify all the necessary paths. Also you can customize the experiment by managing datasets and algorithms.
- Open the command line in the root folder of the repository and launch the tests from it:
```bash
    $ cd TestingFramework/bin/Debug/
    $ mono TestingFramework.exe
```
- Remark: full test suite with the default setup will take a sizeable amount of time to run (up to 2 days depending on the hardware) and will produce up to 15GB of output files with all recovered data and plots.

#### Custom datasets

To add a dataset to the benchmark
- import the file to `TestingFramework/bin/Debug/data/{name}/{name}_normal.txt`
- - Requirements: >= 10 columns, >= 1'000 rows, column separator - empty space, row separator - newline
- add `{name}` to the list of datasets in `TestingFramework/config.cfg`
- `mono TestingFramework.exe`
