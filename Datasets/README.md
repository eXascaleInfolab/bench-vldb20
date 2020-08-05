# Imputation of Missing Values in Time Series Benchmark => Datasets

Folders named after dataset's code contains code_normal.txt file which is a matrix used for the dataset in tests. Subfolders raw/ raw_timeseries/ etc. contain original data and/or processing steps applied before getting code_normal.txt.

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
| Dataset dimensions | N=1000 M=10 |

## Chlorine

| Data info |  |
| -------- | -------- |
| Dataset codename | chlorine |
| Dataset name | Chlorine data |
| Url | https://www.epa.gov/research |
| Source | United States Environmental Protection Agency, EPANET |
|  | Prof. Jeanne M. VanBriesen |
|  |  |
| Time granularity | 5 minutes |
|  |  |
| Dataset dimensions | N=1000 M=50 |

## Gas Sensor Array Drift Dataset at Different Concentrations

| Data info |  |
| -------- | -------- |
| Dataset codename | drift10 |
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
| Time granularity | 6 hours |
| Dataset dimensions | N=1000 M=100 } |
| Remarks | only batch 10 is taken from the dataset |
| Url | https://archive.ics.uci.edu/ml/datasets/Gas+Sensor+Array+Drift+Dataset+at+Different+Concentrations |

## Climate data

| Data info |  |
| -------- | -------- |
| Dataset codename | climate |
| Dataset name | Aggregated and Processed data collection for climate change attribution |
| Url | https://viterbi-web.usc.edu/~liu32/data.html |
| Url item | NA-1990-2002-Monthly.csv |
|  |  |
| Time granularity | 1 month |
|  |  |
| Dataset dimensions | N=5000 M=10 |

## Electricity

| Data info |  |
| -------- | -------- |
| Dataset codename | electricity |
| Dataset name | Electricity Load Diagrams |
| Url | https://archive.ics.uci.edu/ml/datasets/ElectricityLoadDiagrams20112014 |
| Dataset creator | Artur Trindade, artur.trindade '@' elergone.pt |
|  | Elergone, NORTE-07-0202-FEDER-038564 |
|  |  |
| Time granularity | 15 minutes |
|  |  |
| Dataset dimensions | N=5000 M=20 |

## Temperature dataset

| Data info |  |
| -------- | -------- |
| Dataset codename | temp |
| Dataset source | China Meteorological Administration |
| Url | http://www.cma.gov.cn |
|  |  |
| Dataset dimensions | N=5000 M=50 |
| Granularity | daily |
| Remarks | from every time series the value taken is daily average |

## Meteo Suisse data

| Data info |  |
| -------- | -------- |
| Dataset codename | meteo |
| Dataset source | Federal Office of Meteorology and Climatology, MeteoSwiss |
|  | Operation Center 1 |
|  | Postfach 257 |
|  | 8058 Zürich-Flughafen |
|  |  |
| Granularity | 10 minutes |
|  |  |
| Dataset dimensions | N=10000 M=10 |
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

## BAFU

| Data info |  |
| -------- | -------- |
| Dataset codename | BAFU/bafu |
| Dataset name | Hydrological data across multiple stations |
| Url/source | https://www.bafu.admin.ch/bafu/en/home.html |
|  |  |
| Granularity | 30 minutes |
| Observations | spans years 1974 to 2015 |
|  |  |
| Dataset dimensions | N=50000 M=10 |
