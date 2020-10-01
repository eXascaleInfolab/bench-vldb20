using System;
using System.Collections.Generic;
using System.Diagnostics;

using TestingFramework.Testing;

namespace TestingFramework.AlgoIntegration
{
    public partial class LinearImputeAlgorithm : Algorithm
    {
        private static bool _init = false;
        public LinearImputeAlgorithm() : base(ref _init)
        { }
        
        private static string Style => "linespoints lt 8 dt 2 lw 3 pt 1 lc rgbcolor \"black\" pointsize 1.2";

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms() =>
            new[] { new SubAlgorithm($"{AlgCode}", String.Empty, Style) };
        

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms(int tcase) =>
            new[] { new SubAlgorithm($"{AlgCode}", $"{AlgCode}{tcase}", Style) };
        
        protected override void PrecisionExperiment(ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
            RunAlgortithm(GetProcess(data, tcase));
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunAlgortithm(GetRuntimeProcess(data, tcase));
        }

        private Process GetProcess(DataDescription data, int len)
        {
            Process proc = new Process();
            
            proc.StartInfo.WorkingDirectory = EnvPath;
            proc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            proc.StartInfo.UseShellExecute = false;

            proc.StartInfo.Arguments = $"-alg linimp -test o -n {data.N} -m {data.M} " +
                                         $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                         $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return proc;
        }
        
        private Process GetRuntimeProcess(DataDescription data, int len)
        {
            Process proc = new Process();
            
            proc.StartInfo.WorkingDirectory = EnvPath;
            proc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            proc.StartInfo.UseShellExecute = false;

            proc.StartInfo.Arguments = $"-alg linimp -test rt -n {data.N} -m {data.M} " +
                                             $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                             $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return proc;
        }
        private void RunAlgortithm(Process proc)
        {
            proc.Start();
            proc.WaitForExit();
                
            if (proc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] LinearImpute returned code {proc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {proc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
        }
    }
}