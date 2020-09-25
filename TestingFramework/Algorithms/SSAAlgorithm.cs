using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class SSAAlgorithm : Algorithm
    {
        private static bool _init = false;
        public SSAAlgorithm() : base(ref _init)
        { }

        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}.txt", $"{dataCode})_m{tcase}_ts.txt" };
        }
        
        private static string Style => "linespoints lt 8 dt 1 lw 2 pt 1 lc rgbcolor \"black\" pointsize 1.2";

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
            RunSSA(GetSSAProcess(data, tcase));
        }
        
        private Process GetSSAProcess(DataDescription data, int len)
        {
            Process ssaproc = new Process();
            
            ssaproc.StartInfo.WorkingDirectory = EnvPath;
            ssaproc.StartInfo.FileName = "python";
            ssaproc.StartInfo.CreateNoWindow = true;
            ssaproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            ssaproc.StartInfo.UseShellExecute = false;
            
            string functionArgs = $"\"{SubFolderDataIn}{data.Code}_m{len}_ts.txt\", \"{SubFolderDataIn}{data.Code}_m{len}.txt\", \"{SubFolderDataOut}{AlgCode}{len}.txt\", 0, 2";

            ssaproc.StartInfo.Arguments = "-c 'from testScriptSynthControlALS import rmv_mydata; " +
                                          $"rmv_mydata({functionArgs})'";

            return ssaproc;
        }
        
        private Process GetRuntimeSSAProcess(DataDescription data, int len)
        {
            Process ssaproc = new Process();
            
            ssaproc.StartInfo.WorkingDirectory = EnvPath;
            ssaproc.StartInfo.FileName = "python";
            ssaproc.StartInfo.CreateNoWindow = true;
            ssaproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            ssaproc.StartInfo.UseShellExecute = false;
            
            string functionArgs = $"\"{SubFolderDataIn}{data.Code}_m{len}_ts.txt\", \"{SubFolderDataIn}{data.Code}_m{len}.txt\", \"{SubFolderDataOut}{AlgCode}{len}.txt\", 1, 2";

            ssaproc.StartInfo.Arguments = "-c 'from testScriptSynthControlALS import rmv_mydata; " +
                                          $"rmv_mydata({functionArgs})'";

            return ssaproc;
        }
        private void RunSSA(Process ssaproc)
        {
            ssaproc.Start();
            ssaproc.WaitForExit();
                
            if (ssaproc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] SSA returned code {ssaproc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {ssaproc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunSSA(GetRuntimeSSAProcess(data, tcase));
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
            
            IEnumerable<IEnumerable<string>> matrixTrspStr =
                res.EnumerateTranspose()
                    .AddNumeration()
                    .SelectWithNumbers((x, i) =>
                        x.Select((y, j) => Utils.IsMissing(missingBlocks, j, i) ? Double.NaN : y)
                    )
                    .SelectCollapse((list, j) => 
                            list.AddNumeration()
                                .SelectCollapse((x, i) => $"{$"TS{j}".Enquote()},{i},{x}")//"TS0",1956,-0.80636
                    );

            File.WriteAllLines(EnvPath + SubFolderDataIn + $"{code}_m{tcase}_ts.txt",
                new[] { $"{"".Enquote()},{"ts".Enquote()},{"time".Enquote()},{"val".Enquote()}" }//singleton array
                    .Concat(
                        matrixTrspStr
                            .Flatten()
                            .AddNumeration(1)
                            .SelectCollapse((x, i) => $"{i.Enquote()},{x}"))
            );
        }
    }
}