# Imputation of Missing Values in Time Series Benchmark => TestingFramework

For dependencies and setup look in the README file in root of repository.

# Description of testing scenarios

## Basics
- all data is z-score normalized
- normalization occurs before trimming the length for tests where N_test < N
- all tests exist in runtime and precision variants
- when percentage or division is mentioned, the result is floored down to nearest integer

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

blackout/{dataset}:
- fixed data length (size=max) and column count (size=max)
- varying the amount of rows that are missing from all times series [10 to 100, step 10] starting at line=indexof(5%)

fullcolumn/{dataset}:
- fixed data length (size=max) and column count (size=max)
- varying the amount of columns [1 to 10] where all data points are missing, columns removed starting from the last one

multicol-disjoint/{dataset}:
- fixed data length (size=max) and column count (size=max)
- varying the amount of columns [10% to 100%, step 10%] that contain a missing block; if a column is containing a missing block it's of size = N/M, starting at line=(col_idx * size)

multicol-overlap/{dataset}:
- fixed data length (size=max) and column count (size=max)
- varying the amount of columns [10% to 100%, step 10%] that contain a missing block; if a column is containing a missing block it's of size = 2*N/M (except the last column at 100%, for this one the size is N/M), starting at line=(col_idx * size/2)

mcar/{dataset}:
- fixed data length (size=max) and column count (size=max)
- varying the percentage [10% to 100%, step 10%] of columns that will contain missing blocks; missing block have a static size=10 are introduced into involved time series at random until a total of 10% of all values from them are removed; removal is uniform except it ensures at least one element in each time series is left; PRNG seed is fixed so between runs the same elements are removed
