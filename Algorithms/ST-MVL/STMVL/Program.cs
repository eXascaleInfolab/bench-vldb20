using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STMVL
{
    class Program
    {
        static void Main(string[] args)
        {
            Trace.TraceInformation("");
            Trace.TraceInformation("Start new iteator, at... ");
            Trace.Flush();

            //verify();
            //stmv();
            TestScenario(args);
            Trace.Flush();

            Console.WriteLine("finish");
            Console.ReadKey(true);
        }

        public static void verify()
        {
            // row means timestamp
            // column means sensors
            // pls refer to the sample data
            // we leave the missing as blank.
            // When wrapping code, we fix some small bugs, finding a better result than paper.

            string missingFile = @"SampleData/pm25_missing.txt";
            string groundFile = @"SampleData/pm25_ground.txt";
            string predictFile = "pm25_predict.txt";
            string latlngFile = @"SampleData/pm25_latlng.txt";

            int windowSize = 7;
            double alpha = 4;
            double gamma = 0.85;

            int rowCount = 8759; // count of timestamps
            int colCount = 36; // count of sensors

            bool isBlockMissing = true; //for block missing problem, if exist, set true.

            STMVL algorithm = new STMVL(missingFile, latlngFile, rowCount, colCount, alpha, gamma, windowSize, true);
            algorithm.Run(predictFile, isBlockMissing);

            Trace.TraceInformation("Evaluate... " + DateTime.Now);
            algorithm.Evaluate(groundFile);

            Console.WriteLine(DateTime.Now);
        }

        public static IEnumerable<int> ClosedSequence(int start, int end, int step)
        {
            for (int i = start; i <= end; i += step)
            {
                yield return i;
            }
        }

        private static void TestScenario(string[] args)
        {
            if (args.Length > 0)
            {
                string code = args[0];
                int Nlen = Int32.Parse(args[1]);
                int Mlen = Int32.Parse(args[2]);
                int tcase = Int32.Parse(args[3]);
                bool runtime = args[4] == "rt";

                Console.WriteLine($"ARGS PARSED: code={code}; N={Nlen}; M={Mlen}; tcase={tcase}; runtime={runtime}");

                if (runtime)
                    TestScenarioRt(code, Nlen, Mlen, tcase);
                else
                    TestScenarioAny(code, Nlen, Mlen, tcase);
            }
            else
            {
                Environment.Exit(-1);
            }

            //TestScenarioMissing();
            //TestScenarioLength();
            //TestScenarioColumns();

        }

        private static void TestScenarioAny(string code, int Nlen, int Mlen, int tcase)
        {
            string filename = code + "_m{tcase}_";

            string baseFile = "in/" + filename.Replace("{tcase}", tcase.ToString());
            string baseOut = $@"out/stmvl{tcase}";

            int window = 3;
            stmv(Nlen, Mlen, window, baseFile, baseOut);
        }

        private static void TestScenarioRt(string code, int Nlen, int Mlen, int tcase)
        {
            string filename = code + "_m{tcase}_";

            string baseFile = "in/" + filename.Replace("{tcase}", tcase.ToString());
            string baseOut = $@"out/stmvl{tcase}";

            int window = 3;
            Stopwatch sw = new Stopwatch();
            stmv(Nlen, Mlen, window, baseFile, baseOut, sw);

            string predictFile = "Data/" + baseOut + ".txt";
            if (System.IO.File.Exists(predictFile))
            {
                System.IO.File.Delete(predictFile);
            }
            System.IO.File.WriteAllText(predictFile, sw.Elapsed.TotalMilliseconds.ToString());
        }

        [Obsolete]
        private static void TestScenarioMissing()
        {
            // params
            int[] testCases = ClosedSequence(100, 900, 100).ToArray();
            string code = "climate";

            string folder = "";
            string filename = code + "_m{tcase}_";

            //// static [can be optional] ////
            int rows = 1000;
            int cols = 12;

            if (code == "bball" || code == "meteo")
                cols = 4;

            foreach (int tcase in testCases)
            {
                string baseFile = folder + filename.Replace("{tcase}", tcase.ToString());
                string baseOut = folder + $@"out/stmvl{tcase}";

                int window = 3;
                stmv(rows, cols, window, baseFile, baseOut);
            }
        }

        [Obsolete]
        private static void TestScenarioLength()
        {
            // params
            int[] testCases = ClosedSequence(200, 2000, 200).ToArray();
            string code = "climate";

            string folder = "";
            string filename = code + "_m{tcase}_";

            //// static [can be optional] ////
            int rows = 1000;
            int cols = 12;

            if (code == "bball" || code == "meteo")
                cols = 4;

            foreach (int tcase in testCases)
            {
                string baseFile = folder + filename.Replace("{tcase}", tcase.ToString());
                string baseOut = folder + $@"out/stmvl{tcase}";

                int window = 3;
                stmv(tcase, cols, window, baseFile, baseOut);
            }
        }

        [Obsolete]
        private static void TestScenarioColumns()
        {
            // params
            int[] testCases = ClosedSequence(4, 12, 1).ToArray();
            string code = "climate";

            string folder = "in/";
            string filename = code + "_m{tcase}_";

            //// static [can be optional] ////
            int rows = 1000;
            int cols = 12;

            foreach (int tcase in testCases)
            {
                string baseFile = folder + filename.Replace("{tcase}", tcase.ToString());
                string baseOut = $@"out/stmvl{tcase}";

                int window = 3;
                stmv(rows, tcase, window, baseFile, baseOut);
            }
        }

        /*            /////\\\\\           */
        /*           //////\\\\\\          */
        /*          ///////\\\\\\\         */
        /*         ////////\\\\\\\\        */
        /*        /////////\\\\\\\\\       */

        // parametric

        public static void stmv(int rowCount, int colCount, int windowSize, string baseFile, string baseOut)
        {
            stmv(rowCount, colCount, windowSize, baseFile, baseOut, null);
        }

        public static void stmv(int rowCount, int colCount, int windowSize, string baseFile, string baseOut, Stopwatch sw)
        {
            // Fill missing for whole dataset
            string missingFile = "Data/" + baseFile + "missing.txt";
            string latlngFile = "Data/" + baseFile + "latlng.txt";

            string predictFile = "Data/" + baseOut + ".txt";

            double alpha = 4;
            double gamma = 0.85;

            bool isInitialize = true;  //for block missing problem

            STMVL algorithm = new STMVL(missingFile, latlngFile, rowCount, colCount, alpha, gamma, windowSize);

            if (sw != null) sw.Start();

            algorithm.Run(predictFile, isInitialize);

            if (sw != null) sw.Stop();

            Console.WriteLine(DateTime.Now);
        }

        public static void stmv()
        {
            // Fill missing for whole dataset

            int rowCount = 10000; //1907; //8700; //8759; // count of timestamps
            int colCount = 12; //10; //36; // count of sensors

            string baseFile = $@"MyData/bafu_m200_";

            string missingFile = baseFile + "missing.txt";
            string predictFile = baseFile + "predict.txt";
            string latlngFile = baseFile + "latlng.txt";

            int windowSize = 8;
            double alpha = 4;
            double gamma = 0.85;

            bool isInitialize = true;  //for block missing problem

            STMVL algorithm = new STMVL(missingFile, latlngFile, rowCount, colCount, alpha, gamma, windowSize);
            algorithm.Run(predictFile, isInitialize);

            Console.WriteLine(DateTime.Now);
        }
    }
}
