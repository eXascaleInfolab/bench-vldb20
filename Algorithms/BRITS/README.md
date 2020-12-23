## Configure parameters

- You can parameterize the # of epochs of BRITS using the command `-algx`. For example, to run BRITS on the airq dataset using the missing percentage scenario with 150 epochs (instead of 100--default) use

  - `mono TestingFramework.exe -algx brits 150 -d airq -scen miss_perc`

- You can also update the batch size by changing the CLI arguments on lines 51 and 67 in `TestingFramework/AlgoIntegration/BRITSAlgorithm.cs`. 
