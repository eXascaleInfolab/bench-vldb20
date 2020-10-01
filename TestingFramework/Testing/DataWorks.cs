using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using TestingFramework.AlgoIntegration;

namespace TestingFramework.Testing
{
    static class DataWorks
    {
        // this has to point to the folder where all the results are stored and plots are created from them
        public static string FolderPlotsRemoteBase = null;
        
        // can be used to change the base folder for everything
        // one usecase is an average-of-5 runtime test, see Program.cs
        public static string FolderPlotsRemote = FolderPlotsRemoteBase;
        
        //
        // relative paths
        // internal paths for the testing framework
        //
        public const string FolderData = "data/";
        
        public const string FolderResults = ".tech/";
        public const string FolderResultsPlots = ".tech/plots/";

        public const string FolderTimeseries = "timeseries/";

        public static bool PlottableOverride = false;
        public static bool DisableVisualization = false;

        #region DataPrep

        /// <summary>
        /// A hack
        /// </summary>
        /// <param name="fileInput">count 'em here</param>
        public static int CountMatrixColumns(string fileInput)
        {
            fileInput = FolderData + fileInput;
            
            double[][] res =
                File.ReadAllLines(fileInput)
                    .Where(x => !String.IsNullOrEmpty(x))
                    .Take(1)
                    .Select(
                        x => x.Split(' ').Select(Utils.ParseDouble).ToArray()
                    )
                    .ToArray();

            return res[0].Length;
        }
        
        /// <summary>
        /// A hack #2
        /// </summary>
        /// <param name="fileInput">count 'em here</param>
        public static int CountMatrixRows(string fileInput)
        {
            fileInput = FolderData + fileInput;

            return File.ReadAllLines(fileInput).Count(x => !String.IsNullOrEmpty(x));
        }
        
        /// <summary>
        /// Gets an array representing a matrix from a <paramref name="path"/>
        /// of size <paramref name="rowLimit"/> by <paramref name="colLimit"/>.
        /// </summary>
        /// <param name="path">Source file</param>
        /// <param name="rowLimit">Row count of the matrix</param>
        /// <param name="colLimit">Column count of the matrix</param>
        /// <returns>A matrix as a double[][] array</returns>
        public static double[][] GetDataLimited(string path, int rowLimit, int colLimit)
        {
            var sr = new StreamReader(File.Open(path, FileMode.Open));

            var rows = new List<double[]>();

            for (int i = 0; i < rowLimit && !sr.EndOfStream; i++)
            {
                string line = sr.ReadLine();

                while (String.IsNullOrEmpty(line))
                {
                    line = sr.ReadLine();
                }

                double[] row = line.Split(' ').Take(colLimit).Select(Utils.ParseDouble).ToArray();

                rows.Add(row);
            }
            
            sr.Close();
            return rows.ToArray();
        }
        
        public static void DataRange(string fileInput, string fileOutput,
                                     int startRow = 0, int lengthRow = Int32.MaxValue,
                                     int startColumn = 0, int lengthColumn = Int32.MaxValue)
        {
            Console.WriteLine("Entering range...");
            
            if (fileInput == fileOutput)
                throw new ArgumentException("Input and output cannot be the same file");

            fileInput = FolderData + fileInput;
            fileOutput = FolderData + fileOutput;

            // cleanup \\

            if (File.Exists(fileOutput)) File.Delete(fileOutput);

            // transform \\

            double[][] res =
                File.ReadAllLines(fileInput)
                .Where(x => !String.IsNullOrEmpty(x))
                .Select(
                        x => x.Split(' ').Select(Utils.ParseDouble).ToArray()
                    ).ToArray();

            if (startRow >= res.Length) throw new Exception("invalid starting row");
            if (startColumn >= res[0].Length) throw new Exception("invalid starting column");

            lengthRow = Math.Min(lengthRow, res.Length - startRow);
            lengthColumn = Math.Min(lengthColumn, res[0].Length - startColumn);

            StringBuilder sb = new StringBuilder();

            for (int i = startRow; i < startRow + lengthRow; i++)
            {
                string line = "";

                for (int j = startColumn; j < startColumn + lengthColumn; j++)
                {
                    line += res[i][j] + " ";
                }

                sb.Append(line.Trim() + Environment.NewLine);
            }

            File.AppendAllText(fileOutput, sb.ToString());
        }

        #endregion

        #region Post-processing

