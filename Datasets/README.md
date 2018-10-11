# InCD_benchmark => Datasets

Folders named after dataset's code contains code_normal.txt file which is a matrix used for the dataset in tests. Subfolders raw/ raw_timeseries/ etc. contain original data and/or processing steps applied before getting code_normal.txt.

## Baseball

| Data info |  |
| -------- | -------- |
| Dataset codename | BBall/bball |
| Dataset name | Explaining Success in Baseball, The Local Correlation Approach |
| Url | https://www2.stetson.edu/~jrasp/research/localcorrelation.htm |
| Url item | Example3.csv|
|  |  |
| Multivariate series | rows are identified by a Tuple<Year,Team> |
| Time granularity | 1 year (1921 to 2009), consistent |
| Teams/year | ~21, inconsistent |
|  |  |
| Dataset dimensions | N=1906, M=4 |

#### Correlation matrix
| T1 | T2 | T3 | T4 |
| -------- |  -------- |  -------- |  -------- |
|1.00 |-0.53 |0.26 |0.49|
|-0.53 |1.00 |-0.04 |0.36|
|0.26 |-0.04 |1.00 |0.17|
|0.49 |0.36 |0.17 |1.00|


## Hydrology

| Data info |  |
| -------- | -------- |
| Dataset codename | BAFU/bafu |
| Dataset name | Hydrological data across multiple stations |
| Url/source | https://www.bafu.admin.ch/bafu/en/home.html |
|  |  |
| Granularity | inconsistent, average distance between measurements d <= 36 minutes +/- variance |
| Observations | spans years 1974 to 2015 |
|  |  |
| Dataset dimensions | N=85203 M=12 |


#### Correlation matrix
| T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 | T9 | T10 | T11 | T12
| -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- | -------- |
|1.00 |0.75 |0.13 |0.66 |0.89 |0.80 |0.79 |0.59 |0.91 |0.61 |0.03 |0.96|
|0.75 |1.00 |0.12 |0.66 |0.68 |0.84 |0.83 |0.56 |0.79 |0.68 |0.03 |0.71|
|0.13 |0.12 |1.00 |0.13 |0.13 |0.15 |0.14 |0.12 |0.12 |0.12 |0.36 |0.13|
|0.66 |0.66 |0.13 |1.00 |0.67 |0.78 |0.70 |0.84 |0.61 |0.86 |0.04 |0.65|
|0.89 |0.68 |0.13 |0.67 |1.00 |0.78 |0.76 |0.63 |0.82 |0.59 |0.03 |0.88|
|0.80 |0.84 |0.15 |0.78 |0.78 |1.00 |0.93 |0.70 |0.76 |0.71 |0.04 |0.77|
|0.79 |0.83 |0.14 |0.70 |0.76 |0.93 |1.00 |0.62 |0.78 |0.65 |0.03 |0.75|
|0.59 |0.56 |0.12 |0.84 |0.63 |0.70 |0.62 |1.00 |0.53 |0.70 |0.03 |0.59|
|0.91 |0.79 |0.12 |0.61 |0.82 |0.76 |0.78 |0.53 |1.00 |0.61 |0.03 |0.86|
|0.61 |0.68 |0.12 |0.86 |0.59 |0.71 |0.65 |0.70 |0.61 |1.00 |0.06 |0.59|
|0.03 |0.03 |0.36 |0.04 |0.03 |0.04 |0.03 |0.03 |0.03 |0.06 |1.00 |0.04|
|0.96 |0.71 |0.13 |0.65 |0.88 |0.77 |0.75 |0.59 |0.86 |0.59 |0.04 |1.00|


## Air quality

| Data info |  |
| -------- | -------- |
| Dataset codename | airq |
| Dataset name | Air Quality |
| Url | https://archive.ics.uci.edu/ml/datasets/Air+Quality |
| Source | Saverio De Vito (saverio.devito '@' enea.it), ENEA - National Agency for New Technologies, Energy and Sustainable Economic Development |
|  |  |
| Time granularity | hourly |
|  |  |
| Dataset dimensions | N=2947 M=500+ |
| Remarks | Only a subset of columns is considered: columns 4 to 15 (12 total) |


