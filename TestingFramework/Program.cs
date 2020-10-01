using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using TestingFramework.AlgoIntegration;
using TestingFramework.Testing;

namespace TestingFramework
{
    internal static class Program
    {
        private static void CLIPrintHelp()
        {
            Utils.ContinuousWriter cw = new Utils.ContinuousWriter(Console.WriteLine);

            cw.WriteLine("Primary commands:");
            cw.WriteLine();

            cw.WriteLine("--algorithm [alg]");
            cw.WriteLine("-alg [alg]");
            cw.Indent();
            cw.WriteLine("Adds an algorithm to the benchmark");
            cw.WriteLine("[alg] - name of the algorithm");
            cw.Indent();
            cw.WriteLine("or a comma-separated list of algorithms");
            cw.WriteLine("or \"all\" to include all algorithms [default]");
            cw.UnIndent();
            cw.UnIndent();
            cw.WriteLine();
            
            cw.WriteLine("--algorithm-param [alg] [param]");
            cw.WriteLine("-algx [alg] [param]");
            cw.Indent();
            cw.WriteLine("Adds an algorithm with a custom parameter to the benchmark");
            cw.WriteLine("[alg] - name of the algorithm");
            cw.WriteLine("[param] - individual parameter for the algorithm");
            cw.UnIndent();
            cw.WriteLine();
            
            cw.WriteLine("--scenario [scen]");
            cw.WriteLine("-scen [scen]");
            cw.Indent();
            cw.WriteLine("Adds a scenario to the benchmark");
            cw.WriteLine("[scen] - name of the scenario");
            cw.Indent();
            cw.WriteLine("or a comma-separated list of scenarios");
            cw.WriteLine("or \"all\" to include all scenarios [default]");
            cw.UnIndent();
            cw.UnIndent();
            cw.WriteLine();

            cw.WriteLine("--dataset [data]");
            cw.WriteLine("-d [data]");
            cw.Indent();
            cw.WriteLine("Adds a dataset to the benchmark");
            cw.WriteLine("[data] - name of the dataset");
            cw.Indent();
            cw.WriteLine("or a comma-separated list of datasets");
            cw.WriteLine("or \"all\" to include all datasets [default]");
            cw.UnIndent();
            cw.UnIndent();
            cw.WriteLine();
            
            cw.WriteLine("Optional commands:");
            cw.WriteLine();

            cw.WriteLine("--help");
            cw.WriteLine("-h");
            cw.Indent();
            cw.WriteLine("To see this help information");
            cw.UnIndent();
            cw.WriteLine();

            cw.WriteLine("--no-runtime");
            cw.WriteLine("-nort");
            cw.Indent();
            cw.WriteLine("Disable runtime test of the algorithms");
            cw.UnIndent();
            cw.WriteLine();

            cw.WriteLine("--no-precision");
            cw.WriteLine("-noprec");
            cw.Indent();
            cw.WriteLine("Disable precision test of the algorithms");
            cw.UnIndent();
            cw.WriteLine();

            cw.WriteLine("---no-visualization");
            cw.WriteLine("-novis");
            cw.Indent();
            cw.WriteLine("Disable the render of plots which show the recovered block");
            cw.UnIndent();
            cw.WriteLine();

            cw.WriteLine("--output [fodler]");
            cw.WriteLine("-out [folder]");
            cw.Indent();
            cw.WriteLine("Redirect results from default folder to a custom one");
            cw.WriteLine("[folder] - name of the folder where to place the output of the benchmark");
            cw.Indent();
            cw.WriteLine("default value - \"Results/\"");
            cw.UnIndent();
            cw.UnIndent();
            cw.WriteLine();

        }

        private static void CLISetAlgoParam(Algorithm alg, string param)
        {
            switch (alg)
            {
                case CentroidDecompositionRecoveryAlgorithm instAlg:
                    instAlg.KList = new List<int> { Int32.Parse(param) };
                    break;
                    
                case TrmfAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case SpiritAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case NnmfAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case GrouseAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case SoftImputeAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case ROSLAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case DynaMMoAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case SVDImputeAlgorithm instAlg:
                    instAlg.Truncation = Int32.Parse(param);
                    break;
                    
                case TkcmAlgorithm instAlg:
                    instAlg.ParamL = Int32.Parse(param);
                    break;
                    
                case StmvlAlgorithm instAlg:
                    instAlg.Alpha = Double.Parse(param);
                    break;
                    
                case SVTAlgorithm instAlg:
                    instAlg.TauScale = Double.Parse(param);
                    break;

                default:
                    throw new Exception("Implementation incomplete");
            }
        }

        private static void CLIAllAgosIntegrity(bool allAlgosInvoked)
        {
            if (allAlgosInvoked)
            {
                Console.WriteLine("`--algorithm all` or `--algorithm full` was already passed to the tool");
                Console.WriteLine("can't add more algorithms with --algorithm or --algorithm-param");
                Environment.Exit(-1);
            }
        }

