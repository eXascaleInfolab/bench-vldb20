using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class TrmfAlgorithm : Algorithm
    {
        private static bool _init = false;
        public TrmfAlgorithm() : base(ref _init)
        { }

        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}_obs.txt", $"{dataCode}_m{tcase}_indic.txt" };
        }
        
        private static string Style => "linespoints lt 8 dt 2 lw 3 pt 9 lc rgbcolor \"orange\" pointsize 1.2";

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
            RunTrmf(GetTrmfProcess(data, tcase));
        }
        
        private Process GetTrmfProcess(DataDescription data, int len)
        {
            Process trmfproc = new Process();
            
            trmfproc.StartInfo.WorkingDirectory = EnvPath;
            trmfproc.StartInfo.FileName = "octave-cli";
            trmfproc.StartInfo.CreateNoWindow = true;
            trmfproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            trmfproc.StartInfo.UseShellExecute = false;

            string functionArgs = $"{len}, '{data.Code}'";
            trmfproc.StartInfo.Arguments = $"--eval \"experimentRun({functionArgs}, 0)\"";

            return trmfproc;
        }
        
        private Process GetRuntimeTrmfProcess(DataDescription data, int len)
        {
            Process trmfproc = new Process();
            
            trmfproc.StartInfo.WorkingDirectory = EnvPath;
            trmfproc.StartInfo.FileName = "octave-cli";
            trmfproc.StartInfo.CreateNoWindow = true;
            trmfproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            trmfproc.StartInfo.UseShellExecute = false;

            string functionArgs = $"{len}, '{data.Code}'";
            trmfproc.StartInfo.Arguments = $"--eval \"experimentRun({functionArgs}, 1)\"";

            return trmfproc;
        }
        private void RunTrmf(Process trmfproc)
        {
            trmfproc.Start();
            trmfproc.WaitForExit();
                
            if (trmfproc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] TRMF returned code {trmfproc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {trmfproc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunTrmf(GetRuntimeTrmfProcess(data, tcase));
        }

        public override void GenerateData(string sourceFile, string code, int tcase, (int, int, int)[] missingBlocks, (int, int) rowRange,
            (int, int) columnRange)
        {
            sourceFile = DataWorks.FolderData + sourceFile;
            
            (int rFrom, int rTo) = rowRange;
            (int cFrom, int cTo) = columnRange;
            
            double[][] res = DataWorks.GetDataLimited(sourceFile, rTo - rFrom, cTo - cFrom);
            
            int n = rTo > res.Length ? res.Length : rTo;
            int m = cTo > res[0].Length ? res[0].Length : cTo;
            
            StringBuilder dataObs = new StringBuilder();
            StringBuilder dataIndic = new StringBuilder();
            
            for (int i = rFrom; i < n; i++)
            {
                string lineObs = "";
                string lineIndic = "";

                for (int j = cFrom; j < m; j++)
                {
                    // a copy with a missing block
                    if (Utils.IsMissing(missingBlocks, i, j))
                    {
                        lineIndic += "0 ";
                        lineObs += 0.01 + " "; // zeroes in the missing blocks give zero division in algorithms' internal MSE calc
                    }
                    else
                    {
                        lineIndic += "1 ";
                        lineObs += res[i][j] + " ";
                    }
                }

                lineObs = lineObs.Trim();
                lineObs += Environment.NewLine;

                lineIndic = lineIndic.Trim();
                lineIndic += Environment.NewLine;

                dataObs.Append(lineObs);
                dataIndic.Append(lineIndic);
            }

            string destinationObs = EnvPath + SubFolderDataIn + $"{code}_m{tcase}_obs.txt";
            string destinationIndic = EnvPath + SubFolderDataIn + $"{code}_m{tcase}_indic.txt";
            
            if (File.Exists(destinationObs)) File.Delete(destinationObs);
            File.AppendAllText(destinationObs, dataObs.ToString());
            
            if (File.Exists(destinationIndic)) File.Delete(destinationIndic);
            File.AppendAllText(destinationIndic, dataIndic.ToString());
        }
    }
}