#### Correlation matrix
for M=12, j in [[4...15]] wrt original time series

| T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 | T9 | T10 | T11 | T12
| -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- | -------- |
|1.00|0.91|0.89|0.99|0.90|0.89|0.98|0.91|0.90|-0.96|-0.87|-0.83|
|0.91|1.00|0.87|0.90|0.99|0.87|0.89|0.94|0.88|-0.91|-0.96|-0.78|
|0.89|0.87|1.00|0.89|0.87|0.99|0.88|0.86|0.91|-0.86|-0.81|-0.94|
|0.99|0.90|0.89|1.00|0.90|0.88|0.97|0.90|0.89|-0.95|-0.87|-0.83|
|0.90|0.99|0.87|0.90|1.00|0.87|0.88|0.93|0.88|-0.90|-0.95|-0.78|
|0.89|0.87|0.99|0.88|0.87|1.00|0.87|0.86|0.90|-0.86|-0.81|-0.92|
|0.98|0.89|0.88|0.97|0.88|0.87|1.00|0.89|0.88|-0.93|-0.86|-0.83|
|0.91|0.94|0.86|0.90|0.93|0.86|0.89|1.00|0.87|-0.90|-0.87|-0.77|
|0.90|0.88|0.91|0.89|0.88|0.90|0.88|0.87|1.00|-0.89|-0.83|-0.80|
|-0.96|-0.91|-0.86|-0.95|-0.90|-0.86|-0.93|-0.90|-0.89|1.00|0.86|0.78|
|-0.87|-0.96|-0.81|-0.87|-0.95|-0.81|-0.86|-0.87|-0.83|0.86|1.00|0.75|
|-0.83|-0.78|-0.94|-0.83|-0.78|-0.92|-0.83|-0.77|-0.80|0.78|0.75|1.00|


## Climate data

| Data info |  |
| -------- | -------- |
| Dataset codename | climate |
| Dataset name | Aggregated and Processed data collection for climate change attribution |
| Url | http://www-bcf.usc.edu/~liu32/data.html |
| Url item | NA-1990-2002-Monthly.csv |
|  |  |
| Time granularity | 1 month, consistent |
|  |  |
| Dataset dimensions | N=19500 M=16 |
| Remarks | Only a subset of columns is considered |
| | WRT full time series the 12 columns are in the following order: |
| | [7, 12-14, 3-6, 8-11] with 12-14 being moved to the beginning to make a varying M test possible (better cor values) |
| | This allows for a time series on the same dataset with overall higher correlation with the reference series. |
| | The correlation vector for studied TS is now: |
| | X = { 1.00, 0.84, 0.77, 0.55, -0.38, 0.29, 0.11, -0.25, 0.43, -0.75, -0.10, 0.88 } |
| | So max { x_i, s.t. i >= 2 } = 0.88 which is reasonably big, but still less than 0.90 |

#### Correlation matrix
for the first M=16