        private static void CLIVerifyInput(int idx, int len, string cmd, string error)
        {
            if (idx >= len)
            {
                Console.WriteLine($"Obligatory argument {error} is not provided for command {cmd}.");
                Environment.Exit(-1);
            }
        }

        private static Algorithm CLIFindAlgorithmByName(string name)
        {
            if (name == "cd") name = "cdrec";
            Algorithm alg = AlgoPack.ListAlgorithms.Where(x => x.AlgCode.ToLower() == name).FirstOrDefault();
            if (alg == null)
            {
                Console.WriteLine($"Algorithms with code {name} doesn't exist");
                Environment.Exit(-1);
            }
            return alg;
        }

        private static void CLIParse(
            string[] args, ref List<string> codes, ref List<Algorithm> algos, ref List<string> scenarios,
            ref bool doPrecision, ref bool doRuntime)
        {
            string subCmd;
            bool allAlgosInvoked = false;

            if (args.Length == 0)
            {
                CLIPrintHelp();
                Environment.Exit(0);
            }

            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i].ToLower())
                {
                    case "[arguments]":
                    case "--help":
                    case "-h":
                        CLIPrintHelp();
                        Environment.Exit(0);
                        break;

                    case "--dataset":
                    case "-d":
                        CLIVerifyInput(i + 1, args.Length, args[i], "dataset name(s)");
                        subCmd = args[++i];

                        if (subCmd == "all")
                        {}
                        else if (subCmd.Contains(","))
                        {
                            codes.AddRange(subCmd.Split(',').Select(x => x.Trim()).Distinct());
                        }
                        else
                        {
                            codes.Add(subCmd);
                        }
                        break;

                    case "--algorithm":
                    case "-alg":
                        CLIAllAgosIntegrity(allAlgosInvoked);
                        CLIVerifyInput(i + 1, args.Length, args[i], "algorithm name(s)");
                        subCmd = args[++i];

                        if (subCmd == "all")
                        {
                            if (algos.Count() != 0)
                            {
                                Console.WriteLine("Algorithms were already added with --algorithm or --algorithm-param");
                                Console.WriteLine("`--algorithm all` can't be invoked any more");
                                Environment.Exit(-1);
                            }
                            // static list for vldb bench
                            algos.AddRange(new[] { AlgoPack.Stmvl, AlgoPack.CdRec, AlgoPack.Tkcm, AlgoPack.Spirit, AlgoPack.Trmf, AlgoPack.Nnmf, AlgoPack.Grouse, AlgoPack.Svt, AlgoPack.SoftImpute, AlgoPack.ROSL, AlgoPack.DynaMMo, AlgoPack.SvdI });                            
                            allAlgosInvoked = true;
                        }
                        else if (subCmd == "full")
                        {
                            if (algos.Count() != 0)
                            {
                                Console.WriteLine("Algorithms were already added with --algorithm or --algorithm-param");
                                Console.WriteLine("`--algorithm full` can't be invoked any more");
                                Environment.Exit(-1);
                            }
                            // left null and will be filled with ALL algorithms
                            allAlgosInvoked = true;
                        }
                        else if (subCmd.Contains(","))
                        {
                            DataWorks.PlottableOverride = true;
                            algos.AddRange(subCmd.Split(',').Select(x => x.Trim()).Select(CLIFindAlgorithmByName));
                        }
                        else
                        {
                            DataWorks.PlottableOverride = true;
                            algos.Add(CLIFindAlgorithmByName(subCmd)); //map into Algorithm instance
                        }
                        break;

                    case "--algorithm-param":
                    case "-algx":
                        CLIAllAgosIntegrity(allAlgosInvoked);
                        CLIVerifyInput(i + 1, args.Length, args[i], "algorithm name");
                        subCmd = args[++i];

                        CLIVerifyInput(i + 1, args.Length, args[i], "algorithm parameter");
                        string param = args[++i];

                        DataWorks.PlottableOverride = true;
                        Algorithm alg = CLIFindAlgorithmByName(subCmd);
                        algos.Add(alg);

                        CLISetAlgoParam(alg, param);
                        
                        break;

                    case "--scenario":
                    case "-scen":
                        CLIVerifyInput(i + 1, args.Length, args[i], "scenario name(s)");
                        subCmd = args[++i];

                        if (subCmd == "all")
                        {}
                        else if (subCmd.Contains(","))
                        {
                            scenarios.AddRange(subCmd.Split(',').Select(x => x.Trim()).Distinct());
                        }
                        else
                        {
                            scenarios.Add(subCmd);
                        }
                        break;

                    case "--no-runtime":
                    case "-nort":
                        doRuntime = false;
                        break;

                    case "--no-precision":
                    case "-noprec":
                        doPrecision = false;
                        break;

                    case "--no-visualization":
                    case "-novis":
                        DataWorks.DisableVisualization = true;
                        break;

                    case "--set-output":
                    case "-out":
                        CLIVerifyInput(i + 1, args.Length, args[i], "output folder");
                        subCmd = args[++i];

                        if (!subCmd.EndsWith("/")) subCmd = subCmd + "/";

                        DataWorks.FolderPlotsRemote = DataWorks.FolderPlotsRemoteBase = subCmd;
                        break;

                    default:
                        Console.WriteLine($"{args[i]} is not a recognized CLI parameter for the program");
                        Environment.Exit(-1);
                        break;
                }
            }

            if (algos.Count() != algos.Distinct().Count())
            {
                Console.WriteLine($"One or more algorithms were supplied multiple times. This can cause configuration conflicts.");
                Console.WriteLine($"Please ensure every algorithm is given only once.");
                Environment.Exit(-1);
            }

            codes.ForEach(c =>
            {
                if (!File.Exists(DataWorks.FolderData + $"{c}/{c}_normal.txt"))
                {
                    Console.WriteLine($"Dataset with name {c} is not found. Please make sure dataset with this name exists and the file is named correctly");
                    Environment.Exit(-1);
                }
            });

            var allScen = EnumMethods.AllExperimentScenarios().SelectMany(x => new[] { x.ToLongString(), x.ToShortString() });
            var invalidScen = scenarios.Except(allScen);
            if (invalidScen.Count() != 0)
            {
                Console.WriteLine($"{invalidScen.First()} is an invalid scenario name");
                Environment.Exit(-1);
            }
        }

        private static void Main(string[] args)
        {
            DataWorks.FolderPlotsRemote = DataWorks.FolderPlotsRemoteBase = "Results/";

            // technical
            Console.CancelKeyPress += InterruptHandler;

            // cli
            List<string> codesinit = new List<string>();
            List<Algorithm> algos = new List<Algorithm>();
            List<string> scenarios = new List<string>();
            bool doPrecision = true;
            bool doRuntime = true;
            CLIParse(args, ref codesinit, ref algos, ref scenarios, ref doPrecision, ref doRuntime);

            if (! (doPrecision || doRuntime))
            {
                Console.WriteLine("Both precision and runtime tests are disabled, no results will be produced");
                Environment.Exit(-1);
            }

            if (codesinit == null || codesinit.Count == 0)
            {
                codesinit = Directory.GetDirectories(DataWorks.FolderData)
                    .Where(x => !x.EndsWith("_.temp"))
                    .Select(x => x.Replace(DataWorks.FolderData, ""))
                    .ToList();
            }
            
            string[] codes = codesinit.Distinct().ToArray();
            string[] codesLimited = codes
                .Where(c => DataWorks.CountMatrixColumns($"{c}/{c}_normal.txt") > 4)
                .ToArray();

            if (algos == null || algos.Count == 0)
            {
                algos = AlgoPack.ListAlgorithms.ToList();
            }

            if (scenarios == null || scenarios.Count == 0)
            {
                scenarios = EnumMethods.AllExperimentScenarios().Select(EnumMethods.ToLongString).ToList();
            }
            AlgoPack.GlobalAlgorithmsLocation = "../../../Algorithms/";
            AlgoPack.GlobalNewAlgorithmsLocation = "../../../Algorithms/NewAlgorithms/";

            // verificaiton that all necessary entries are provided

            if (codes == null || codes.Length == 0)
            {
                throw new InvalidProgramException($"Invalid program state: no datasets found in {DataWorks.FolderData} folder");
            }

            AlgoPack.ListAlgorithms = algos.ToArray();
            AlgoPack.ListAlgorithmsMulticolumn = algos.Where(alg => alg.IsMultiColumn).ToArray();
            
            AlgoPack.CleanUncollectedResults();
            AlgoPack.EnsureFolderStructure(scenarios);
            
            void FullRun(bool enablePrecision, bool enableRuntime)
            {
                foreach (string code in codes)
                {
                    foreach (ExperimentScenario es in EnumMethods.AllExperimentScenarios().Where(x => scenarios.Contains(x.ToLongString()) || scenarios.Contains(x.ToShortString())))
                    {
                        if (es.IsLimited() && !codesLimited.Contains(code)) continue;
                        if (enablePrecision) TestRoutines.PrecisionTest(ExperimentType.Recovery, es, code);
                        if (enableRuntime) TestRoutines.RuntimeTest(ExperimentType.Recovery, es, code);
                    }
                }
            }
            
            FullRun(doPrecision, doRuntime);
            
            FinalSequence();
        }

        private static void FinalSequence()
        {
            if (Utils.DelayedWarnings.Count > 0)
            {
                var writer = new Utils.ContinuousWriter();

                writer.WriteLine($"====={Environment.NewLine}The following delayed warnings were recorded by Testing Framework:" +
                                 $"{Environment.NewLine}===={Environment.NewLine}");

                writer.Indent();

                while (Utils.DelayedWarnings.Count > 0)
                {
                    writer.WriteLine(Utils.DelayedWarnings.Dequeue());
                }
            }
            Console.WriteLine("--- END ---");
        }

        private static void InterruptHandler(object sender, ConsoleCancelEventArgs args)
        {
            Console.WriteLine("--- Ctrl+C received ---");
            FinalSequence();
        }

    }
}
