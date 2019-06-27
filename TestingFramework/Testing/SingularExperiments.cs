using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using TestingFramework.Algorithms;

namespace TestingFramework.Testing
{
    public static class SingularExperiments
    {
        /// <summary>
        /// Compares algorithms' RMSE performance based on a test run
        /// </summary>
        /// <param name="codes">see Program.cs</param>
        /// <param name="codes_limited">see Program.cs</param>
        /// <param name="desination">stream writeline function, Console.WriteLine or another lambda that handles output</param>
        /// <param name="cmptype">foldername to compare with</param>
        public static void MsePerformanceReport(string[] codes, string[] codes_limited, Action<string> desination, string cmptype)
        {
            int equals = 0,
                almost = 0,
                algorithm1 = 0,
                algorithm1_big = 0,
                algorithm2 = 0,
                algorithm2_big = 0;

            string algorithm1name = "Alg1_reference";
            string algorithm2name = $"Alg2_{cmptype}";
            
            string part1 = "/home/zakhar/MVR/Project/cd_eval/reference/precision/";
            string part2 = $"/home/zakhar/MVR/Project/cd_eval/{cmptype}/precision/";
            
            const double eps = 1E-14;
            const double eps2 = 1E-7;
            const double eps_big = 0.05;

            var writer = new Utils.ContinuousWriter(desination);
            
            EnumMethods.AllExperimentTypes().Where(x => x != ExperimentType.Streaming)
                .Select(EnumMethods.ToLongString).ForEach(et =>
            {
                writer.WriteLine($"Entering Test Type = {et}");
                writer.Indent();
                
                EnumMethods.AllExperimentScenarios()
                    .Select(EnumMethods.ToLongString).ForEach(es =>
                {
                    writer.WriteLine($"Entering Test Scenario = {es}");
                    writer.Indent();
                    
                    string[] activeCodes = es == "columns" ? codes_limited : codes;
                    
                    activeCodes.ForEach(code =>
                    {
                        writer.WriteLine($"Dataset = {code}");
                        writer.Indent();
                        
                        string folder1 = part1 + et + "/" + es + "/" + code + "/error/results/values/rmse/";
                        string folder2 = part2 + et + "/" + es + "/" + code + "/error/results/values/rmse/";
                        
                        new[] { 2, 3 }.ForEach(k => // can be changed to a single value to do compare algorithms without sub-algorithms
                        {
                            string file1 = folder1 + $"RMSE_incd_k{k}.dat", // set both to an RMSE file of a correct algorithm
                                   file2 = folder2 + $"RMSE_incd_k{k}.dat";

                            string[] lines1 = File.ReadAllLines(file1).Skip(1).ToArray();
                            string[] lines2 = File.ReadAllLines(file2).Skip(1).ToArray();

                            IEnumerable<(int, double)> cases1 = lines1.Select(l => (Int32.Parse(l.Split(' ')[0]), Utils.ParseDouble(l.Split(' ')[1])));
                            IEnumerable<(int, double)> cases2 = lines2.Select(l => (Int32.Parse(l.Split(' ')[0]), Utils.ParseDouble(l.Split(' ')[1])));
                            
                            cases1.ForEach(c =>
                            {
                                (int tcase, double val1) = c;
                                double val2 = cases2.Where(c2 => c2.Item1 == tcase).Select(x => x.Item2).First();

                                double diff = val1 - val2;
                                
                                if (Math.Abs(diff) < eps)
                                {
                                    writer.WriteLine($"CD k={k}; case = {tcase}; EQUAL");
                                    equals++;
                                }
                                else if (Math.Abs(diff) < eps2)
                                {
                                    writer.WriteLine($"CD k={k}; case = {tcase}; ALMOST");
                                    almost++;
                                }
                                else if (diff < 0) // LOWER (NOT HIGHER) IS BETTER
                                {
                                    writer.WriteLine($"CD k={k}; case = {tcase}; {algorithm1name} BETTER; diff={diff:F7}; LOWER={val1}");
                                    algorithm1++;
                                    if (Math.Abs(diff) > eps_big) algorithm1_big++;
                                }
                                else
                                {
                                    writer.WriteLine($"CD k={k}; case = {tcase}; {algorithm2name} BETTER; diff={diff:F7}; LOWER={val2}");
                                    algorithm2++;
                                    if (Math.Abs(diff) > eps_big) algorithm2_big++;
                                }
                            });
                            
                            writer.WriteLine();
                        });
                        
                        writer.UnIndent();
                    });
                    
                    writer.UnIndent();
                    writer.WriteLine($"Exiting Test Scenario = {es}");
                    writer.WriteLine();
                });
                
                writer.UnIndent();
                writer.WriteLine($"Exiting Test Type = {et}");
            });

            writer.WriteLine();
            writer.WriteLine("REPORT:");
            writer.Indent();
            writer.WriteLine($"TOTAL VALUES: {equals+almost+algorithm1+algorithm2}");
            writer.WriteLine($"EQUAL (diff < {eps:E1}): {equals}");
            writer.WriteLine($"ALMOST (diff < {eps2:E1}): {almost}");
            
            writer.WriteLine();
            
            writer.WriteLine($"{algorithm1name} BETTER: {algorithm1}");
            writer.Indent();
            writer.WriteLine($"OF THOSE diff > {eps_big}: {algorithm1_big}");
            writer.UnIndent();
            
            writer.WriteLine($"{algorithm2name} BETTER: {algorithm2}");
            writer.Indent();
            writer.WriteLine($"OF THOSE diff > {eps_big}: {algorithm2_big}");
            writer.UnIndent();
            writer.UnIndent();
            writer.WriteLine("END REPORT");
        }