| T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 | T9 | T10 | T11 | T12 | T13 | T14 | T15 | T16 |
| -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- | -------- | -------- | -------- | -------- | -------- |
|1.00|0.69|0.26|-0.26|-0.01|0.15|-0.16|-0.05|0.16|-0.08|-0.17|-0.17|-0.18|-0.04|-0.07|-0.05|
|0.69|1.00|0.54|-0.48|0.09|0.34|-0.29|-0.00|0.34|-0.28|-0.36|-0.40|-0.42|-0.40|-0.38|-0.33|
|0.26|0.54|1.00|0.07|0.21|0.48|-0.38|0.04|0.41|-0.30|-0.46|-0.49|-0.51|-0.37|-0.27|-0.23|
|-0.26|-0.48|0.07|1.00|0.05|-0.10|0.29|0.16|-0.30|0.11|0.32|0.33|0.33|0.45|0.51|0.46|
|-0.01|0.09|0.21|0.05|1.00|0.61|0.11|0.65|0.05|-0.56|-0.05|-0.14|-0.22|-0.20|0.00|0.06|
|0.15|0.34|0.48|-0.10|0.61|1.00|-0.25|0.25|0.50|-0.69|-0.47|-0.57|-0.65|-0.59|-0.41|-0.32|
|-0.16|-0.29|-0.38|0.29|0.11|-0.25|1.00|0.43|-0.75|-0.10|0.88|0.84|0.77|0.55|0.63|0.54|
|-0.05|-0.00|0.04|0.16|0.65|0.25|0.43|1.00|-0.29|-0.39|0.31|0.24|0.16|0.02|0.21|0.19|
|0.16|0.34|0.41|-0.30|0.05|0.50|-0.75|-0.29|1.00|-0.21|-0.92|-0.92|-0.90|-0.72|-0.75|-0.68|
|-0.08|-0.28|-0.30|0.11|-0.56|-0.69|-0.10|-0.39|-0.21|1.00|0.12|0.28|0.42|0.50|0.32|0.28|
|-0.17|-0.36|-0.46|0.32|-0.05|-0.47|0.88|0.31|-0.92|0.12|1.00|0.98|0.95|0.73|0.77|0.67|
|-0.17|-0.40|-0.49|0.33|-0.14|-0.57|0.84|0.24|-0.92|0.28|0.98|1.00|0.98|0.79|0.79|0.70|
|-0.18|-0.42|-0.51|0.33|-0.22|-0.65|0.77|0.16|-0.90|0.42|0.95|0.98|1.00|0.83|0.80|0.70|
|-0.04|-0.40|-0.37|0.45|-0.20|-0.59|0.55|0.02|-0.72|0.50|0.73|0.79|0.83|1.00|0.94|0.90|
|-0.07|-0.38|-0.27|0.51|0.00|-0.41|0.63|0.21|-0.75|0.32|0.77|0.79|0.80|0.94|1.00|0.97|
|-0.05|-0.33|-0.23|0.46|0.06|-0.32|0.54|0.19|-0.68|0.28|0.67|0.70|0.70|0.90|0.97|1.00|

## Meteo Suisse data

| Data info |  |
| -------- | -------- |
| Dataset codename | meteo |
| Dataset source | Federal Office of Meteorology and Climatology, MeteoSwiss |
|  | Operation Center 1 |
|  | Postfach 257 |
|  | 8058 Zürich-Flughafen |
|  |  |
| Granularity | 10 minutes, consistent save for missing values in the raw TS |
|  |  |
| Dataset dimensions | N=192069 M=4 |
| Remarks | Only a subset of rows is considered |
|  | TBA |

#### Dataset description

##### Stations
| stn | Name| Parameter| Data source | Longitude/Latitude | Coordinates [km] | Elevation [m] |
| -------- | --------| --------| -------- | -------- | -------- | -------- |
| ZHUST| Aatal Höhe| erssurs0 | Kanton Zürich; Tiefbauamt | 8°45'/47°21' | 698612/244769 | 490 |

##### Parameters

| | Unit | Description |
| -------- | -------- | -------- |
| erssurs0 | mm | null |
| merssurs0 | Code | Mutation information on erssurs0 |

Observation interval for hourly values, unless otherwise indicated in the parameter description: HH  = (HH-1):41 - HH:40
Example: 13 = observation period 12:41 to 13:40

#### Correlation matrix
| T1 | T2 | T3 | T4 |
| -------- |  -------- |  -------- |  -------- |
|1.00 |0.90 |0.82 |-0.12|
|0.90 |1.00 |0.95 |-0.53|
|0.82 |0.95 |1.00 |-0.56|
|-0.12 |-0.53 |-0.56 |1.00|

## Gas Sensor Array Drift Dataset at Different Concentrations

