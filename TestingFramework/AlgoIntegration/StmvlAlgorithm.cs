using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.AlgoIntegration
{
    public partial class StmvlAlgorithm : Algorithm
    {
        private static bool _init = false;
        public StmvlAlgorithm() : base(ref _init)
        { }
        
        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}_latlng.txt", $"{dataCode}_m{tcase}_missing.txt" };
        }
        
        private static string Style => "linespoints lt 8 dt 2 lw 3 pt 8 lc rgbcolor \"green\" pointsize 1.2";
        
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
            RunStmvl(GetStmvlProcess(data.N, data.M, data, tcase));
        }
        
        private Process GetStmvlProcess(int finalN, int finalM, DataDescription data, int len)
        {
            Process stmvlproc = new Process();

            stmvlproc.StartInfo.WorkingDirectory = EnvPath;
            stmvlproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            stmvlproc.StartInfo.CreateNoWindow = true;
            stmvlproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            stmvlproc.StartInfo.UseShellExecute = false;

            stmvlproc.StartInfo.Arguments = $"-alg st-mvl -test o -n {data.N} -m {data.M} -k {(int)(Alpha * 1000.0)} " +
                                            $"-xtra {SubFolderDataIn}{data.Code}_m{len}_latlng.txt " +
                                            $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                            $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return stmvlproc;
        }

        private Process GetRuntimeStmvlProcess(int finalN, int finalM, DataDescription data, int len)
        {
            Process stmvlproc = new Process();

            stmvlproc.StartInfo.WorkingDirectory = EnvPath;
            stmvlproc.StartInfo.FileName = EnvPath + "../cmake-build-debug/algoCollection";
            stmvlproc.StartInfo.CreateNoWindow = true;
            stmvlproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            stmvlproc.StartInfo.UseShellExecute = false;

            stmvlproc.StartInfo.Arguments = $"-alg st-mvl -test rt -n {data.N} -m {data.M} -k {(int)(Alpha * 1000.0)} " +
                                            $"-xtra {SubFolderDataIn}{data.Code}_m{len}_latlng.txt " +
                                            $"-in ./{SubFolderDataIn}{data.Code}_m{len}.txt " +
                                            $"-out ./{SubFolderDataOut}{AlgCode}{len}.txt";

            return stmvlproc;
        }
        
        private void RunStmvl(Process stmvlproc)
        {
            stmvlproc.Start();
            //stmvlproc.StandardInput.Write(Environment.NewLine); not in c++ version
            stmvlproc.WaitForExit();
                
            if (stmvlproc.ExitCode != 0) Console.WriteLine($"[WARNING] STMVL returned code {stmvlproc.ExitCode} on exit.{Environment.NewLine}" +
                                                        $"CLI args: {stmvlproc.StartInfo.Arguments}");
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunStmvl(GetRuntimeStmvlProcess(data.N, data.M, data, tcase));
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
            var sensors = new List<string>();
            
            decimal lat = new Decimal(39.954047);
            decimal lng = new Decimal(116.348991);

            for (int i = cFrom; i < m; i++)
            {
                // time series - generate sensors with coordinates
                int sensorId = 1000 + i;
                sensors.Add(sensorId + "," + lat + "," + lng);
                // variate coordinates
                if (i % 2 == 0)
                {
                    lat += new Decimal(0.01);
                }
                else
                {
                    lng += new Decimal(0.01);   
                }
            }
            
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
            string destinationLatlng = EnvPath + SubFolderDataIn + $"{code}_m{tcase}_latlng.txt";

            // latlng
            if (File.Exists(destinationLatlng)) File.Delete(destinationLatlng);
            File.AppendAllText(destinationLatlng, "sensor_id,latitude,longitude" + Environment.NewLine);
            File.AppendAllLines(destinationLatlng, sensors);
            
            // data
            if (File.Exists(destination)) File.Delete(destination);
            File.AppendAllText(destination, data.ToString());
        }
    }
}