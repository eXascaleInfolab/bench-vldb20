using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class SpiritAlgorithm : Algorithm
    {
        private static bool _init = false;
        public SpiritAlgorithm() : base(ref _init)
        { }
        
        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}.txt", $"{dataCode}_m{tcase}_range.txt" };
        }
        
        private static string Style => "linespoints lt 8 dt 6 lw 3 pt 6 lc rgbcolor \"dark-green\" pointsize 2";
        
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
            RunSpirit(GetSpiritProcess(data, tcase));
        }
        
        private Process GetSpiritProcess(DataDescription data, int tcase)
        {
            Process spiritproc = new Process();
            
            spiritproc.StartInfo.WorkingDirectory = EnvPath;
            spiritproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/incCD";
            spiritproc.StartInfo.CreateNoWindow = true;
            spiritproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            spiritproc.StartInfo.UseShellExecute = false;
            
            spiritproc.StartInfo.Arguments = $"-alg spirit -test o -n {data.N} -m {data.M} -k {AlgoPack.TypicalTruncation} " +
                                             $"-in ./{SubFolderDataIn}{data.Code}_m{tcase}.txt " +
                                             $"-out ./{SubFolderDataOut}{AlgCode}{tcase}.txt";

            return spiritproc;
        }
        
        private Process GetRuntimeSpiritProcess(DataDescription data, int tcase, bool streaming)
        {
            Process spiritproc = new Process();
            
            spiritproc.StartInfo.WorkingDirectory = EnvPath;
            spiritproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/incCD";
            spiritproc.StartInfo.CreateNoWindow = true;
            spiritproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            spiritproc.StartInfo.UseShellExecute = false;
            
            spiritproc.StartInfo.Arguments = $"-alg spirit -test rt -n {data.N} -m {data.M} -k {AlgoPack.TypicalTruncation} " +
                                             $"-in ./{SubFolderDataIn}{data.Code}_m{tcase}.txt " +
                                             $"-out ./{SubFolderDataOut}{AlgCode}{tcase}.txt";

            return spiritproc;
        }

        private void RunSpirit(Process spiritproc)
        {
            spiritproc.Start();
            spiritproc.WaitForExit();
                
            if (spiritproc.ExitCode != 0) Console.WriteLine($"[WARNING] SPIRIT returned code {spiritproc.ExitCode} on exit.{Environment.NewLine}" +
                                                          $"CLI args: {spiritproc.StartInfo.Arguments}");
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunSpirit(GetRuntimeSpiritProcess(data, tcase, et == ExperimentType.Streaming));
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
    }
}