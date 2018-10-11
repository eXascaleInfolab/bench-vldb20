# InCD_benchmark => TestingFramework

For dependencies and setup look in the README file in root of repository.

# Description of testing scenarios

## Basics
- all data is z-score normalized
- normalization occurs before trimming the length for tests where N_test < N
- all tests exist in runtime and precision variants

### Recovery
missingpercentage/{dataset}:
- fixed data length (size=1k)
- fixed column count (size=12 or size=4, depending on the dataset)
- varying missing block size [10% to 80%, step 10%] starting at col 0, line=indexof(5%)

length/{dataset}:
- fixed missing block length (size=100), start at col 0, line 50
- fixed column count (size=12 or size=4, depending on the dataset)
- varying total length of the time series [200 to 2K, step 200] (for bball = [200 to 1.8K, step 200])

columns/{dataset}:
- fixed data length (size=1k) and missing block (length=100), starting at col 0, line 50
- varying the amount of columns 4 to 12, only for datasets with M=12

### Continuous (disabled by default)
missingpercentage/{dataset}:
- fixed data length (size=1k)
- fixed column count (size=12 or size=4, depending on the dataset)
- varying missing block size [10% to 80%, step 10%] starting at col 0, line=1000-missingBlock.size

length/{dataset}:
- fixed missing block length (size=100), start at col 0, line=len-100
- fixed column count (size=12 or size=4, depending on the dataset)
- varying total length of the time series [200 to 2K, step 200] (for bball dataset = [200 to 1.8K, step 200])

columns/{dataset}:
- fixed data length (size=1k) and missing block (length=100), starting at col 0, line 900
- varying the amount of columns 4 to 12, only for datasets with M=12

### Streaming (disabled by default)

- only a subset of algorithms is running streaming test
    - inCD
    - TKCM
    - SPIRIT
- others don't have explicit streaming/update option that can improve performance compared to normal runtime test

missingpercentage/{dataset}:
- fixed data length (size=1k)
- fixed column count (size=12 or size=4, depending on the dataset)
- varying missing block size [10% to 80%, step 10%] starting at col 0, line=1000-missingBlock.size
- algorithms pre-process N - blockSize first, then new data is given and after that time is measured

length/{dataset}:
- fixed missing block length (size=1), start at col 0, line=len-100
- fixed column count (size=12 or size=4, depending on the dataset)
- varying total length of the time series [200 to 2K, step 200] (for bball dataset = [200 to 1.8K, step 200])
- algorithms pre-process N - blockSize first, then new data is given and after that time is measured

columns/{dataset}:
- fixed data length (size=1k) and missing block (length=100), starting at col 0, line 900
- varying the amount of columns 4 to 12, only for datasets with M=12
- algorithms pre-process N - blockSize first, then new data is given and after that time is measured