        /// <summary>
        /// Adds the GNUPlot numeration for a file with time series.
        /// </summary>
        /// <param name="files">List of files.</param>
        public static void AddGnuPlotNumeration(bool skipHeader, string subFolder = "", int startAt = 0, params string[] files)
        {
            Console.WriteLine("Entering GNUPlot numeration...");

            files.Select(x => FolderResults + x)
                .Where(x => !File.Exists(x))
                .ForEach(x => Console.WriteLine($"[WARNING] File {x} not found."));

            var res = files.Select(x => FolderResults + x)
                          .Where(File.Exists)
                          .Select(x => (x, File.ReadAllLines(x)));

            foreach (var fileInfo in res)
            {
                string[] data = fileInfo.Item2;

                if (skipHeader)
                {
                    for (int i = 1; i < data.Length; i++)
                    {
                        if (!String.IsNullOrEmpty(data[i])) data[i] = (i + startAt) + "\t" + data[i];
                    }
                }
                else
                {
                    for (int i = 0; i < data.Length; i++)
                    {
                        if (!String.IsNullOrEmpty(data[i])) data[i] = (i + 1 + startAt) + "\t" + data[i];
                    }
                }

                var fileWithExt = Utils.SplitFileExtention(fileInfo.Item1);
                string newFileName = fileWithExt.Item1.Replace(FolderResults, FolderResultsPlots + subFolder) + (String.IsNullOrEmpty(fileWithExt.Item2) ? "" : "." + fileWithExt.Item2);
                if (File.Exists(newFileName)) { File.Delete(newFileName); }
                File.AppendAllLines(newFileName, data);

                // copy into raw/
                newFileName = fileWithExt.Item1.Replace(FolderResults, FolderResultsPlots + subFolder + "raw/") + (String.IsNullOrEmpty(fileWithExt.Item2) ? "" : "." + fileWithExt.Item2);
                if (File.Exists(newFileName)) { File.Delete(newFileName); }
                File.Copy(fileInfo.Item1, newFileName);
            }
        }

        public static void CalculateMse(ValueTuple<int, int, int>[] missingBlocks, string referenceTs,
                                        string mMatOutput, params string[] compareFiles)
        {
            Console.WriteLine("Entering MSE check...");

            referenceTs = FolderResults + referenceTs;

            // new data \\

            double[][] reference =
                File.ReadAllLines(referenceTs)
                .Where(x => !String.IsNullOrEmpty(x))
                .Select(
                        x => x.Split(' ').Select(Utils.ParseDouble).ToArray()
                    ).ToArray();

            // try to do correlation test

            double[][] missingMatrix = new double[compareFiles.Length + 1][];

            for (int i = 0; i < missingMatrix.Length; i++)
            {
                missingMatrix[i] = new double[missingBlocks.Sum(x => x.Item3)];
            }

            int total = 0;
            for (int i = 0; i < missingBlocks.Length; i++)
            {
                var mBlock = missingBlocks[i];

                for (int j = 0; j < mBlock.Item3; j++)
                {
                    missingMatrix[0][total + j] = reference[mBlock.Item2 + j][mBlock.Item1];
                }

                total += mBlock.Item3;
            }

            // continue

            int cnt = 0;
            foreach (string file in compareFiles)
            {
                cnt++;
                double[][] compare =
                    File.ReadAllLines(FolderResults + file)
                    .Where(x => !String.IsNullOrEmpty(x))
                    .Select(
                            x => x.Trim().Split(x.Contains(',') ? ',' : ' ').Select(Utils.ParseDouble).ToArray()
                        ).ToArray();

                total = 0;
                for (int mi = 0; mi < missingBlocks.Length; mi++)
                {
                    var mb = missingBlocks[mi];

                    for (int i = mb.Item2; i < mb.Item2 + mb.Item3; i++)
                    {
                        missingMatrix[cnt][total + i - mb.Item2] = compare[i][mb.Item1];
                    }

                    total += mb.Item3;
                }

                //Console.WriteLine($"MSE [ref vs {file}] on all MB (of size {missingBlocks.Sum(x => x.Item3)}): {mse}");

                StringBuilder sb = new StringBuilder();

                for (int i = 0; i < missingMatrix[0].Length; i++)
                {
                    string line = "";
                    for (int j = 0; j < missingMatrix.Length; j++)
                    {
                        line += missingMatrix[j][i] + " ";
                    }
                    sb.Append(line.Trim() + Environment.NewLine);
                }

                string missingMatrixFile = FolderResults + mMatOutput;

                if (File.Exists(missingMatrixFile)) File.Delete(missingMatrixFile);
                File.AppendAllText(missingMatrixFile, sb.ToString());
            }
        }

        public static void CollectRuntimeResults(IEnumerable<int> scenarios, IEnumerable<Algorithm> algorithms,
            params string[] backupLocation)
        {
            IList<int> enumerable = scenarios as IList<int> ?? scenarios.ToList();
            
            foreach (Algorithm alg in algorithms)
            {
                foreach (SubAlgorithm subAlgorithm in alg.EnumerateSubAlgorithms())
                {
                    var algCase = new List<(int, double)>();
                    
                    foreach (var tcase in enumerable)
                    {
                        string file = alg.EnumerateSubAlgorithms(tcase).First(x => x.Code == subAlgorithm.Code).CaseCode;
                        string runtimeValueStr = File.ReadAllText(FolderResults + file + ".txt");
                        double runtimeValue = Utils.ParseDouble(runtimeValueStr);

                        algCase.Add((tcase, runtimeValue));
                    }

                    string outputFileName = subAlgorithm.Code + "_runtime.txt";

                    var sb = new StringBuilder();
                    algCase.ForEach(tuple => sb.Append(tuple.Item1 + " " + tuple.Item2 + Environment.NewLine));

                    foreach (string folder in backupLocation)
                    {
                        string backupOutput = folder + outputFileName;

                        if (File.Exists(backupOutput)) File.Delete(backupOutput);
                        File.AppendAllText(backupOutput, sb.ToString());
                    }
                }
            }
        }
        