| Data info |  |
| -------- | -------- |
| Dataset codename | drift6 and drift10 |
| Dataset creators |  |
| Alexander Vergara | (vergara '@' ucsd.edu) |
| BioCircutis Institute | University of California San Diego |
|  | San Diego, California, USA |
| Donors of the Dataset: |  |
| Alexander Vergara | (vergara '@' ucsd.edu) |
| Jordi Fonollosa | (fonollosa '@'ucsd.edu) |
| Irene Rodriguez-Lujan | (irrodriguezlujan '@' ucsd.edu) |
| Ramon Huerta | (rhuerta '@' ucsd.edu)  |
|  |  |
| Dataset dimensions | batch 6 { N=2300 M=128 } batch 10 { N=3600 M=128 } |
| Remarks | only batch 6 and batch 10 are taken from the dataset |
| Url | https://archive.ics.uci.edu/ml/datasets/Gas+Sensor+Array+Drift+Dataset+at+Different+Concentrations |

#### Dataset description

Only a subset of columns was taken to construct M=12 datasets from both drift6 and drift10.

- Batch 6: columns # 3,  2,  7,  41, 42, 34, 16, 51, 98, 66, 74, 10
- Batch 10: columns # 1, 6, 10, 28, 18, 58, 17, 21, 52, 124, 110, 36

#### Correlation matrices

Batch 6

| T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 | T9 | T10 | T11 | T12
| -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- | -------- |
|1.00 | 0.67 | -0.83 | 0.63 | 0.48 | 0.58 | -0.86 | 0.20 | 0.37 | 0.68 | 0.70 | 0.64|
|0.67 | 1.00 | -0.53 | 0.58 | 0.69 | 0.72 | -0.40 | 0.31 | 0.63 | 0.92 | 0.90 | 0.96|
|-0.83 | -0.53 | 1.00 | -0.48 | -0.34 | -0.44 | 0.78 | -0.14 | -0.25 | -0.56 | -0.58 | -0.49|
|0.63 | 0.58 | -0.48 | 1.00 | 0.83 | 0.85 | -0.48 | 0.69 | 0.79 | 0.65 | 0.68 | 0.57|
|0.48 | 0.69 | -0.34 | 0.83 | 1.00 | 0.98 | -0.26 | 0.67 | 0.98 | 0.69 | 0.69 | 0.72|
|0.58 | 0.72 | -0.44 | 0.85 | 0.98 | 1.00 | -0.36 | 0.63 | 0.95 | 0.74 | 0.75 | 0.75|
|-0.86 | -0.40 | 0.78 | -0.48 | -0.26 | -0.36 | 1.00 | -0.08 | -0.16 | -0.46 | -0.50 | -0.40|
|0.20 | 0.31 | -0.14 | 0.69 | 0.67 | 0.63 | -0.08 | 1.00 | 0.73 | 0.40 | 0.41 | 0.29|
|0.37 | 0.63 | -0.25 | 0.79 | 0.98 | 0.95 | -0.16 | 0.73 | 1.00 | 0.64 | 0.64 | 0.66|
|0.68 | 0.92 | -0.56 | 0.65 | 0.69 | 0.74 | -0.46 | 0.40 | 0.64 | 1.00 | 0.99 | 0.90|
|0.70 | 0.90 | -0.58 | 0.68 | 0.69 | 0.75 | -0.50 | 0.41 | 0.64 | 0.99 | 1.00 | 0.87|
|0.64 | 0.96 | -0.49 | 0.57 | 0.72 | 0.75 | -0.40 | 0.29 | 0.66 | 0.90 | 0.87 | 1.00|

Batch 10

