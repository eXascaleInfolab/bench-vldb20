using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using TestingFramework.Testing;

namespace TestingFramework.Algorithms
{
    public static class AlgoPack
    {
        // points to the location of the Algorithms/ folder, all the subfolders are handled automatically
        public static string GlobalAlgorithmsLocation = null;
        
        // static
        public static readonly Algorithm Cd = new CentroidDecompositionAlgorithm(); // a slower version of Recovery CD
        public static readonly Algorithm InCd = new IncrementalCentroidDecompositionAlgorithm();
        public static readonly Algorithm Tkcm = new TkcmAlgorithm();
        public static readonly Algorithm Trmf = new TrmfAlgorithm();
        public static readonly Algorithm Spirit = new SpiritAlgorithm();
        public static readonly Algorithm Stmvl = new StmvlAlgorithm();
        public static readonly Algorithm Nnmf = new NnmfAlgorithm();
        public static readonly Algorithm Grouse = new GrouseAlgorithm();
        public static readonly Algorithm Svt = new SVTAlgorithm();
        public static readonly Algorithm SoftImpute = new SoftImputeAlgorithm();
        public static readonly Algorithm ROSL = new ROSLAlgorithm();
        public static readonly Algorithm DynaMMo = new DynaMMoAlgorithm();
        public static readonly Algorithm SvdI = new SVDImputeAlgorithm();
        public static readonly Algorithm MeanImp = new MeanImputeAlgorithm();

        //example:
        //    public static readonly Algorithm Example = new ExampleAlgorithm();
        
        public static Algorithm[] ListAlgorithms = { Stmvl, InCd, Tkcm, Spirit, Trmf, Nnmf, Grouse, Svt, SoftImpute, ROSL, DynaMMo, SvdI, MeanImp };
        public static Algorithm[] ListAlgorithmsMulticolumn = { Stmvl, InCd, Trmf, Nnmf, Grouse, Svt, SoftImpute, ROSL, DynaMMo, SvdI };
        public static Algorithm[] ListAlgorithmsStreaming = { InCd, Tkcm, Spirit };

        public const int TypicalTruncation = 3;

        public static void PurgeAllIntermediateFiles()
        {
            ListAlgorithms.ForEach(x => x.DataCleanUp());
        }

        public static void CleanUncollectedResults()
        {
            Directory.EnumerateDirectories(DataWorks.FolderResults + "plots/")
                .Where(x => !x.EndsWith("/raw"))
                .Where(Directory.Exists)
                .ForEach(x => Directory.Delete(x, true));

            Directory.EnumerateFiles(DataWorks.FolderResults + "missingMat/")
                .Where(File.Exists)
                .Where(x => !x.EndsWith(".gitkeep"))
                .ForEach(File.Delete);

            Directory.EnumerateFiles(DataWorks.FolderResults + "plotfiles/out/")
                .Where(File.Exists)
                .Where(x => !x.EndsWith(".gitkeep"))
                .ForEach(File.Delete);
        }

        public static void EnsureFolderStructure(List<string> scenarios)
        {
            string root = DataWorks.FolderPlotsRemote;

            foreach (var es in EnumMethods.AllExperimentScenarios().Where(x => scenarios.Contains(x.ToLongString())))
            {
                string esDir = $"{root}{es.ToLongString()}/";
                if (!Directory.Exists(esDir))
                {
                    Directory.CreateDirectory(esDir);
                }
            }
        }
    }
    
    /*///////////////////////////////////////////////////////////*/
    /*                         P A T H S                         */
    /*///////////////////////////////////////////////////////////*/
    
    public partial class CentroidDecompositionAlgorithm
    {
        public override string AlgCode => "cd";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
    }
    
    public partial class IncrementalCentroidDecompositionAlgorithm
    {
        public override string AlgCode => "cdrec";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
    }

    public partial class NnmfAlgorithm
    {
        public override string AlgCode => "tenmf";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public override bool IsPlottable => false;
        public int Truncation = AlgoPack.TypicalTruncation;
    }

    public partial class SpiritAlgorithm
    {
        public override string AlgCode => "spirit";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public override bool IsPlottable => false;
        public int Truncation = 4;
    }

