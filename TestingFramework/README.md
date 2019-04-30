# Imputation of Missing Values in Time Series Benchmark => TestingFramework

For dependencies and setup look in the README file in root of repository.

# Description of testing scenarios

## Basics
- all data is z-score normalized
- normalization occurs before trimming the length for tests where N_test < N
- all tests exist in runtime and precision variants

### Recovery
missingpercentage/{dataset}:
- fixed data length (size=max) and column count (size=max)
- varying missing block size [10% to 80%, step 10%] starting at col 0, line=indexof(5%)

length/{dataset}:
- fixed missing block length (size=10% of total length), start at col 0, line=indexof(5%)
- fixed column count (size=12 or size=4, depending on the dataset)
- varying total length of the time series [200 to 2K, step 200]

columns/{dataset}:
- fixed data length (size=max) and missing block (size=10% of total length), starting at col 0, line=indexof(5%)
- varying the amount of columns [10% to 100% | but only entried with at least 4 columns]

TBA more
