using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class GrouseAlgorithm : Algorithm
    {
        private static bool _init = false;
        public GrouseAlgorithm() : base(ref _init)
        { }

        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}_sparce.txt" };
        }
        
        private static string Style => "linespoints lt 8 dt 2 lw 3 pt 7 lc rgbcolor \"yellow\" pointsize 2";//todo: style

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
            RunGrouse(GetGrouseProcess(data, tcase));
        }
        
        private Process GetGrouseProcess(DataDescription data, int len)
        {
            Process grouseproc = new Process();
            
            grouseproc.StartInfo.WorkingDirectory = EnvPath;
            grouseproc.StartInfo.FileName = "octave";
            grouseproc.StartInfo.CreateNoWindow = true;
            grouseproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            grouseproc.StartInfo.UseShellExecute = false;

            // transposed matrix to meet grouse prereq
            string functionArgs = $"{len}, '{data.Code}', {data.M}, {data.N}";
            grouseproc.StartInfo.Arguments = $"--eval \"experimentRun({functionArgs}, 0)\"";

            return grouseproc;
        }
        
        private Process GetRuntimeGrouseProcess(DataDescription data, int len)
        {
            Process grouseproc = new Process();
            
            grouseproc.StartInfo.WorkingDirectory = EnvPath;
            grouseproc.StartInfo.FileName = "octave";
            grouseproc.StartInfo.CreateNoWindow = true;
            grouseproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            grouseproc.StartInfo.UseShellExecute = false;

            // transposed matrix to meet grouse prereq
            string functionArgs = $"{len}, '{data.Code}', {data.M}, {data.N}";
            grouseproc.StartInfo.Arguments = $"--eval \"experimentRun({functionArgs}, 1)\"";

            return grouseproc;
        }
        private void RunGrouse(Process grouseproc)
        {
            grouseproc.Start();
            grouseproc.WaitForExit();
                
            if (grouseproc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] GROUSE returned code {grouseproc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {grouseproc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunGrouse(GetRuntimeGrouseProcess(data, tcase));
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
            
            StringBuilder dataSparce = new StringBuilder();
            
            for (int i = rFrom; i < n; i++)
            {
                string lineSparce = "";

                for (int j = cFrom; j < m; j++)
                {
                    if (!Utils.IsMissing(missingBlocks, i, j))
                    {
                        // transposed matrix to meet grouse prereq
                        lineSparce += (j + 1) + " " + (i + 1) + " " + res[i][j] + Environment.NewLine;
                    }
                }

                dataSparce.Append(lineSparce);
            }

            string destinationSparce = EnvPath + SubFolderDataIn + $"{code}_m{tcase}_sparce.txt";
            
            if (File.Exists(destinationSparce)) File.Delete(destinationSparce);
            File.AppendAllText(destinationSparce, dataSparce.ToString());
        }
    }
}