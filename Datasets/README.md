# Imputation of Missing Values in Time Series Benchmark => Datasets

Folders named after dataset's code contains code_normal.txt file which is a matrix used for the dataset in tests. Subfolders raw/ raw_timeseries/ etc. contain original data and/or processing steps applied before getting code_normal.txt.

## BAFU

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

