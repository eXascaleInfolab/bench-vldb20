using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STMVL
{
    class STMVL
    {
        int rowCount;
        int columnCount;
        double alpha;
        double gamma;
        int windowSize;

        double[,] missingMatrix;
        double[,] predictMatrix;
        double[,] temporaryMatrix;

        Dictionary<Tuple<int, int>, double> distanceDic;
        Dictionary<int, List<int>> stationDic;

        int temporal_threshold = 5;

        double default_missing = -1; // we set missing as -1, you can change it based on the actual data.

        List<DateTime> dtList;
        bool isVerify = false;

        public double ComputeSpatialWeight(double dis)
        {
            double ws = Math.Pow((1 / dis), alpha);
            return ws;
        }

        public double ComputeTemporalWeight(int timespan)
        {
            double wt = gamma * Math.Pow((1 - gamma), (timespan - 1));
            return wt;
        }

        public STMVL(string missingFile, string latlngFile, int _rowCount, int _colCount, double _alpha, double _gamma, int _windowsize, bool _isVerify = false)
        {
            rowCount = _rowCount;
            columnCount = _colCount;
            alpha = _alpha;
            gamma = _gamma;
            windowSize = _windowsize;
            isVerify = _isVerify;
            dtList = new List<DateTime>();
            missingMatrix = new double[rowCount, columnCount];
            predictMatrix = new double[rowCount, columnCount];
            temporaryMatrix = new double[rowCount, columnCount];
            string reader = "";

            int rowNum = 0;
            StreamReader srMiss = new StreamReader(missingFile);
            reader = srMiss.ReadLine(); // skip first line, description of sensors
            while ((reader = srMiss.ReadLine()) != null)
            {
                string[] items = reader.Split(',');

                if (isVerify)
                {
                    dtList.Add(Convert.ToDateTime(items[0]));
                }

                for (int j = 1; j < items.Length; j++)
                {
                    if (items[j] != "")
                    {
                        missingMatrix[rowNum, j - 1] = Convert.ToDouble(items[j]); // start from 0
                        predictMatrix[rowNum, j - 1] = Convert.ToDouble(items[j]);
                        temporaryMatrix[rowNum, j - 1] = Convert.ToDouble(items[j]);
                    }
                    else
                    {
                        missingMatrix[rowNum, j - 1] = default_missing;
                        predictMatrix[rowNum, j - 1] = default_missing;
                        temporaryMatrix[rowNum, j - 1] = default_missing;
                    }
                }

                rowNum++;
            }
            srMiss.Close();

            double[] latitude = new double[columnCount];
            double[] longitude = new double[columnCount];
            rowNum = 0;
            StreamReader srLatLng = new StreamReader(latlngFile);
            reader = srLatLng.ReadLine();
            while ((reader = srLatLng.ReadLine()) != null)
            {
                string[] items = reader.Split(',');
                latitude[rowNum] = Convert.ToDouble(items[1]);
                longitude[rowNum] = Convert.ToDouble(items[2]);
                rowNum++;
            }
            srLatLng.Close();

            distanceDic = new Dictionary<Tuple<int, int>, double>();
            stationDic = new Dictionary<int, List<int>>();

            for (int i = 0; i < columnCount; i++)
            {
                Dictionary<int, double> distance_dic_temp = new Dictionary<int, double>();

                for (int j = 0; j < columnCount; j++)
                {
                    if (i != j)
                    {
                        double dis = BaseFunction.GeoDistance(latitude[i], longitude[i], latitude[j], longitude[j]);
                        distanceDic.Add(new Tuple<int, int>(i, j), dis);
                        distance_dic_temp.Add(j, dis);
                    }
                }

                distance_dic_temp = distance_dic_temp.OrderBy(o => o.Value).ToDictionary(o => o.Key, p => p.Value);

                stationDic.Add(i, new List<int>());

                foreach (var p in distance_dic_temp)
                {
                    stationDic[i].Add(p.Key);
                }
            }
        }

        public void Run(string predictFile, bool isBlockMissing)
        {
            if (isBlockMissing)
            {
                InitializeMissing();
            }

            string equationFile = "stmvl_equation.txt";

            GenerateTrainingCase();

            SquareError.FourView(equationFile, columnCount);

            doSTMVL(predictFile, equationFile);
        }

        public void doSTMVL(string predictFile, string equaFile)
        {
            Trace.TraceInformation("Do STMVL... " + DateTime.Now);

            string reader = "";
            double[,] equation = new double[columnCount, 5]; // 5 means 4 four views and residual 

            StreamReader srEquation = new StreamReader(equaFile);
            int count = 0;
            while ((reader = srEquation.ReadLine()) != null)
            {
                string[] items = reader.Split(',');
                equation[count, 0] = Convert.ToDouble(items[0]);
                equation[count, 1] = Convert.ToDouble(items[1]);
                equation[count, 2] = Convert.ToDouble(items[2]);
                equation[count, 3] = Convert.ToDouble(items[3]);
                equation[count, 4] = Convert.ToDouble(items[4]);

                count++;
            }
            srEquation.Close();

            for (int i = 0; i < rowCount; i++)
            {
                for (int j = 0; j < columnCount; j++)
                {
                    if (missingMatrix[i, j] == default_missing)
                    {
                        MVL(i, j, equation);
                    }
                }
            }

            StreamWriter sw = new StreamWriter(predictFile);
            for (int i = 0; i < rowCount; i++)
            {
                for (int j = 0; j < columnCount - 1; j++)
                {
                    sw.Write(predictMatrix[i, j] + " ");
                }
                sw.WriteLine(predictMatrix[i, columnCount - 1]);
            }
            sw.Close();
        }

        public void InitializeMissing()
        {
            Trace.TraceInformation("Initialize Missing... " + DateTime.Now);

            for (int i = 0; i < rowCount; i++)
            {
                for (int j = 0; j < columnCount; j++)
                {
                    if (missingMatrix[i, j] == default_missing)
                    {
                        GlobalViewCombine(i, j);
                    }
                }
            }
        }

        public void GlobalViewCombine(int i, int j)
        {
            double result_IDW = IDW(i, j, missingMatrix);
            double result_SES = SES(i, j, missingMatrix);

            if (result_IDW != default_missing && result_SES != default_missing)
            {
                temporaryMatrix[i, j] = (result_IDW + result_SES) / 2; // simple combine equally.
            }
            else if (result_SES != default_missing)
            {
                temporaryMatrix[i, j] = result_SES;
            }
            else if (result_IDW != default_missing)
            {
                temporaryMatrix[i, j] = result_IDW;
            }
        }

        public void MVL(int i, int j, double[,] equation)
        {
            double result_UCF = UCF(i, j, temporaryMatrix);
            double result_ICF = ICF(i, j, temporaryMatrix);
            double result_IDW = IDW(i, j, temporaryMatrix);
            double result_SES = SES(i, j, temporaryMatrix);

            if (result_UCF != default_missing && result_ICF != default_missing && result_IDW != default_missing && result_SES != default_missing)
            {
                double result = equation[j, 0] * result_UCF + equation[j, 1] * result_IDW + equation[j, 2] * result_ICF + equation[j, 3] * result_SES + equation[j, 4];
                predictMatrix[i, j] = result;
            }
            else
            {
                predictMatrix[i, j] = temporaryMatrix[i, j];
            }
        }

        public double UCF(int ti, int tj, double[,] dataMatrix)
        {
            Dictionary<double, double> candiDic = new Dictionary<double, double>();

            foreach (var jj in stationDic[tj])
            {
                if (dataMatrix[ti, jj] != default_missing)
                {
                    double sim = calUserEuclideanSim(tj, jj, ti, dataMatrix);

                    if (sim != 0)
                    {
                        if (candiDic.ContainsKey(dataMatrix[ti, jj]))
                        {
                            candiDic[dataMatrix[ti, jj]] += sim;
                        }
                        else
                        {
                            candiDic.Add(dataMatrix[ti, jj], sim);
                        }
                    }
                }
            }

            double spatialPredict = default_missing;
            if (candiDic.Count != 0)
            {
                double weightSum = candiDic.Values.Sum();
                double tempSpatialPredict = 0;
                foreach (var p in candiDic)
                {
                    tempSpatialPredict += p.Key * p.Value / weightSum;
                }
                spatialPredict = tempSpatialPredict;
            }

            return spatialPredict;
        }

        public double calUserEuclideanSim(int tj, int jj, int ti, double[,] dataMatrix)
        {
            double similarity = 0;
            double offset = 0;
            int NT = 0;

            int upRow = ti - windowSize / 2;
            if (upRow < 0)
            {
                upRow = 0;
            }

            int downRow = ti + windowSize / 2;
            if (downRow >= rowCount)
            {
                downRow = rowCount - 1;
            }

            for (int i = upRow; i < downRow; i++)
            {
                if (dataMatrix[i, tj] != default_missing && dataMatrix[i, jj] != default_missing)
                {
                    offset += Math.Pow((dataMatrix[i, tj] - dataMatrix[i, jj]), 2);
                    NT++;
                }
            }

            if (NT > 0 && offset > 0)
            {
                double avgDis = Math.Sqrt(offset) / NT;
                similarity = 1 / (avgDis);
            }

            return similarity;
        }

        public double ICF(int ti, int tj, double[,] dataMatrix)
        {
            Dictionary<double, double> candiDic = new Dictionary<double, double>();

            int upRow = ti - windowSize / 2;
            if (upRow < 0)
            {
                upRow = 0;
            }

            int downRow = ti + windowSize / 2;
            if (downRow >= rowCount)
            {
                downRow = rowCount - 1;
            }

            for (int ii = upRow; ii < downRow; ii++)
            {
                if (ii == ti)
                    continue;

                if (dataMatrix[ii, tj] != default_missing)
                {
                    double sim = calItemEuclideanSim(ti, ii, tj, dataMatrix);
                    if (sim != 0)
                    {
                        if (candiDic.ContainsKey(dataMatrix[ii, tj]))
                        {
                            candiDic[dataMatrix[ii, tj]] += sim;
                        }
                        else
                        {
                            candiDic.Add(dataMatrix[ii, tj], sim);
                        }
                    }
                }
            }

            double temporalPredict = default_missing;
            if (candiDic.Count != 0)
            {
                double weightSum = candiDic.Values.Sum();
                double tempTemporalPredict = 0;
                foreach (var p in candiDic)
                {
                    tempTemporalPredict += p.Key * p.Value / weightSum;
                }
                temporalPredict = tempTemporalPredict;
            }

            return temporalPredict;
        }

        public double calItemEuclideanSim(int ti, int ii, int tj, double[,] dataMatrix)
        {
            double similarity = 0;
            double offset = 0;
            int NS = 0;

            foreach (var jj in stationDic[tj])
            {
                if (dataMatrix[ti, jj] != default_missing && dataMatrix[ii, jj] != default_missing)
                {
                    offset += Math.Pow((dataMatrix[ti, jj] - dataMatrix[ii, jj]), 2);
                    NS++;
                }
            }

            if (NS > 0 && offset > 0)
            {
                double avgDis = Math.Sqrt(offset) / NS;
                similarity = 1 / (avgDis);
            }

            return similarity;
        }

        public double SES(int ti, int tj, double[,] dataMatrix)
        {
            Dictionary<double, double> candiDic = new Dictionary<double, double>();

            for (int i = 1; i <= temporal_threshold; i++)
            {
                int ii = ti - i;
                if (ii >= 0 && dataMatrix[ii, tj] != default_missing)
                {
                    double weight = ComputeTemporalWeight(Math.Abs(i));
                    double value = dataMatrix[ii, tj];

                    if (candiDic.ContainsKey(value))
                    {
                        candiDic[value] += weight;
                    }
                    else
                    {
                        candiDic.Add(value, weight);
                    }
                }

                ii = ti + i;
                if (ii < rowCount && dataMatrix[ii, tj] != default_missing)
                {
                    double weight = ComputeTemporalWeight(Math.Abs(i));
                    double value = dataMatrix[ii, tj];

                    if (candiDic.ContainsKey(value))
                    {
                        candiDic[value] += weight;
                    }
                    else
                    {
                        candiDic.Add(value, weight);
                    }
                }
            }

            candiDic = candiDic.OrderByDescending(o => o.Value).ToDictionary(o => o.Key, p => p.Value);

            double temporalPredict = default_missing;
            if (candiDic.Count > 0)
            {
                double weightSum = candiDic.Values.Sum();
                double tempTemporalPredict = 0;
                foreach (var q in candiDic)
                {
                    tempTemporalPredict += q.Key * q.Value / weightSum;
                }
                temporalPredict = tempTemporalPredict;
            }
            return temporalPredict;
        }

        public double IDW(int ti, int tj, double[,] dataMatrix)
        {
            Dictionary<double, double> candiDic = new Dictionary<double, double>();

            foreach (var jj in stationDic[tj])
            {
                double dis = distanceDic[new Tuple<int, int>(tj, jj)];
                if (dataMatrix[ti, jj] != default_missing)
                {
                    double weight = ComputeSpatialWeight(dis);
                    double value = dataMatrix[ti, jj];

                    if (candiDic.ContainsKey(value))
                    {
                        candiDic[value] += weight;
                    }
                    else
                    {
                        candiDic.Add(value, weight);
                    }
                }
            }

            candiDic = (from item in candiDic orderby item.Value descending select item).ToDictionary(pair => pair.Key, pair => pair.Value);

            double spatialPredict = default_missing;
            if (candiDic.Count != 0)
            {
                double weightSum = candiDic.Values.Sum();
                double tempSpatialPredict = 0;
                foreach (var q in candiDic)
                {
                    tempSpatialPredict += q.Key * q.Value / weightSum;
                }
                spatialPredict = tempSpatialPredict;
            }

            return spatialPredict;
        }

        public void GenerateTrainingCase()
        {
            Trace.TraceInformation("Generate Training Case... " + DateTime.Now);

            Directory.CreateDirectory(@"TraingFolder");

            for (int j = 0; j < columnCount; j++)
            {
                int caseCount = 0;

                for (int i = 0; i < rowCount; i++)
                {
                    if (isVerify)
                    {
                        int mm = dtList[i].Month;
                        if (mm == 3 || mm == 6 || mm == 9 || mm == 12)
                        {
                            continue;
                        }
                    }

                    if (missingMatrix[i, j] != default_missing)
                    {
                        if (checkContextData(i, j) == 1)
                        {
                            caseCount++;
                        }
                    }
                }

                StreamWriter swTrain = new StreamWriter(@"TraingFolder/" + @"train_" + j + ".txt");
                swTrain.WriteLine(caseCount);

                for (int i = 0; i < rowCount; i++)
                {
                    if (isVerify)
                    {
                        int mm = dtList[i].Month;
                        if (mm == 3 || mm == 6 || mm == 9 || mm == 12)
                        {
                            continue;
                        }
                    }

                    if (missingMatrix[i, j] != default_missing)
                    {
                        if (checkContextData(i, j) == 1)
                        {
                            outputCase(i, j, swTrain);
                        }
                    }
                }

                swTrain.Close();
            }
        }

        public int checkContextData(int ti, int tj)
        {
            int count = 0;
            for (int j = 0; j < columnCount; j++)
            {
                if (missingMatrix[ti, j] == default_missing)
                    count++;
            }
            if (count > (columnCount / 2))
                return 0;

            int si = ti - windowSize / 2;
            int ei = ti + windowSize / 2;
            if (si < 0)
                si = 0;
            if (ei >= rowCount)
                ei = rowCount - 1;

            count = 0;

            for (int i = si; i <= ei; i++)
            {
                if (missingMatrix[i, tj] == default_missing)
                    count++;
            }

            if (count > ((ei - si + 1) / 2))
                return 0;

            return 1;
        }

        public void outputCase(int i, int j, StreamWriter swTrain)
        {
            double result_UCF = UCF(i, j, temporaryMatrix);
            double result_ICF = ICF(i, j, temporaryMatrix);
            double result_IDW = IDW(i, j, temporaryMatrix);
            double result_SES = SES(i, j, temporaryMatrix);

            if (result_UCF != default_missing && result_ICF != default_missing && result_IDW != default_missing && result_SES != default_missing)
            {
                swTrain.WriteLine(missingMatrix[i, j] + "," + result_UCF + "," + result_IDW + "," + result_ICF + "," + result_SES);
            }
            else
            {
                //Console.WriteLine();
                // some case is recorded as training case, but not all views have results. //skip it, it do not have any bad influence.
            }
        }

        public void Evaluate(string groundFile)
        {
            string reader = "";
            List<string> allMissingList = new List<string>();
            double[,] groundMatrix = new double[rowCount, columnCount];

            StreamReader srGround = new StreamReader(groundFile);
            int rowNum = 0;
            srGround.ReadLine();
            while ((reader = srGround.ReadLine()) != null)
            {
                string[] items = reader.Split(',');
                for (int j = 1; j <= columnCount; j++)
                {
                    if (items[j] != "")
                    {
                        groundMatrix[rowNum, j - 1] = Convert.ToDouble(items[j]);
                    }
                    else
                    {
                        groundMatrix[rowNum, j - 1] = default_missing;
                    }
                }
                rowNum++;
            }
            srGround.Close();

            double all_MAE = 0;
            double all_MRE = 0;
            List<double> allList = new List<double>();

            for (int i = 0; i < rowCount; i++)
            {
                int mm = dtList[i].Month;
                if (mm == 3 || mm == 6 || mm == 9 || mm == 12)
                {
                    for (int j = 0; j < columnCount; j++)
                    {
                        if (groundMatrix[i, j] != default_missing && missingMatrix[i, j] == default_missing && predictMatrix[i, j] != default_missing)
                        {
                            double delta = Math.Abs(groundMatrix[i, j] - predictMatrix[i, j]);
                            all_MAE += delta;
                            allList.Add(groundMatrix[i, j]);
                        }
                    }
                }
            }

            all_MRE = all_MAE / allList.Sum();
            all_MAE = all_MAE / allList.Count();

            Trace.TraceInformation(all_MAE.ToString("F2") + "\t" + all_MRE.ToString("F3"));
        }
    }
}

