using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public partial class TkcmAlgorithm : Algorithm
    {
        private static bool _init = false;
        public TkcmAlgorithm() : base(ref _init)
        { }
        
        private static string Style => "linespoints lt 8 dt 4 lw 3 pt 4 lc rgbcolor \"green\" pointsize 1";
        
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
             RunTkcm(GetTkcmProcess(data, tcase, es));
        }
        
        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            if (et == ExperimentType.Streaming)
            {
                RunTkcm(GetStreamingTkcmProcess(data, tcase, es));
            }
            else
            {
                RunTkcm(GetRuntimeTkcmProcess(data, tcase, es));
            }
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

        private Process GetTkcmProcess(DataDescription data, int len, ExperimentScenario es)
        {
            Process tkcmproc = new Process();
            
            tkcmproc.StartInfo.WorkingDirectory = EnvPath;
            tkcmproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            tkcmproc.StartInfo.CreateNoWindow = true;
            tkcmproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            tkcmproc.StartInfo.UseShellExecute = false;
            
            tkcmproc.StartInfo.Arguments = $"-alg tkcm -test o -n {data.N} -m {data.M} -k {ParamL} " +
                                           $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                           $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return tkcmproc;
        }
        
        private Process GetRuntimeTkcmProcess(DataDescription data, int len, ExperimentScenario es)
        {
            Process tkcmproc = new Process();
            
            tkcmproc.StartInfo.WorkingDirectory = EnvPath;
            tkcmproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            tkcmproc.StartInfo.CreateNoWindow = true;
            tkcmproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            tkcmproc.StartInfo.UseShellExecute = false;
            
            tkcmproc.StartInfo.Arguments = $"-alg tkcm -test rt -n {data.N} -m {data.M} -k {ParamL} " +
                                           $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                           $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return tkcmproc;
        }
        
        private Process GetStreamingTkcmProcess(DataDescription data, int len, ExperimentScenario es)
        {
            Process tkcmproc = new Process();
            
            tkcmproc.StartInfo.WorkingDirectory = EnvPath;
            tkcmproc.StartInfo.FileName = "python";
            tkcmproc.StartInfo.CreateNoWindow = true;
            tkcmproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            tkcmproc.StartInfo.UseShellExecute = false;

            //def experimentRun(n, m, code, missing, missing_start, tcase):
            string functionArgs = $"{data.N}, {data.M}, \"{data.Code}\", {data.MissingBlocks[0].Item3}, {data.MissingBlocks[0].Item2}, {len}";
            
            tkcmproc.StartInfo.Arguments = "-c 'from _tests.testscript import streamingExperimentRun; " +
                                           $"streamingExperimentRun({functionArgs})'";

            return tkcmproc;
        }
        
        private void RunTkcm(Process tkcmproc)
        {
            tkcmproc.Start();
            tkcmproc.WaitForExit();

            if (tkcmproc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] TKCM returned code {tkcmproc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {tkcmproc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }
    }
}