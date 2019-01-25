using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class NnmfAlgorithm : Algorithm
    {
        private static bool _init = false;
        public NnmfAlgorithm() : base(ref _init)
        { }

        private static string Style => "linespoints lt 8 dt 5 lw 3 pt 8 lc rgbcolor \"blue\" pointsize 2";
        
        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms()
        {
            return new[] { new SubAlgorithm($"{AlgCode}", String.Empty, Style) };
        }

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms(int tcase)
        {
            return new[] { new SubAlgorithm($"{AlgCode}", $"{AlgCode}{tcase}", Style) };
        }

        protected override void PrecisionExperiment(ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
             RunNnmf(GetNnmfProcess(data, tcase, "False", "False"));
        }
        
        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
            RunNnmf(et == ExperimentType.Streaming
                ? GetNnmfProcess(data, tcase, "True", "True")
                : GetNnmfProcess(data, tcase, "True", "False")
            );
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

        private Process GetNnmfProcess(DataDescription data, int len, string rtVal, string rtStrVal)
        {
            Process nnmfproc = new Process();
            
            nnmfproc.StartInfo.WorkingDirectory = EnvPath;
            nnmfproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/incCD";
            nnmfproc.StartInfo.CreateNoWindow = true;
            nnmfproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            nnmfproc.StartInfo.UseShellExecute = false;

            string testType = rtVal == "True" ? "rt" : "o";
            nnmfproc.StartInfo.Arguments = $"-alg nnmf -test {testType} -n {data.N} -m {data.M} -k {AlgoPack.TypicalTruncation} " +
                                           $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                           $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return nnmfproc;
        }
        
        private void RunNnmf(Process nnmfproc)
        {
            nnmfproc.Start();
            nnmfproc.WaitForExit();

            if (nnmfproc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] NNMF returned code {nnmfproc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {nnmfproc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }
    }
}