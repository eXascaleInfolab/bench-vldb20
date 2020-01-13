using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class SoftImputeAlgorithm : Algorithm
    {
        private static bool _init = false;
        public SoftImputeAlgorithm() : base(ref _init)
        { }

        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}.txt" };
        }
        
        private static string Style => "linespoints lt 8 dt 2 lw 3 pt 5 lc rgbcolor \"dark-violet\" pointsize 1.2";

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
            RunSoftImpute(GetSoftImputeProcess(data, tcase));
        }
        
        private Process GetSoftImputeProcess(DataDescription data, int len)
        {
            Process siproc = new Process();
            
            siproc.StartInfo.WorkingDirectory = EnvPath;
            siproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            siproc.StartInfo.CreateNoWindow = true;
            siproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            siproc.StartInfo.UseShellExecute = false;

            siproc.StartInfo.Arguments = $"-alg softimpute -test o -n {data.N} -m {data.M} -k {Truncation} " +
                                         $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                         $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return siproc;
        }
        
        private Process GetRuntimeSoftImputeProcess(DataDescription data, int len)
        {
            Process siproc = new Process();
            
            siproc.StartInfo.WorkingDirectory = EnvPath;
            siproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            siproc.StartInfo.CreateNoWindow = true;
            siproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            siproc.StartInfo.UseShellExecute = false;

            siproc.StartInfo.Arguments = $"-alg softimpute -test rt -n {data.N} -m {data.M} -k {Truncation} " +
                                             $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                             $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return siproc;
        }
        private void RunSoftImpute(Process siproc)
        {
            siproc.Start();
            siproc.WaitForExit();
                
            if (siproc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] SoftImpute returned code {siproc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {siproc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunSoftImpute(GetRuntimeSoftImputeProcess(data, tcase));
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