| T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 | T9 | T10 | T11 | T12
| -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- | -------- |
|1.00 | -0.75 | 0.57 | 0.52 | 0.64 | 0.71 | 0.77 | 0.64 | 0.70 | 0.78 | -0.55 | 0.63|
|-0.75 | 1.00 | -0.46 | -0.43 | -0.54 | -0.59 | -0.59 | -0.50 | -0.55 | -0.61 | 0.55 | -0.51|
|0.57 | -0.46 | 1.00 | 0.40 | 0.47 | 0.55 | 0.46 | 0.44 | 0.48 | 0.53 | -0.37 | 0.51|
|0.52 | -0.43 | 0.40 | 1.00 | 0.87 | 0.85 | 0.88 | 0.96 | 0.95 | 0.90 | -0.87 | 0.90|
|0.64 | -0.54 | 0.47 | 0.87 | 1.00 | 0.99 | 0.94 | 0.87 | 0.90 | 0.86 | -0.86 | 0.75|
|0.71 | -0.59 | 0.55 | 0.85 | 0.99 | 1.00 | 0.94 | 0.87 | 0.90 | 0.88 | -0.84 | 0.77|
|0.77 | -0.59 | 0.46 | 0.88 | 0.94 | 0.94 | 1.00 | 0.92 | 0.95 | 0.93 | -0.86 | 0.82|
|0.64 | -0.50 | 0.44 | 0.96 | 0.87 | 0.87 | 0.92 | 1.00 | 0.97 | 0.94 | -0.85 | 0.89|
|0.70 | -0.55 | 0.48 | 0.95 | 0.90 | 0.90 | 0.95 | 0.97 | 1.00 | 0.98 | -0.86 | 0.92|
|0.78 | -0.61 | 0.53 | 0.90 | 0.86 | 0.88 | 0.93 | 0.94 | 0.98 | 1.00 | -0.80 | 0.91|
|-0.55 | 0.55 | -0.37 | -0.87 | -0.86 | -0.84 | -0.86 | -0.85 | -0.86 | -0.80 | 1.00 | -0.84|
|0.63 | -0.51 | 0.51 | 0.90 | 0.75 | 0.77 | 0.82 | 0.89 | 0.92 | 0.91 | -0.84 | 1.00|

## Temperature dataset

| Data info |  |
| -------- | -------- |
| Dataset codename | temp |
| Dataset source | China Meteorological Administration |
| Url | http://www.cma.gov.cn |
|  |  |
| Dataset dimensions | N=19538 M=428 |
| Granularity | daily, consistent |
| Remarks | from every time series the value taken is daily average |

#### Dataset description

Only a subset of columns was taken to construct M=12 dataset.

- columns # 1, 374, 299, 428, 417, 105, 240, 383, 222, 40, 105, 409

#### Correlation matrix

| T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 | T9 | T10 | T11 | T12
| -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- |  -------- | -------- |
|1.00 | 0.86 | 0.84 | 0.82 | 0.82 | 0.87 | 0.78 | 0.87 | 0.85 | 0.89 | 0.87 | 0.81|
|0.86 | 1.00 | 0.92 | 0.86 | 0.92 | 0.85 | 0.83 | 0.95 | 0.82 | 0.82 | 0.85 | 0.93|
|0.84 | 0.92 | 1.00 | 0.82 | 0.91 | 0.85 | 0.83 | 0.94 | 0.83 | 0.80 | 0.85 | 0.90|
|0.82 | 0.86 | 0.82 | 1.00 | 0.88 | 0.79 | 0.89 | 0.84 | 0.82 | 0.77 | 0.79 | 0.88|
|0.82 | 0.92 | 0.91 | 0.88 | 1.00 | 0.82 | 0.85 | 0.94 | 0.81 | 0.79 | 0.82 | 0.97|
|0.87 | 0.85 | 0.85 | 0.79 | 0.82 | 1.00 | 0.75 | 0.86 | 0.79 | 0.83 | 1.00 | 0.82|
|0.78 | 0.83 | 0.83 | 0.89 | 0.85 | 0.75 | 1.00 | 0.82 | 0.87 | 0.71 | 0.75 | 0.86|
|0.87 | 0.95 | 0.94 | 0.84 | 0.94 | 0.86 | 0.82 | 1.00 | 0.83 | 0.84 | 0.86 | 0.93|
|0.85 | 0.82 | 0.83 | 0.82 | 0.81 | 0.79 | 0.87 | 0.83 | 1.00 | 0.77 | 0.79 | 0.80|
|0.89 | 0.82 | 0.80 | 0.77 | 0.79 | 0.83 | 0.71 | 0.84 | 0.77 | 1.00 | 0.83 | 0.78|
|0.87 | 0.85 | 0.85 | 0.79 | 0.82 | 1.00 | 0.75 | 0.86 | 0.79 | 0.83 | 1.00 | 0.82|
|0.81 | 0.93 | 0.90 | 0.88 | 0.97 | 0.82 | 0.86 | 0.93 | 0.80 | 0.78 | 0.82 | 1.00|