        /// <summary>
        /// A function that averages multiple runtime results with the currently active scenario and store them into #1's folder
        /// [WARNING] FUNCTION COMPLETELY OVERWRITES RESULT #1 WITH THE AVERAGE
        /// </summary>
        /// <param name="codes">see Program.cs</param>
        /// <param name="codesLimited">see Program.cs</param>
        /// <param name="max">number of runs</param>
        public static void AverageRTRuns(IEnumerable<string> codes, IEnumerable<string> codesLimited, int max)
        {
            string[] folders =
                Utils.ClosedSequence(1, max)
                    .Select(i => DataWorks.FolderPlotsRemoteBase + i + "/")
                    .ToArray();
            
            var cw = new Utils.ContinuousWriter();

            foreach (var et in EnumMethods.AllExperimentTypes())
            {
                cw.Indent();
                cw.WriteLine($"Entering {et.ToLongString()} experiment type...");
                
                IEnumerable<ExperimentScenario> scenarios =
                    et == ExperimentType.Continuous
                        ? EnumMethods.AllExperimentScenarios().Where(e => e.IsContinuous())
                        : EnumMethods.AllExperimentScenarios();
                
                foreach (var es in scenarios)
                {
                    IEnumerable<Algorithm> algos =
                        es.IsSingleColumn()
                            ? AlgoPack.ListAlgorithms
                            : AlgoPack.ListAlgorithmsMulticolumn;
                    
                    cw.Indent();
                    cw.WriteLine($"Entering {es.ToLongString()} experiment type...");
                    
                    IEnumerable<string> currentCodes = es.IsLimited()
                        ? codesLimited
                        : codes;
                    
                    foreach (string code in currentCodes)
                    {
                        cw.Indent();
                        cw.WriteLine($"Entering {code} dataset...");
                        
                        foreach (Algorithm alg in algos)
                        {
                            foreach (SubAlgorithm subAlg in alg.EnumerateSubAlgorithms())
                            {
                                cw.Indent();
                                cw.WriteLine($"Algorithm: {alg.AlgCode}, subalgorithm: {subAlg.Code}");
                                
                                var caseValues = new Dictionary<int, double>();
                                
                                // step 1: grab all values, split cases and put all of them to dictionary
                                foreach (string folder in folders)
                                {
                                    string currentSource = $"{folder}runtime/{et.ToLongString()}/{es.ToLongString()}/{code}/results/";
                                    if (!Directory.Exists(currentSource))
                                    {
                                        string errMsg = $"[Warning] Folder {currentSource} doesn't exist, aborting last operation.";
                                        
                                        cw.WriteLine(errMsg);
                                        Utils.DelayedWarnings.Enqueue(errMsg);
                                        
                                        goto label_end;
                                    }

                                    string file = currentSource + $"{subAlg.Code}_runtime.txt";
                                    
                                    IEnumerable<(int, double)> res = File.ReadAllLines(file)
                                        .Where(x => !String.IsNullOrEmpty(x)) //shouldn't be the case, but whatever, it's IO
                                        .Select(x => x.Split(' '))
                                        .Select(x =>
                                            (Int32.Parse(x[0]), Utils.ParseDouble(x[1]))
                                        );

                                    res.ForEach(x => {
                                        if (!caseValues.ContainsKey(x.Item1))
                                        {
                                            caseValues.Add(x.Item1, x.Item2);
                                        }
                                        else
                                        {
                                            caseValues[x.Item1] += x.Item2;
                                        }
                                    });
                                }

                                // step 2: dump the values into the first folder
                                //         while dividing them by folders.count
                                
                                string firstSource = $"{folders.First()}runtime/{et.ToLongString()}/{es.ToLongString()}/{code}/results/";
                                string firstFile = firstSource + $"{subAlg.Code}_runtime.txt";
                                
                                File.Delete(firstFile);
                                
                                string allAverages = caseValues.Keys
                                    .Select(key => $"{key} {caseValues[key] / folders.Length}")
                                    .StringJoin(Environment.NewLine);
                                
                                File.WriteAllText(firstFile, allAverages);
                                
                                // final
                                label_end:
                                cw.UnIndent();
                            }
                        }
                        
                        // all algos done - mimic the usual plotting call on #1
                        string rootDir = $"{folders.First()}runtime/{et.ToLongString()}/{es.ToLongString()}/{code}/";
                        Utils.RunSimpleVoidProcess("gnuplot", rootDir, $"{code}_rt.plt", true);
                        
                        cw.UnIndent();
                    }
                    cw.UnIndent();
                }
                cw.UnIndent();
            }
        }
    }
}