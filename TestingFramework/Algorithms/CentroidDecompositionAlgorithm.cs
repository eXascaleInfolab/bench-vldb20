using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class CentroidDecompositionAlgorithm : Algorithm
    {
        public List<int> KList = new List<int>(new[] { 3, 2, 1 });

        private static bool _init = false;
        public CentroidDecompositionAlgorithm() : base(ref _init)
        { }
        
        public override string[] EnumerateOutputFiles(int tcase)
        {
            return KList.Select(k => $"{AlgCode}{tcase}_k{k}.txt").ToArray();
        }

        private static string StyleOf(int k) =>
            "linespoints lt 8 lw 3 pt 7 lc rgbcolor \"" + (k == 2 ? "dark-" : "") + $"red\" pointsize {k-1}";

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms()
        {
            return KList.Select(k => new SubAlgorithm($"{AlgCode}_k{k}", String.Empty, StyleOf(k)));
        }

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms(int tcase)
        {
            return KList.Select(k => new SubAlgorithm($"{AlgCode}_k{k}", $"{AlgCode}{tcase}_k{k}", StyleOf(k)));
        }
        
        protected override void PrecisionExperiment(ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
            KList.ForEach(k => RunCd(GetCdProcess(data.N, data.M, data, tcase, k)));
        }

        private Process GetCdProcess(int n, int m, DataDescription data, int len, int k)
        {
            Process cdproc = new Process();

            cdproc.StartInfo.WorkingDirectory = EnvPath;
            cdproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/incCD";
            cdproc.StartInfo.CreateNoWindow = true;
            cdproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            cdproc.StartInfo.UseShellExecute = false;

            cdproc.StartInfo.Arguments = $"-test r -n {n} -m {m} -k {k} " +
                                         $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                         $"-out ./{SubFolderDataOut}{AlgCode}{len}_k{k}.txt";

            return cdproc;
        }

        private void RunCd(Process cdproc)
        {
            cdproc.Start();
            cdproc.WaitForExit();
            
            if (cdproc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] CD returned code {cdproc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {cdproc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
            if (et == ExperimentType.Streaming)
            {
                throw new ArgumentException("Batch CD is not suitable for a streaming test.");
            }
            
            KList.ForEach(k => RunCd(GetRuntimeCdProcess(data.N, data.M, data, tcase, k)));
        }

        public override void GenerateData(string sourceFile, string code, int tcase, (int, int, int)[] missingBlocks,
            (int, int) rowRange, (int, int) columnRange)
        {
            sourceFile = DataWorks.FolderData + sourceFile;
            
            (int rFrom, int rTo) = rowRange;
            (int cFrom, int cTo) = columnRange;
            
            double[][] res = DataWorks.GetDataLimited(sourceFile, rTo - rFrom, cTo - cFrom);
            
            int n = rTo > res.Length ? res.Length : rTo;
            int m = cTo > res[0].Length ? res[0].Length : cTo;
            
            var data = new StringBuilder();

            for (int i = rFrom; i < n; i++)
            {
                string line = "";

                for (int j = cFrom; j < m; j++)
                {
                    if (Utils.IsMissing(missingBlocks, i, j))
                    {
                        line += "NaN" + " ";
                    }
                    else
                    {
                        line += res[i][j] + " ";
                    }
                }
                data.Append(line.Trim() + Environment.NewLine);
            }

            string destination = EnvPath + SubFolderDataIn + $"{code}_m{tcase}.txt";
            
            if (File.Exists(destination)) File.Delete(destination);
            File.AppendAllText(destination, data.ToString());
        }

        private Process GetRuntimeCdProcess(int n, int m, DataDescription data, int len, int k)
        {
            Process cdproc = new Process();

            cdproc.StartInfo.WorkingDirectory = EnvPath;
            cdproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/incCD";
            cdproc.StartInfo.CreateNoWindow = true;
            cdproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            cdproc.StartInfo.UseShellExecute = false;

            cdproc.StartInfo.Arguments = $"-test rt -n {n} -m {m} -k {k} " +
                                         $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                         $"-out ./{SubFolderDataOut}{AlgCode}{len}_k{k}.txt";

            return cdproc;
        }
    }
}