        public static void GeneratePrecisionGnuPlot(IEnumerable<Algorithm> algorithms, string code, int nlimit, int tcase, (int, int, int)[] missingBlocks, int offset = 0)
        {
            const string lineTemplate =
                "'recovery/values/{len}/{algo_file}.txt' index 0 using 1:2 title '{algo_code}' with {algo_style}, \\";

            var allAlgos = new List<string>();

            foreach (Algorithm alg in algorithms.Where(a => a.IsPlottable || PlottableOverride))
            {
                foreach (var subAlgorithm in alg.EnumerateSubAlgorithms(tcase).Take(1)) // Take(1)
                {
                    string line = lineTemplate
                        .Replace("{len}", tcase.ToString())
                        .Replace("{algo_file}", subAlgorithm.CaseCode)
                        .Replace("{algo_code}", subAlgorithm.Code.Replace("_", "\\_"))
                        .Replace("{algo_style}", subAlgorithm.Style);
                    
                    allAlgos.Add(line);
                }
            }

            string result = allAlgos.StringJoin(Environment.NewLine + "\t");
            
            Utils.FileFindAndReplace(FolderResults + "plotfiles/template_mblock.plt",
                $"{FolderResults}plotfiles/out/{code}_m{tcase}.plt",
                ("{len}", tcase.ToString()),
                ("{code}", code),
                ("{nlimit}", nlimit.ToString()),
                ("{mbsize}", Math.Min(missingBlocks[0].Item3, 2500).ToString()),
                ("{mbstart}", (missingBlocks[0].Item2 + offset).ToString()), //offset'd
                ("{allplots}", result));
        }

        public static void GenerateMseGnuPlot(IEnumerable<Algorithm> algorithms, string code, int caseStart, int caseEnd, int caseTick, ExperimentScenario es)
        {
            const string lineTemplate =
                "'error/rmse/RMSE_{algo_file}.dat' index 0 using 1:2 title '{algo_code}' with {algo_style}, \\";

            var allAlgos = new List<string>();

            foreach (Algorithm alg in algorithms.Where(a => a.IsPlottable || PlottableOverride))
            {
                foreach (var subAlgorithm in alg.EnumerateSubAlgorithms())
                {
                    string line = lineTemplate
                        .Replace("{algo_file}", subAlgorithm.Code)
                        .Replace("{algo_code}", subAlgorithm.Code.Replace("_", "\\_"))
                        .Replace("{algo_style}", subAlgorithm.Style);
                    
                    allAlgos.Add(line);
                }
            }

            string rmse = allAlgos.StringJoin(Environment.NewLine + "\t");
            string mse = rmse.Replace("RMSE", "MSE").Replace("rmse", "mse"); //strip R
            string mae = rmse.Replace("RMSE", "MAE").Replace("rmse", "mae"); //place mae
            
            Utils.FileFindAndReplace(FolderResults + "plotfiles/template_mse.plt",
                $"{FolderResults}plotfiles/out/{code}_mse.plt",
                ("{code}", code),
                ("{descr}", es.ToDescriptiveString()),
                ("{caseStart}", caseStart.ToString()),
                ("{caseEnd}", caseEnd.ToString()),
                ("{caseTick}", caseTick.ToString()),
                ("{rmse}", rmse),
                ("{mse}", mse),
                ("{mae}", mae));
        }
        
        public static void GenerateRuntimeGnuPlot(IEnumerable<Algorithm> algorithms, string code, int caseStart, int caseEnd, int caseTick, ExperimentType et, ExperimentScenario es)
        {
            const string lineTemplate =
                "'runtime/values/{algo_file}_runtime.txt' index 0 using 1:2 title '{algo_code}' with {algo_style}, \\";

            var allAlgos = new List<string>();
            
            foreach (Algorithm alg in algorithms.Where(a => a.IsPlottable || PlottableOverride))
            {
                foreach (var subAlgorithm in alg.EnumerateSubAlgorithms())
                {
                    string line = lineTemplate
                        .Replace("{algo_file}", subAlgorithm.Code)
                        .Replace("{algo_code}", subAlgorithm.Code.Replace("_", "\\_"))
                        .Replace("{algo_style}", subAlgorithm.Style);
                    
                    allAlgos.Add(line);
                }
            }

            string rt = allAlgos.StringJoin(Environment.NewLine + "\t");
            
            Utils.FileFindAndReplace(FolderResults + "plotfiles/template_runtime.plt",
                $"{FolderResults}plotfiles/out/{code}_rt.plt",
                ("{code}", code),
                ("{descr}", es.ToDescriptiveString()),
                ("{caseStart}", caseStart.ToString()),
                ("{caseEnd}", caseEnd.ToString()),
                ("{caseTick}", caseTick.ToString()),
                ("{rt}", rt));
        }

        #endregion
    }
}