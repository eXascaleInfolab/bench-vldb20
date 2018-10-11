using System.Collections.Generic;
using System.ComponentModel;
using System.IO;

namespace TestingFramework.Testing
{
    public static class EnumMethods
    {
        public static bool EnableStreaming = false;
        public static bool EnableContinuous = false;
        public static bool EnableMulticolumn = false;
        
        //
        // Experiments
        //
        public static string ToShortString(this Experiment ex)
        {
            switch (ex)
            {
                case Experiment.Precision: return "prec";
                case Experiment.Runtime: return "runtime";
                default: throw new InvalidDataException();
            }
        }

        public static string ToLongString(this Experiment ex)
        {
            switch (ex)
            {
                case Experiment.Precision: return "precision";
                case Experiment.Runtime: return "runtime";
                default: throw new InvalidDataException();
            }
        }

        public static IEnumerable<Experiment> AllExperiments()
        {
            yield return Experiment.Precision;
            yield return Experiment.Runtime;
        }

        //
        // Types
        //
        
        public static string ToShortString(this ExperimentType et)
        {
            switch (et)
            {
                case ExperimentType.Continuous: return "cont";
                case ExperimentType.Recovery: return "rec";
                case ExperimentType.Streaming: return "str";
                default: throw new InvalidDataException();
            }
        }

        public static string ToLongString(this ExperimentType et)
        {
            switch (et)
            {
                case ExperimentType.Continuous: return "continuous";
                case ExperimentType.Recovery: return "recovery";
                case ExperimentType.Streaming: return "streaming";
                default: throw new InvalidDataException();
            }
        }

        public static IEnumerable<ExperimentType> AllExperimentTypes()
        {
            yield return ExperimentType.Recovery;
            if (EnableContinuous) yield return ExperimentType.Continuous;
            if (EnableStreaming) yield return ExperimentType.Streaming;
        }
        
        //
        // Scenarios
        //

        public static string ToShortString(this ExperimentScenario es)
        {
            switch (es)
            {
                case ExperimentScenario.Missing: return "mis";
                case ExperimentScenario.Length: return "len";
                case ExperimentScenario.Columns: return "col";
                case ExperimentScenario.MissingMultiColumn: return "mmc";
                default: throw new InvalidDataException();
            }
        }
        
        public static string ToLongString(this ExperimentScenario es)
        {
            switch (es)
            {
                case ExperimentScenario.Missing: return "missingpercentage";
                case ExperimentScenario.Length: return "length";
                case ExperimentScenario.Columns: return "columns";
                case ExperimentScenario.MissingMultiColumn: return "missingmulticolumn";
                default: throw new InvalidDataException();
            }
        }
        
        public static string ToDescriptiveString(this ExperimentScenario es)
        {
            switch (es)
            {
                case ExperimentScenario.Missing: return "number of missing values";
                case ExperimentScenario.Length: return "number of rows";
                case ExperimentScenario.Columns: return "number of columns";
                case ExperimentScenario.MissingMultiColumn: return "number of missing values";
                default: throw new InvalidDataException();
            }
        }

        public static IEnumerable<ExperimentScenario> AllExperimentScenarios()
        {
            yield return ExperimentScenario.Missing;
            if (EnableMulticolumn) yield return ExperimentScenario.MissingMultiColumn;
            yield return ExperimentScenario.Length;
            yield return ExperimentScenario.Columns;
        }
    }
    public enum Experiment
    {
        Precision, Runtime
    }
    
    public enum ExperimentType
    {
        Continuous, Recovery, Streaming
    }

    public enum ExperimentScenario
    {
        Length, Missing, Columns, MissingMultiColumn
    }

    [ImmutableObject(true)]
    public struct DataDescription
    {
        public readonly int N;
        public readonly int M;
        public readonly (int, int, int)[] MissingBlocks;
        public readonly string Code;

        public DataDescription(int n, int m, (int, int, int)[] missingBlocks, string code)
        {
            N = n;
            M = m;
            MissingBlocks = missingBlocks;
            Code = code;
        }
    }
}