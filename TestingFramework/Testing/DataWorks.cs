using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using TestingFramework.Algorithms;

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
        
        public const string FolderResults = "results/";
        public const string FolderResultsPlots = "results/plots/";

        public const string FolderTimeseries = "timeseries/";

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
                "'data/{len}/{algo_file}.txt' index 0 using 1:2 title '{algo_code}' with {algo_style}, \\";

            var allAlgos = new List<string>();

            foreach (Algorithm alg in algorithms.Where(a => a.IsPlottable))
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
                ("{mbsize}", missingBlocks[0].Item3.ToString()),
                ("{mbstart}", (missingBlocks[0].Item2 + offset).ToString()), //offset'd
                ("{allplots}", result));
        }

        public static void GenerateMseGnuPlot(IEnumerable<Algorithm> algorithms, string code, int caseStart, int caseEnd, int caseTick, ExperimentScenario es)
        {
            const string lineTemplate =
                "'error/results/values/rmse/RMSE_{algo_file}.dat' index 0 using 1:2 title '{algo_code}' with {algo_style}, \\";

            var allAlgos = new List<string>();

            foreach (Algorithm alg in algorithms.Where(a => a.IsPlottable))
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
                "'results/{algo_file}_runtime.txt' index 0 using 1:2 title '{algo_code}' with {algo_style}, \\";

            var allAlgos = new List<string>();
            
            foreach (Algorithm alg in algorithms.Where(a => a.IsPlottable))
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

        #region TSPrep

        public static List<(DateTime, double)>[] TimeSeries(
            string subFolder, string searchFormat, int headerSize,
            Func<string, (DateTime, double)> lineParser,
            DateTime skipUtil = default(DateTime),
            bool storeNaN = false)
        {
            string folder = FolderTimeseries + subFolder;
            if (!folder.EndsWith("/")) folder += "/";

            string[] files = Directory.GetFiles(folder, searchFormat);

            List<(DateTime, double)>[] data = files.Select(x => new List<(DateTime, double)>()).ToArray();

            for (int i = 0; i < files.Length; i++)
            {
                string file = files[i];

                var sr = new StreamReader(File.Open(file, FileMode.Open));

                if (headerSize > 0)
                {
                    string header = sr.ReadLine();

                    Console.WriteLine($"File {file}:{Environment.NewLine}  {header}");

                    Enumerable.Range(0, headerSize).ForEach((x) => sr.ReadLine());
                }
                else
                {
                    Console.WriteLine($"File {file}: [no header]");
                }

                string line = sr.ReadLine();
                DateTime date;
                double val;

                do
                {
                    (date, _) = lineParser(line);
                    line = sr.ReadLine();

                } while (date < skipUtil && !sr.EndOfStream);

                uint[] hours = new uint[24];
                DateTime lastDate;
                UInt64 diff = 0;
                UInt64 counter = 1;

                // line 1
                (date, val) = lineParser(line);
                lastDate = date;

                if (storeNaN || !Double.IsNaN(val))
                {
                    data[i].Add((date, val));
                }

                while (!sr.EndOfStream)
                {
                    //read
                    line = sr.ReadLine();
                    (DateTime, double) datapoint = lineParser(line);
                    if (storeNaN || !Double.IsNaN(datapoint.Item2))
                    {
                        data[i].Add(datapoint);

                        (date, _) = datapoint;

                        //hourly frequency
                        hours[date.Hour] += 1;

                        //average distance between timestamps
                        diff += (UInt64)(date - lastDate).TotalMinutes;
                        lastDate = date;

                        counter++;
                    }

                }

                //summary:
                Console.WriteLine("[Summary]");
                Console.WriteLine($"Average distance={diff / counter}");

                Console.WriteLine();

                for (int h = 0; h < hours.Length; h++)
                {
                    Console.WriteLine($"---Hour {h}: {hours[h]} entries");
                }
                Console.WriteLine($"---|---Total: {hours.Sum((x) => (decimal)x)} entries");

                // end
                Console.WriteLine();
            }

            return data.Where(x => x.Any()).ToArray();
        }

        public static void TimeSeriesMerge(List<(DateTime, double)>[] data, string output)
        {
            DateTime latestStartingDate = data[0][0].Item1;
            DateTime earliestEndDate = data[0][data[0].Count - 1].Item1;

            for (int i = 0; i < data.Length; i++)
            {
                DateTime currentStart = data[i][0].Item1;
                DateTime currentEnd = data[i][data[i].Count - 1].Item1;

                Console.WriteLine($"TS #{i} from {currentStart.ToShortDateString()} to {currentEnd.ToShortDateString()}");

                latestStartingDate = currentStart > latestStartingDate ? currentStart : latestStartingDate;
                earliestEndDate = currentEnd < earliestEndDate ? currentEnd : earliestEndDate;
            }

            DateTime startingPoint = new DateTime(latestStartingDate.Year, latestStartingDate.Month, latestStartingDate.Day);
            DateTime endPoint = new DateTime(earliestEndDate.Year, earliestEndDate.Month, earliestEndDate.Day);

            var localData = data.Select(x => new List<(DateTime, double)>()).ToArray();
            var indexBuffer = new int[data.Length];

            DateTime today = startingPoint;

            List<(DateTime, double[])> ts = new List<(DateTime, double[])>();

            do
            {
                // pull all data points from today and put them into localData collection
                // also shift the index from the buffer until it's no longer today
                for (int i = 0; i < data.Length; i++)
                {
                    int j = indexBuffer[i];

                    for (; j < data[i].Count; j++)
                    {
                        var dataPoint = data[i][j];

                        (DateTime date, _) = dataPoint;

                        if ((date - today).Days == 0)
                        {
                            localData[i].Add(dataPoint);
                        }
                        else
                        {
                            break;
                        }
                    }

                    indexBuffer[i] = j;
                }

                int any = localData.Min(x => x.Count);

                Console.WriteLine($"today={today.ToShortDateString()} any={any}");

                if (any < 1)
                {
                    goto post_process;
                }

                // we have at least one data point from each TS for this day

                int tick = (24 * 60) / any;

                for (int j = 0; j < any; j++)
                {
                    double[] toDayta = localData.Select(x => x.Skip(j).First().Item2).ToArray();

                    ts.Add((today.AddMinutes(tick * j), toDayta));
                }

                post_process:
                {
                    today = today.AddDays(1);
                    localData.ForEach(x => x.Clear());
                }
            } while (today <= endPoint);

            string file = @"timeseries/" + output;
            if (File.Exists(file)) File.Delete(file);
            StreamWriter sw = new StreamWriter(new FileStream(file, FileMode.Append));

            for (int j = 0; j < ts[0].Item2.Length; j++)
            {
                for (int i = 0; i < ts.Count; i++)
                {
                    if (Double.IsNaN(ts[i].Item2[j]))
                    {
                        ts[i].Item2[j] = (ts[i - 1].Item2[j] + ts[i + 1].Item2[j]) / 2;
                    }
                }
            }

            foreach (var dataPoint in ts)
            {
                (DateTime date, double[] values) = dataPoint;

                var line = values.Aggregate("", (arg1, arg2) => arg1 + " " + arg2).Trim(' ');
                //sw.WriteLine($"{date.ToString("YYYY-MM-DD")},{line}");
                sw.WriteLine($"{line}");
            }
            sw.Close();
        }

        #endregion
    }
}