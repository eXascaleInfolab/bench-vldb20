## Configure parameters

To change the # of epochs run testing framework use `-algx`. Example:

- `mono TestingFramework.exe -algx brits 150 -d airq -scen miss_perc`

this will run BRITS with 150 epochs (default is 100)

To change the batch size go to `TestingFramework/AlgoIntegration/BRITSAlgorithm.cs` and change batch size in CLI arguments on lines 51 and 67.