    public partial class StmvlAlgorithm
    {
        public override string AlgCode => "stmvl";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public double Alpha = 2.0;
    }

    public partial class TkcmAlgorithm
    {
        public override string AlgCode => "tkcm";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public override bool IsPlottable => false;
        public int ParamL = 4;
    }

    public partial class TrmfAlgorithm
    {
        public override string AlgCode => "trmf";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}trmf/trmf/";
        protected override string SubFolderDataIn => "../data/in/";
        protected override string SubFolderDataOut => "../data/out/";
        public int Truncation = AlgoPack.TypicalTruncation;
    }
    
    public partial class GrouseAlgorithm
    {
        public override string AlgCode => "grouse";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public override bool IsPlottable => false;
        public int Truncation = AlgoPack.TypicalTruncation;
    }
    
    public partial class SVTAlgorithm
    {
        public override string AlgCode => "svt";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public override bool IsPlottable => false;
        public double TauScale = 0.2;
    }
    
    public partial class SoftImputeAlgorithm
    {
        public override string AlgCode => "softimp";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public int Truncation = AlgoPack.TypicalTruncation;
    }
    
    public partial class ROSLAlgorithm
    {
        public override string AlgCode => "rosl";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public override bool IsPlottable => false;
        public int Truncation = AlgoPack.TypicalTruncation;
    }
    
    public partial class DynaMMoAlgorithm
    {
        public override string AlgCode => "dynammo";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public int Truncation = AlgoPack.TypicalTruncation;
    }
    
    public partial class SVDImputeAlgorithm
    {
        public override string AlgCode => "svdimp";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}AlgoCollection/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
        public int Truncation = AlgoPack.TypicalTruncation;
    }
    
    public partial class MeanImputeAlgorithm
    {
        public override string AlgCode => "meanimp";
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}NewAlgorithms/cpp/_data/";
        protected override string SubFolderDataIn => "in/";
        protected override string SubFolderDataOut => "out/";
    }

    /*///////////////////////////////////////////////////////////*/
    /*                        E X A M P L E                      */
    /*///////////////////////////////////////////////////////////*/

    public partial class ExampleAlgorithm
    {
        // should have trailing slash, should have access level to write to
        // {this.EnvPath + this.SubFolderIn} and {this.EnvPath + this.SubFolderOut} should both be:
        // * valid locations as a Directory
        // * {{} + validFileName} should always be an eligible file destination to be copied or written to

        public override string AlgCode => "example"; // unique code
        protected override string _EnvPath => $"{AlgoPack.GlobalAlgorithmsLocation}/TBA/"; // usually expected to be a working directory of an executable
        protected override string SubFolderDataIn => "todo/in/";
        protected override string SubFolderDataOut => "todo/out/";
        
        // optional override of a getter
        public override bool IsPlottable => false;
    }
    
    public partial class ExampleAlgorithm : Algorithm
    {
        private static bool _init = false;
        public ExampleAlgorithm() : base(ref _init)
        { }
        
        // optional override function
        public override string[] EnumerateInputFiles(string dataCode, int tcase)
        {
            throw new NotImplementedException();
        }
        
        // optional override function
        public override string[] EnumerateOutputFiles(int tcase)
        {
            throw new NotImplementedException();
        }

        protected override void PrecisionExperiment(ExperimentType et, ExperimentScenario es,
            DataDescription data, int tcase)
        {
            throw new NotImplementedException();
        }

        protected override void RuntimeExperiment(ExperimentType et, ExperimentScenario es, DataDescription data,
            int tcase)
        {
            throw new NotImplementedException();
        }

        public override void GenerateData(string sourceFile, string code, int tcase, (int, int, int)[] missingBlocks,
            (int, int) rowRange, (int, int) columnRange)
        {
            throw new NotImplementedException();
        }
        
        // last element of the tuple should be
        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms()
        {
            return new[] { new SubAlgorithm($"{AlgCode}", String.Empty, "") };
        }

        public override IEnumerable<SubAlgorithm> EnumerateSubAlgorithms(int tcase)
        {
            return new[] { new SubAlgorithm($"{AlgCode}", $"{AlgCode}{tcase}", "") };
        }
    }
}