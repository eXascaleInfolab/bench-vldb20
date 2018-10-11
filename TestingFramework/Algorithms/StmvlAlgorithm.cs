using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
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
        
        private static string Style => "linespoints lt 8 dt 3 lw 3 pt 5 lc rgbcolor \"cyan\" pointsize 2";
        
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
            stmvlproc.StartInfo.FileName = "mono";
            stmvlproc.StartInfo.CreateNoWindow = true;
            stmvlproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            stmvlproc.StartInfo.UseShellExecute = false;

            //necessary to send a key to circumvent ReadKey() in main
            stmvlproc.StartInfo.RedirectStandardInput = true;

            string cli = $"{data.Code} {finalN} {finalM} {len} nonrt";

            stmvlproc.StartInfo.Arguments = "STMVL.exe " + cli;

            return stmvlproc;
        }

        private Process GetRuntimeStmvlProcess(int finalN, int finalM, DataDescription data, int len)
        {
            Process stmvlproc = new Process();

            stmvlproc.StartInfo.WorkingDirectory = EnvPath;
            stmvlproc.StartInfo.FileName = "mono";
            stmvlproc.StartInfo.CreateNoWindow = true;
            stmvlproc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            stmvlproc.StartInfo.UseShellExecute = false;

            //necessary to send a key to circumvent ReadKey() in main
            stmvlproc.StartInfo.RedirectStandardInput = true;

            string cli = $"{data.Code} {finalN} {finalM} {len} rt";

            stmvlproc.StartInfo.Arguments = "STMVL.exe " + cli;

            return stmvlproc;
        }
        
        private void RunStmvl(Process stmvlproc)
        {
            stmvlproc.Start();
            stmvlproc.StandardInput.Write(Environment.NewLine);
            stmvlproc.WaitForExit();
                
            if (stmvlproc.ExitCode != 0) Console.WriteLine($"[WARNING] STMVL returned code {stmvlproc.ExitCode} on exit.{Environment.NewLine}" +
                                                        $"CLI args: {stmvlproc.StartInfo.Arguments}");
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            RunStmvl(GetRuntimeStmvlProcess(data.N, data.M, data, tcase));
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
            
            var sensors = new List<string>();
            var dataHeader = new StringBuilder("datetime");

            // transform \\

            decimal lat = new Decimal(39.954047);
            decimal lng = new Decimal(116.348991);

            for (int i = cFrom; i < m; i++)
            {
                // time series - generate sensors with coordinates
                int sensorId = 1000 + i;
                sensors.Add(sensorId + "," + lat + "," + lng);
                dataHeader.Append(",").Append(sensorId);
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

            dataHeader.Append(Environment.NewLine);

            var dataMissing = new List<string>();
            var temporal = new DateTime(2014, 05, 01, 00, 00, 00);

            for (int j = rFrom; j < n; j++)
            {
                temporal = temporal.AddHours(1);

                string lineMissing = temporal.ToString(@"yyyy\/MM\/yy HH:mm:ss") + ",";

                for (int i = cFrom; i < m; i++)
                {
                    // data with a missing block
                    if (Utils.IsMissing(missingBlocks, j, i))
                    {
                        // missing
                        lineMissing += ",";
                    }
                    else
                    {
                        lineMissing += res[j][i] + ",";
                    }
                }

                lineMissing = lineMissing.Trim(',');
                dataMissing.Add(lineMissing);
            }

            // dump \\
            string destinationMissing = EnvPath + SubFolderDataIn + $"{code}_m{tcase}_missing.txt";
            string destinationLatlng = EnvPath + SubFolderDataIn + $"{code}_m{tcase}_latlng.txt";

            // latlng
            if (File.Exists(destinationLatlng)) File.Delete(destinationLatlng);
            File.AppendAllText(destinationLatlng, "sensor_id,latitude,longitude" + Environment.NewLine);
            File.AppendAllLines(destinationLatlng, sensors);

            // data
            if (File.Exists(destinationMissing)) File.Delete(destinationMissing);
            File.AppendAllText(destinationMissing, dataHeader.ToString());
            File.AppendAllLines(destinationMissing, dataMissing);
        }
    }
}