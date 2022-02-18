# ImputeBench:  Benchmark of Imputation Techniques in Time Series  

ImputeBench implements 15 SOTA recovery techniques for blocks of missing values in time series and evaluates their precision and runtime on various real-world time series datasets using different recovery scenarios. Technical details can be found in our 
PVLDB 2020 paper:  <a href = "http://www.vldb.org/pvldb/vol13/p768-khayati.pdf">Mind the Gap: An Experimental Evaluation of Imputation of Missing Values Techniques in Time Series </a>. The benchmark can be easity extended with new algorithms (C/C++, Python or Matlab), new datasets and new scenarios.  

- The benchmark implements the following algorithms (in C++): [CDRec](https://rdcu.be/b32bv), [DynaMMo](https://dl.acm.org/doi/10.1145/1557019.1557078), [GROUSE](http://proceedings.mlr.press/v51/zhang16b.html),  [ROSL](https://ieeexplore.ieee.org/abstract/document/6909890), [SoftImpute](https://www.jmlr.org/papers/v11/mazumder10a.html), [SPIRIT](https://dl.acm.org/doi/10.5555/1083592.1083674), [STMVL](https://www.ijcai.org/Proceedings/16/Papers/384.pdf), [SVDImpute](https://academic.oup.com/bioinformatics/article/17/6/520/272365), [SVT](https://epubs.siam.org/doi/10.1137/080738970?mobileUi=0), [TeNMF](http://proceedings.mlr.press/v70/mei17a.html), [TRMF](https://papers.nips.cc/paper/6160-temporal-regularized-matrix-factorization-for-high-dimensional-time-series-prediction.pdf), and [TKCM](https://openproceedings.org/2017/conf/edbt/paper-112.pdf). We recently added these algorithms (in Python): [SSA](https://dl.acm.org/doi/10.1145/3287319), [MRNN](https://ieeexplore.ieee.org/document/8485748) and [BRITS](http://papers.nips.cc/paper/7911-brits-bidirectional-recurrent-imputation-for-time-series).
- All the datasets used in this benchmark can be found [here](https://github.com/eXascaleInfolab/bench-vldb20/tree/master/Datasets).
- The full list of recovery scenarios can be found [here](https://github.com/eXascaleInfolab/bench-vldb20/blob/master/TestingFramework/README.md).
- **Remark**: Algorithms `tkcm`,  `spirit`, `ssa`, and `mr-nn`  cannot handle multiple incomplete time series. These allgorithms will not produce results for the following scenarios: `miss_disj`, `miss_over`, `mcar` and `blackout`.


[**Prerequisites**](#prerequisites) | [**Build**](#build) | [**Execution**](#execution) | [**Extension**](#extension)  | [**Contributors**](#contributors) | [**Award**](#award) | [**Citation**](#citation)

___




## Prerequisites

- Ubuntu 16 or Ubuntu 18 (including Ubuntu derivatives, e.g., Xubuntu) or the same distribution under WSL.
- Clone this repository.
- Mono: Install mono from https://www.mono-project.com/download/stable/ and reboot your terminal.

___



## Build

- Build the Testing Framework using the installation script located in the root folder (takes several minutes)
```bash
    $ sh install_linux.sh
```
- To evaluate the recently added algorithms (SSA, MRNN and BRITS), please install the following packages (takes several minutes):
```bash
    $ sh install_extra.sh
```
___

## Execution


```bash
    $ cd TestingFramework/bin/Debug/
    $ mono TestingFramework.exe [arguments]
```

### Arguments

 | -alg  | -d  |  -scen 
 | -------- | -------- | -------- |
 | cdrec    | airq        | miss_perc |
 | dynammo  | bafu        | ts_length |
 | grouse   | chlorine    | ts_nbr    |
 | rosl     | climate     | miss_disj |
 | softimp  | drift10     | miss_over |
 | svdimp   | electricity | mcar      |
 | svt      | meteo       | blackout  |
 | stmvl    | temp        | *all*     |
 | spirit   | bafu_red    |           |
 | tenmf    | drift10_red |           |
 | tkcm     | *all*       |           |
 | trmf     |             |           |
 | *all*    |             |           |
 | -------- | -------- | -------- |
 | New algs |             |           |
 | -------- | -------- | -------- |
 | ssa      |             |           |
 | m-rnn    |             |           |
 | brits    |             |           |



### Results
All results and plots will be added to `Results` folder. The accuracy results of all algorithms will be sequentially added for each scenario and dataset to: `Results/.../.../error/`. The runtime results of all algorithms will be added to: `Results/.../.../runtime/`. The plots of the recovered blocks will be added to the folder `Results/.../.../recovery/plots/`.


### Execution examples


1. Run a single algorithm (cdrec) on a single dataset (drift10) using one scenario (missing percentage)
```bash
    $ mono TestingFramework.exe -alg cdrec -d drift10 -scen miss_perc
```

2. Run two algorithms (cdrec, spirit) on a single dataset (drift10) using one scenario (missing percentage)
```bash
    $ mono TestingFramework.exe -alg cdrec,spirit -d drift10 -scen miss_perc
```

3. Run point 2 without runtime results
```bash
    $ mono TestingFramework.exe -alg cdrec,spirit -d drift10 -scen miss_perc -nort
```

4. Run the whole VLDB'20 benchmark (all algorithms, all datasets, all scenarios, precision and runtime)
```bash
    $ mono TestingFramework.exe -alg all -d all -scen all
```
**Warning**: Running the whole benchmark will take a sizeable amount of time (up to 4 days depending on the hardware) and will produce up to 15GB of output files with all recovered data and plots unless stopped early.

5. Create patterns of missing blocks on one complete dataset (airq) using one scenario (missing percentage)
```bash
    $ mono TestingFramework.exe -alg mvexport -d airq -scen miss_perc
```
**Note**: You need to run each scenario seperately on one or multiple datasets. Each time you execute one scenario, the `Results` folder will be overwritten with the new files.

6. Additional command-line parameters
```bash
    $ mono TestingFramework.exe --help
```


### Parametrized execution

- You can parametrize each algorithm using the command `-algx`. For example, you can run
the svdimp algorithm with a reduction value of 4 on the drift dataset and by varying the sequence length as follows:

```bash
    $ mono TestingFramework.exe -algx svdimp 4 -d drift10 -scen ts_nbr
```

- If you want to run some algorithms with default parameters, and some with customized ones, you can use `-alg` and `-algx` together. For example, you can run stmvl algorithm with default parameter and cdrec algorithm with a reduction value of 4 on the airq dataset and by varying the sequence length as follows:

```bash
    $ mono TestingFramework.exe -alg stmvl -algx cdrec 4 -d airq -scen ts_nbr
```

**Remark**: The command `-algx` cannot be executed in group and thus must preceed the name of each algorithm.

___

## Extension
- To add new algorithms:
    - C/C++: https://github.com/eXascaleInfolab/bench-vldb20/tree/master/NewAlgorithms/cpp
    - Python: https://github.com/eXascaleInfolab/bench-vldb20/tree/master/NewAlgorithms/python
    - Matlab: https://github.com/eXascaleInfolab/bench-vldb20/tree/master/NewAlgorithms/matlab
- To add new datasets:
  - import the file to `TestingFramework/bin/Debug/data/{name}/{name}_normal.txt` (`name` is the name of your dataset).
  - **Requirements**: rows>= 1'000, columns>= 10, column separator: empty space, row separator: newline
  - **Note**: the benchmark can also run with rows>= 100 and columns>= 5, but with a limited number of scenarios and algorithms.


___

## Contributors
Mourad Khayati (mkhayati@exascale.info) and Zakhar Tymchenko (zakhar.tymchenko@unifr.ch).


___
## Award
Imputebench has received the VLDB 2020 Most Reproducible Paper [Award](https://vldb2020.org/vldb-2020-awards.html).

___

## Citation
```bibtex
@inproceedings{imputebench2020vldb,
 author    = {Mourad Khayati and Alberto Lerner and Zakhar Tymchenko and Philippe Cudr{\'{e}}{-}Mauroux},
 title     = {Mind the Gap: An Experimental Evaluation of Imputation of Missing Values Techniques in Time Series},
 booktitle = {Proceedings of the VLDB Endowment},
 volume    = {13},
 number    = {5},
 year      = {2020}
}
```



<!---
### Optional commands

 | Argument | Description | Options | Remarks |
 | -------- | -------- | -------- | -------- | 
 | -nort | Doesn't test runtime of the algorithms | n/a | - |
 | -noprec | Doesn't test precision of the algorithms | n/a | - |
 | -novis | Doesn't render plots which show the recovered block | n/a | - |
 | -out [folder] | Redirects results from default folder to a custom one | [folder] : a folder to store the results | Folder will be created is it doesn't exist. Existing files might be overwritten. |
 --->
