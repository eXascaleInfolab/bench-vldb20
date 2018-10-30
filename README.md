# InCD_benchmark

#### Repository structure
- Algorithms - missing value recovery algorithms: CD, InCD, ST-MVL, TRMF, TKCM, SPIRIT, TeNMF, GROUSE.
- Datasets - different datasets and time series from different sources.
- Testing Framework - a program to run automated suite of tests on the datasets with the algorithms mentioned above.

Each directory contains its own README file with the descriptions of details.

#### List of dependencies

- Ubuntu 14 and higher (or Ubuntu derivatives like Xubuntu)

or
- Supported version of macOS (10.11+)
- Sudo rights on the user.
- Download the repository as zip and extract.

or
- Open terminal in a folder you want to put the project to:
```bash
    $ git init
    $ git clone https://github.com/eXascaleInfolab/InCD_benchmark.git
```
- macOS only: if at `git init` step or any time later a prompt about developer tools appears, choose "install", then repeat the last command you tried.


##### Dependencies for GNU/Linux

- C/C++ compilers:
```bash
    $ sudo apt-get install build-essential
```
- Python and python libraries:
```bash
    $ sudo apt-get install python-all
    $ sudo apt-get install python-numpy
    $ sudo apt-get install python-scipy
    $ sudo apt-get install python-sklearn
    $ sudo apt-get install python-pandas
```
- Mono Runtime and Compiler: install `mono-devel` from the installation guide in https://www.mono-project.com/download/stable/ for your Ubuntu version and afterwards do:

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

##### Dependencies for macOS

- Homebrew ( https://brew.sh/ ), paste the following command into the terminal and follow the instructions:
```bash
    $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
- Python and python libraries:
```bash
    $ brew install python2
    $ pip2 install numpy
    $ pip2 install scipy
    $ pip2 install pandas
    $ pip2 install sklearn
```
- Mono Runtime and Compiler. Open https://www.mono-project.com/download/stable/ and install the Visual Studio channel.
- GNU Octave
```bash
    $ brew install octave
```
- R to enable calculation of errors (MSE/RMSE, correlation):
```bash
    $ brew install r
```
- Gnuplot to enable recovery visualization (doesn't require R) and MSE plots (requires R as well):
```bash
    $ brew install gnuplot
```

#### Build & tests

- Restart the terminal window after all the dependencies are installed. Open it in the root folder of the repository.
- Build all the algorithms and Testing Framework using a script depending on your platform:
```bash
    (macOS)
    $ python mac_build.py
    (Linux)
    $ python linux_build.py
```
- Enter TestingFramework/ folder, open config.cfg file in a text editor and specify all the necessary paths.
- Open the command line in this folder and launch the tests from it:
```bash
    (from the root folder of the repository)
    $ cd TestingFramework/bin/Debug/
    $ mono TestingFramework.exe
```

#### Custom datasets

To add a dataset to the benchmark
- import the file to `TestingFramework/bin/Debug/data/{name}/{name}_normal.txt`
- - Requirements: >= 4 columns, >= 2'000 rows, column separator - empty space, row separator - newline
- add `{name}` to the list of datasets in `TestingFramework/config.cfg`
- `mono TestingFramework.exe`
