using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace TestingFramework.Testing
{
    internal static class Utils
    {
        public static readonly Queue<string> DelayedWarnings = new Queue<string>();
        
        public static Tuple<string, string> SplitFileExtention(string fileName)
        {
            // edge case
            if (!fileName.Contains('.'))
            {
                return new Tuple<string, string>(fileName, "");
            }

            int index;
            int start = 0;
            while ((index = fileName.IndexOf('.', start)) != -1)
            {
                start = index + 1;
            }
            return new Tuple<string, string>(fileName.Substring(0, start - 1), fileName.Substring(start));
        }

        public static Dictionary<string, string> ReadConfigFile(string configFile = "../../config.cfg")
        {
            var dict = new Dictionary<string, string>();

            IEnumerable<string> lines = File.ReadAllLines(configFile)
                .Where(s => !String.IsNullOrWhiteSpace(s))
                .Where(s => !s.StartsWith("#"));

            foreach (string line in lines)
            {
                string[] keyvalue = line.Split('=');
                
                if (keyvalue.Length < 2)
                    throw new InvalidProgramException("Incorrect config file: contains an entry isn't of the format Key=Value");
                
                dict.Add(keyvalue[0].Trim(), keyvalue.Skip(1).StringJoin("=").Trim());
            }

            return dict;
        }

        /// <summary>
        /// Determines if (<paramref name="i"/>, <paramref name="j"/>) is inside one of the <paramref name="missingBlocks"/>.
        /// </summary>
        /// <param name="missingBlocks">An array of tuples {column index, starting row, size} which describe missing blocks.</param>
        /// <param name="i">Row index.</param>
        /// <param name="j">Column index.</param>
        public static bool IsMissing(ValueTuple<int, int, int>[] missingBlocks, int i, int j)
        {
            return missingBlocks.Any(mblock => j == mblock.Item1 && i >= mblock.Item2 && i < mblock.Item2 + mblock.Item3);
        }

        /// <summary>
        /// Determines if index <paramref name="i"/> is inside any of the <paramref name="missingBlocks"/>.
        /// </summary>
        /// <param name="missingBlocks">An array of tuples {column index, starting row, size} with describe missing blocks.</param>
        /// <param name="i">Row or column index.</param>
        /// <param name="isRow">If the index is row (true, default) or column (false).</param>
        public static bool IsMissing(ValueTuple<int, int, int>[] missingBlocks, int i, bool isRow = true)
        {
            foreach (var mblock in missingBlocks)
            {
                if (isRow)
                {
                    if (i >= mblock.Item2 && i < mblock.Item2 + mblock.Item3)
                    {
                        return true;
                    }
                }
                else
                {
                    if (i == mblock.Item1)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        /// <summary>
        /// Performs an <paramref name="action"/> on every member of the collection.
        /// </summary>
        /// <param name="action">Action to perform on the collection elements</param>
        public static void ForEach<T>(this IEnumerable<T> ienum, Action<T> action)
        {
            foreach (T item in ienum)
            {
                action(item);
            }
        }
        
        /// <summary>
        /// Performs an <paramref name="action"/> on every member of the collection and its index.
        /// </summary>
        /// <param name="action">Action to perform on the collection elements and their index</param>
        public static void ForEach<T>(this IEnumerable<T> ienum, Action<int, T> action)
        {
            int counter = 0;
            foreach (T item in ienum)
            {
                action(counter, item);
                counter++;
            }
        }
        
        public static ValueTuple<int, int, int>[] MissingValuesNone()
        {
            return new ValueTuple<int, int, int>[0];
        }

        /// <summary>
        /// Replaces all occurences of certain substrings by matching replacements provided in <paramref name="sourceReplacement"/>.
        /// </summary>
        /// <param name="fileInput">File to read.</param>
        /// <param name="fileOutput">File to output after replacement.</param>
        /// <param name="sourceReplacement">Tuples of (sourceText, replacementText) to be applied to the file.</param>
        public static void FileFindAndReplace(string fileInput, string fileOutput,
                                              params (string, string)[] sourceReplacement)
        {
            var fsin = new StreamReader(File.OpenRead(fileInput));

            if (File.Exists(fileOutput)) File.Delete(fileOutput);
            var fsout = new StreamWriter(File.OpenWrite(fileOutput));

            while (!fsin.EndOfStream)
            {
                string line = fsin.ReadLine() ?? "";
                foreach (var replacementPair in sourceReplacement)
                {
                    line = line.Replace(replacementPair.Item1, replacementPair.Item2);
                }
                fsout.WriteLine(line);
            }

            fsout.Flush();
            fsout.Close();

            fsin.Close();
        }

        /// <summary>
        /// Generates a set X = { x_i | x_i = <paramref name="start"/> + i * <paramref name="step"/>, i = 0 ... inf, x_i &lt;= <paramref name="end"/> }.
        /// </summary>
        /// <returns>The sequence.</returns>
        /// <param name="start">Start of the sequence.</param>
        /// <param name="end">End of the sequence.</param>
        /// <param name="step">Step size (default value is 1).</param>
        public static IEnumerable<int> ClosedSequence(int start, int end, int step = 1)
        {
            for (int i = start; i <= end; i += step)
            {
                yield return i;
            }
        }

        /// <summary>
        /// Takes a <paramref name="collection"/> of collections and returns a collection where each member is a member of a sub-collection.
        /// </summary>
        /// <param name="collection">A collection of collections.</param>
        public static IEnumerable<T> Flatten<T>(this IEnumerable<IEnumerable<T>> collection) =>
            collection.SelectMany(x => x);

        /// <summary>
        /// Takes a <paramref name="stringColection"/> and joins them into a single string.
        /// All elements are separated by <paramref name="joiner"/>.
        /// </summary>
        /// <param name="stringColection">A collection to contract into a single string.</param>
        /// <param name="joiner">A string that interpolates elements of <paramref name="stringColection"/>. Default value is a blank space.</param>
        public static string StringJoin(this IEnumerable<string> stringColection, string joiner = " ")
        {
            IEnumerable<string> enumerated = stringColection as string[] ?? stringColection as IList<string> ?? stringColection.ToArray();
            
            int count = enumerated.Count();
            var result = new StringBuilder();
            
            for (int i = 0; i < count - 1; i++)
            {
                result.Append(enumerated.ElementAt(i));
                result.Append(joiner);
            }

            result.Append(enumerated.ElementAt(count - 1));

            return result.ToString();
        }

        public static void RunSimpleVoidProcess(string command, string workingDir, string cliArgs, bool silent = false)
        {
            Process proc = new Process();
            
            proc.StartInfo.WorkingDirectory = workingDir;
            proc.StartInfo.FileName = command;
            
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            proc.StartInfo.UseShellExecute = false;

            proc.StartInfo.Arguments = cliArgs;
            
            proc.Start();
            proc.WaitForExit();
                
            if (proc.ExitCode != 0)
            {
                string errText =
                    $"[WARNING] Process {command} returned code {proc.ExitCode} on exit.{Environment.NewLine}" +
                    $"CLI args: {proc.StartInfo.Arguments}";
                
                Console.WriteLine(errText);
                Utils.DelayedWarnings.Enqueue(errText);
            }
            else if (!silent)
            {
                Console.WriteLine($"Process {command} successfully exited with code 0");
            }
        }
        
        public class ContinuousWriter
        {
            // fields
            private int _indentLevel;
            private readonly Action<string> _writingAction;

            // constructors
            public ContinuousWriter()
            {
                _writingAction = Console.WriteLine;
            }
            public ContinuousWriter(Action<string> writingAction)
            {
                _writingAction = writingAction;
            }
            
            // public methods
            public void WriteLine() => WriteLine("");
            
            public void WriteLine(string text)
            {
                string indent = new String(Enumerable.Repeat(' ', _indentLevel * 3).ToArray());
                text = text.Replace("\r", "");
                string[] lines = text.Split('\n');

                foreach (string line in lines)
                {
                    _writingAction(indent + line);
                }
            }

            public void Indent() => _indentLevel++;
            public void UnIndent() => _indentLevel--;
        }

        /// <summary>
        /// Used only for parsing time series
        /// </summary>
        public static class Specific
        {
            private const int trio_date_idx = 1;
            private const int trio_value_idx = 2;

            public static ValueTuple<DateTime, double> ParseWasserstand(string line)
            {
                // example:
                // 2044;1974.01.01 00:00:00;   5.858

                string[] trio = line.Split(';');
                // [0] = station id {ignore}
                // [1] = datetime
                // [2] = value

                DateTime date = DateTime.Parse(trio[trio_date_idx]);

                string strVal = trio[trio_value_idx].Replace(" ", "").Replace("\r", "");

                double val;
                if (!Double.TryParse(strVal, out val))
                {
                    val = Double.NaN;
                }

                return (date, val);
            }

            public static ValueTuple<DateTime, double> ParseMeteosuisse(string line)
            {
                // example:
                // ZHUST;200412070820;-;-

                string[] trio = line.Split(';');
                // [0] = station id {ignore}
                // [1] = datetime
                // [2] = value
                // [3] = whatever

                DateTime date;
                try
                {
                    date = DateTime.ParseExact(trio[trio_date_idx], "yyyyMMddHHmm", System.Globalization.CultureInfo.InvariantCulture);
                }
                catch
                {
                    date = DateTime.ParseExact(trio[trio_date_idx], "yyyyMMddHH", System.Globalization.CultureInfo.InvariantCulture);
                }

                string strVal = trio[1];

                if (!Double.TryParse(strVal, out double val))
                {
                    val = Double.NaN;
                }

                return (date, val);
            }
            
            public static ValueTuple<DateTime, double> ParseTemperature(string line)
            {
                int[] row = line.Split(' ').Select(Int32.Parse).ToArray();
                var date = new DateTime(row[1], row[2], row[3]);

                return (date, Convert.ToDouble(row[4]));
            }
        }
    }
}
