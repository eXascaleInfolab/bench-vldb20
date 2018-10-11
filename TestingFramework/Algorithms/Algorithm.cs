using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public abstract class Algorithm
    {
        // constructor
        protected Algorithm(ref bool init)
        {
            if (init)
            {
                throw new Exception("Second instance per algorithm is not allowed");
            }
            init = true;
            
            CheckInitValues();
        }

        private void CheckInitValues()
        {
            if (SubFolderDataIn == "" || SubFolderDataIn == "." || SubFolderDataIn == "/" || SubFolderDataIn == "./"
                || SubFolderDataOut == "" || SubFolderDataOut == "." || SubFolderDataOut == "/" || SubFolderDataOut == "./"
                || SubFolderDataIn == SubFolderDataOut)
            {
                throw new ApplicationException($"Wrong hardcoded paths for {AlgCode}, DataIn/Out can't be the same folder as EnvPath or each other.");
            }

            if (!EnvPath.EndsWith("/") || !SubFolderDataIn.EndsWith("/") || !SubFolderDataOut.EndsWith("/"))
            {
                throw new ApplicationException("Violation of folder name convention. Folder names should have a trailing forward slash.");
            }
        }
        
        // Fields
        public abstract string AlgCode { get; }
        
        protected abstract string _EnvPath { get; }
        protected string EnvPath => _EnvPath.Replace("~", Environment.GetFolderPath(Environment.SpecialFolder.Personal));
        
        protected abstract string SubFolderDataIn { get; }
        protected abstract string SubFolderDataOut { get; }
        
        public virtual bool IsPlottable => true;

        // virtual functions
        public virtual string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            return new[] { $"{dataCode}_m{tcase}.txt" };
        }

        public virtual string[] EnumerateOutputFiles(int tcase)
        {
            return new[] { $"{AlgCode}{tcase}.txt" };
        }

        public abstract IEnumerable<SubAlgorithm> EnumerateSubAlgorithms();

        public abstract IEnumerable<SubAlgorithm> EnumerateSubAlgorithms(int tcase);
        
        // Abstract functions
        protected abstract void PrecisionExperiment(ExperimentType et, ExperimentScenario es, DataDescription data, int tcase);
        protected abstract void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data, int tcase);

        public abstract void GenerateData(string sourceFile, string code, int tcase, (int, int, int)[] missingBlocks,
            (int, int) rowRange, (int, int) columnRange);
        
        // Concrete functions, work regardless of algorithm
        public void WriteDataIn(string sourceFolder, params string[] files)
        {
            foreach (var file in files)
            {
                string src = sourceFolder + file;
                string dest = EnvPath + SubFolderDataIn + file;
                
                if (File.Exists(dest))
                {
                    File.Delete(dest);
                }
                
                File.Copy(src, dest);
            }
        }
        
        public void CollectResults(Experiment ex, string destFolder, params string[] files)
        {
            switch (ex)
            {
                case Experiment.Precision:
                    foreach (var file in files)
                    {
                        string src = EnvPath + SubFolderDataOut + file;
                        string dest = destFolder + file;

                        if (File.Exists(dest))
                        {
                            File.Delete(dest);
                        }

                        File.Copy(src, dest);
                    }
                    break;
                
                case Experiment.Runtime:
                    foreach (var file in files)
                    {
                        string src = EnvPath + SubFolderDataOut + file;
                        string dest = destFolder + file;

                        if (File.Exists(dest))
                        {
                            File.Delete(dest);
                        }

                        File.Copy(src, dest);
                    }
                    break;
            }
        }
        
        public void DataCleanUp()
        {
            string[] filesIn = Directory.GetFiles(EnvPath + SubFolderDataIn);
            string[] filesOut = Directory.GetFiles(EnvPath + SubFolderDataOut);
            
            filesIn.Where(x => !x.EndsWith(".gitkeep")).ForEach(File.Delete);
            filesOut.Where(x => !x.EndsWith(".gitkeep")).ForEach(File.Delete);
        }
        
        public void RunExperiment(Experiment ex, ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
            switch (ex)
            {
                case Experiment.Precision:
                    PrecisionExperiment(et, es, data, tcase);
                    break;

                case Experiment.Runtime:
                    RuntimeExperiment(et, es, data, tcase);
                    break;
                
                default:
                    throw new ArgumentException("Provided arguments don't make for a valid experiment");
            }
        }
    }

    [ImmutableObject(true)]
    public class SubAlgorithm
    {
        public readonly string Code;
        public readonly string CaseCode;
        public readonly string Style;

        public SubAlgorithm(string code, string caseCode, string style)
        {
            Code = code;
            CaseCode = caseCode;
            Style = style;
        }
    }
}