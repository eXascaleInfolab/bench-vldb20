using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.AlgoIntegration
{
    public partial class MeanImputeMLAlgorithm : Algorithm
    {
        private static bool _init = false;
        public MeanImputeMLAlgorithm() : base(ref _init)
        { }

        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}.txt" };
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
            RunProcess(GetProcess(data, tcase, false));
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunProcess(GetProcess(data, tcase, true));
        }
        
        private Process GetProcess(DataDescription data, int len, bool runtime)
        {
            Process proc = new Process();
            
            proc.StartInfo.WorkingDirectory = EnvPath;
            proc.StartInfo.FileName = "octave-cli";
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            proc.StartInfo.UseShellExecute = false;
            
            string functionArgs = $"'{SubFolderDataIn}{data.Code}_m{len}.txt', '{SubFolderDataOut}{AlgCode}{len}.txt', {(runtime ? 1 : 0)}";

            if (proc.StartInfo.FileName.Contains("octave"))
            {
                proc.StartInfo.Arguments = $"--eval \"recovery('{AlgCode}', {functionArgs})\"";
            }
            else
            {
                proc.StartInfo.Arguments = $"-nodisplay -r \"recovery('{AlgCode}', {functionArgs})\"";
            }

            return proc;
        }
        
        private void RunProcess(Process proc)
        {
            proc.Start();
            proc.WaitForExit();
                
            if (proc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] {AlgCode} returned code {proc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {proc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }
    }
}