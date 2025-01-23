# ImputeBench: A Comprehensive Benchmark for Time Series Imputation Techniques

ImputeBench implements over 15 advanced imputation techniques for missing blocks in time series. It evaluates their precision and runtime on various real-world time series datasets using different recovery scenarios. Technical details can be found in our PVLDB 2020 paper: <a href = "http://www.vldb.org/pvldb/vol13/p768-khayati.pdf">Mind the Gap: An Experimental Evaluation of Imputation of Missing Values Techniques in Time Series </a>. The benchmark can be easily extended with new algorithms (C/C++, Python, or Matlab), datasets, and scenarios.

- **Initial Algorithms**: The original benchmark implements the following algorithms (in C++):
  - [CDRec](https://rdcu.be/b32bv): Scalable Recovery of Missing Blocks in Time Series with High and Low Cross-Correlations, KAIS'20
  - [DynaMMo](https://dl.acm.org/doi/10.1145/1557019.1557078): DynaMMo: mining and summarization of coevolving sequences with missing values, KDD'09
  - [GROUSE](http://proceedings.mlr.press/v51/zhang16b.html): Global Convergence of a Grassmannian Gradient Descent Algorithm for Subspace Estimation, PMLR'16
  - [ROSL](https://ieeexplore.ieee.org/abstract/document/6909890): Robust Orthonormal Subspace Learning: Efficient Recovery of Corrupted Low-Rank Matrices, CVPR'14
  - [SoftImpute](https://www.jmlr.org/papers/v11/mazumder10a.html): Spectral Regularization Algorithms for Learning Large Incomplete Matrices, JMLR'10
  - [SPIRIT](https://dl.acm.org/doi/10.5555/1083592.1083674)\*: Streaming pattern discovery in multiple time-series, VLDB'05
  - [STMVL](https://www.ijcai.org/Proceedings/16/Papers/384.pdf): ST-MVL: Filling Missing Values in Geo-Sensory Time Series Data, IJCAI'16
  - [SVDImpute](https://academic.oup.com/bioinformatics/article/17/6/520/272365): Missing value estimation methods for DNA microarrays, BIOINFORMATICS'01
  - [SVT](https://epubs.siam.org/doi/10.1137/080738970?mobileUi=0): A Singular Value Thresholding Algorithm for Matrix Completion, SIAM J. OPTIM'10
  - [TeNMF](http://proceedings.mlr.press/v70/mei17a.html): Nonnegative Matrix Factorization for Time Series Recovery From a Few Temporal Aggregates, PMLR'17
  - [TRMF](https://papers.nips.cc/paper/6160-temporal-regularized-matrix-factorization-for-high-dimensional-time-series-prediction.pdf): Temporal Regularized Matrix Factorization for High-dimensional Time Series Prediction, NIPS'16
  - [TKCM](https://openproceedings.org/2017/conf/edbt/paper-112.pdf)\*: Continuous Imputation of Missing Values in Streams of Pattern-Determining Time Series, EDBT'17
    
- **Recently Integrated Algorithms**: We recently expanded the original benchmark with new algorithms (in their original implementation):
  - [MPIN](https://www.vldb.org/pvldb/vol17/p345-li.pdf): Missing Value Imputation for Multi-attribute Sensor Data Streams via Message Propagation, PVLDB'24
  - [PriSTI](https://ieeexplore.ieee.org/document/10184808): PriSTI: A Conditional Diffusion Framework for Spatiotemporal Imputation, ICDE'23
  - [GRIN](https://openreview.net/pdf?id=kOu3-S3wJ7): Filling the G_ap_s: Multivariate Time Series Imputation by Graph Neural Networks, ICLR'22
  - [DeepMVI](http://vldb.org/pvldb/vol14/p2533-bansal.pdf): Missing Value Imputation on Multidimensional Time Series, PVLDB'21
  - [IIM](https://ieeexplore.ieee.org/document/8731351)\*: Learning Individual Models for Imputation, ICDE '19
  - [MRNN](https://ieeexplore.ieee.org/document/8485748)\*: Estimating Missing Data in Temporal Data Streams Using Multi-Directional Recurrent Neural Networks, Trans. On Bio Eng.'19
  - [BRITS](http://papers.nips.cc/paper/7911-brits-bidirectional-recurrent-imputation-for-time-series): BRITS: Bidirectional Recurrent Imputation for Time Series, NeurIPS'18
  - [SSA](https://dl.acm.org/doi/10.1145/3287319)\*: Model Agnostic Time Series Analysis via Matrix Estimation, Meas. Anal. Comput. Syst'18

- **Algorithms under Integration**:
  - [DAMR](https://dl.acm.org/doi/abs/10.1145/3589333): Dynamic Adjacency Matrix Representation Learning for Multivariate Time Series Imputation, SIGMOD'23
  - [HKMF-T](https://ieeexplore.ieee.org/document/8979178): HKMF-T: Recover From Blackouts in Tagged Time Series With Hankel Matrix Factorization, TKDE'21
  - [NAOMI](https://proceedings.neurips.cc/paper_files/paper/2019/file/50c1f44e426560f3f2cdcb3e19e39903-Paper.pdf): NAOMI: Non-Autoregressive Multiresolution Sequence Imputation, NeurIPS'19
  - [E2EGAN](https://www.ijcai.org/proceedings/2019/429): E²GAN: End-to-End Generative Adversarial Network for Multivariate Time Series Imputation, IJCAI'19

- **Datasets**: All the datasets used in this benchmark can be found [here](https://github.com/eXascaleInfolab/bench-vldb20/tree/master/Datasets).
- **Missingness Patterns**: The full list of recovery scenarios can be found [here](https://github.com/eXascaleInfolab/bench-vldb20/blob/master/TestingFramework/README.md).
- **Notes**: The algorithms marked with <sup>\*</sup> cannot handle multiple incomplete time series. They produce results only for the following scenarios: `miss_perc`, `ts_length`, and ` ts_nbr`.

 [**Prerequisites**](#prerequisites) | [**Build**](#build) | [**Execution**](#execution) | [**Extension**](#extension) | [**Contributors**](#contributors) | [**Award**](#award) | [**Citation**](#citation)


---

## Prerequisites

- Ubuntu 20 or Ubuntu 22 (including Ubuntu derivatives, e.g., Xubuntu) or the same distribution under WSL.
- Clone this repository

---

## Build
- Install mono from https://www.mono-project.com/download/stable/ and reboot your terminal.

- Build the Testing Framework using the installation script located in the root folder 

```bash
    $ sh install_linux.sh
```

<!--- 
This will install a virtual environment (`bench-env`) under which the packages for this version will be installed. To use algorithms built using this Python version (DeepMVI, MPIN), you need to activate this virtual environment (example provided in next section).
-->

---

## Execution

```bash
    $ cd TestingFramework/bin/Debug/
    $ mono TestingFramework.exe [arguments]
```

### Arguments

| -alg     | -d          | -scen     |
| -------- | ----------- | --------- |
| cdrec    | airq        | miss_perc |
| dynammo  | bafu        | ts_length |
| grouse   | chlorine    | ts_nbr    |
| rosl     | climate     | miss_disj |
| softimp  | drift10     | miss_over |
| svdimp   | electricity | mcar      |
| svt      | meteo       | blackout  |
| stmvl    | temp        | _all_     |
| spirit   | bafu_red    |           |
| tenmf    | drift10_red |           |
| tkcm     | _all_       |           |
| trmf     |             |           |
| _all_    |             |           |
| -------- | --------    | --------  |
| New algs |             |           |
| -------- | --------    | --------  |
| ssa      |             |           |
| m-rnn    |             |           |
| brits    |             |           |
| deepmvi  |             |           |
| mpin     |             |           |
| pristi   |             |           |
| iim      |             |           |




### Results

All results and plots will be added to the `Results` folder. The accuracy results of all algorithms will be sequentially added for each scenario and dataset to: `Results/.../.../error/`. The runtime results of all algorithms will be added to: `Results/.../.../runtime/`. The plots of the recovered blocks will be added to the folder `Results/.../.../recovery/plots/`.

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

**Warning**: Running the whole benchmark takes a sizeable amount of time (up to 4 days, depending on the hardware) and produces up to 15GB of output files with all recovered data and plots unless stopped early.

5. Create patterns of missing blocks on one complete dataset (airq) using one scenario (missing percentage)

```bash
    $ mono TestingFramework.exe -alg mvexport -d airq -scen miss_perc
```

**Note**: You must run each scenario separately on one or multiple datasets. Each time you execute one scenario, the `Results` folder will be overwritten with the new files.

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

- If you want to run some algorithms with default parameters and some with customized ones, you can use `-alg` and `-algx` together. For example, you can run stmvl algorithm with default parameter and cdrec algorithm with a reduction value of 4 on the airq dataset by varying the sequence length as follows:

```bash
    $ mono TestingFramework.exe -alg stmvl -algx cdrec 4 -d airq -scen ts_nbr
```

**Remark**: The command `-algx` cannot be executed in a group and thus must precede the name of each algorithm.

---

## Executing New Algorithms


- To evaluate the newly integrated algorithms, please install the following Python packages (takes several minutes):

```bash
    $ sh install_extra.sh
```

- Activate the virtual environment and execute the new algorithms from the table above

```bash
    $ source bench-env/bin/activate
    $ mono TestingFramework.exe [arguments]
```


## Extension

- To add new algorithms:
  - C/C++: https://github.com/eXascaleInfolab/bench-vldb20/tree/master/NewAlgorithms/cpp
  - Python: https://github.com/eXascaleInfolab/bench-vldb20/tree/master/NewAlgorithms/python
  - Matlab: https://github.com/eXascaleInfolab/bench-vldb20/tree/master/NewAlgorithms/matlab
- To add new datasets:
  - import the file to `TestingFramework/bin/Debug/data/{name}/{name}_normal.txt` (`name` is the name of your dataset).
  - **Requirements**: rows>= 1'000, columns>= 10, column separator: empty space, row separator: newline
  - **Note**: the benchmark can also run with rows>= 100 and columns>= 5 but with a limited number of scenarios and algorithms.

---

## Contributors

- Mourad Khayati (mkhayati@exascale.info)
-  Zakhar Tymchenko (zakhar.tymchenko@unifr.ch).

---

## Award

ImputeBench has received the VLDB 2020 Best Experiments and Analysis Paper [Award](https://vldb2020.org/vldb-2020-awards.html).

---

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
