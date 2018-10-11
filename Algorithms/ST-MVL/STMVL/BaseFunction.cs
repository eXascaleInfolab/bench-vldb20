using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STMVL
{
    class BaseFunction
    {
        private const double EARTH_RADIUS = 6378137.0; //earth radius

        public static double RadToDeg(double d)
        {
            return d * Math.PI / 180.0;
        }

        public static double GeoDistance(double lat1, double lng1, double lat2, double lng2)
        {
            double radLat1 = RadToDeg(lat1);
            double radLat2 = RadToDeg(lat2);
            double a = radLat1 - radLat2;
            double b = RadToDeg(lng1) - RadToDeg(lng2);

            double s = 2 * Math.Asin(Math.Sqrt(Math.Pow(Math.Sin(a / 2), 2) + Math.Cos(radLat1) * Math.Cos(radLat2) * Math.Pow(Math.Sin(b / 2), 2)));
            s = s * EARTH_RADIUS;
            return s;
        }
    }
}
