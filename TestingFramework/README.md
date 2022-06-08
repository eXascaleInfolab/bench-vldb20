# Description of recovery scenarios

## Setup
- all data is z-score normalized
- normalization occurs before trimming the length for tests where N_test < N
- when percentage or division is mentioned, the result is floored down to nearest integer

## Scenarios
N = lentgh of time series

M = number of time series

W = 10% * N

miss_perc:
- N = max; M = max;
- size of a single single block varies between 10% and 80% of the series, position: at 5% of 1st series from the top.

ts_length:
- M = max; N varies between 20% and 100% of the series;
- size of a single missing block is 10% * N, position: at 5% of 1st series from the top.

ts_nbr:
- N = max; M = varies from min(10% of columns, 4) to 100% of columns;
- size of a single missing block is W, position: at 5% of 1st series from the top.

miss_disj:
- N = max; M = max;
- Missing data - size = N/M, position: in each time series = column_index * size

miss_over:
- N = max; M = max;
- Missing data - size = 2 * N/M for all columns except last; last column = N/M; position: in each time series = column_index * (size/2)

mcar\*:
- N = max; M = max;
- Missing data - 10 to 100% time series are incomplete; Missing blocks - size = 10, removed from a random series at a random position until a total of W of all points of time series are missing.

blackout:
- N = max; M = max;
- Missing data - 10 to 100 rows in each time series, position: at 5% of all series from the top.

\* mcar scenario uses random number generator with fixed seed and will produce the same blocks every run
