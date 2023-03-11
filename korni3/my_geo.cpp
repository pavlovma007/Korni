#include "my_geo.h"

#include <vector>

// OK
vector<int> BITS{ 16, 8, 4, 2, 1 };
const char BASE32[] = "0123456789bcdefghjkmnpqrstuvwxyz";
string encodeGeoHash(double latitude, double longitude)
{
    bool is_even = true;
    int i = 0;
    vector<double> lat(2); //  = [];
    vector<double> lon(2); //  = [];
    int bit = 0;
    unsigned int ch = 0;
    int precision = 12;
    string geohash = "";

    lat[0] = -90.0;
    lat[1] = 90.0;
    lon[0] = -180.0;
    lon[1] = 180.0;

    while (geohash.length() < precision)
    {
        if (is_even)
        {
            double mid = (lon[0] + lon[1]) / 2;
            if (longitude > mid)
            {
                ch |= BITS[bit];
                lon[0] = mid;
            }
            else
                lon[1] = mid;
        }
        else
        {
            double mid = (lat[0] + lat[1]) / 2;
            if (latitude > mid)
            {
                ch |= BITS[bit];
                lat[0] = mid;
            }
            else
                lat[1] = mid;
        }

        is_even = !is_even;
        if (bit < 4)
            bit++;
        else
        {
            geohash += BASE32[ch];
            bit = 0;
            ch = 0;
        }
    }
    return geohash;
}

// OK
void decodeGeoHash(const string& geohash, double& outLat, double& outLongitude)
{
    auto refine_interval = [](vector<double>& interval, const int cd, const int mask) {
        if (cd & mask)
            interval[0] = (interval[0] + interval[1]) / 2;
        else
            interval[1] = (interval[0] + interval[1]) / 2;
    };
    bool is_even = 1;
    vector<double> lat(3);
    vector<double> lon(3);
    lat[0] = -90.0;
    lat[1] = 90.0;
    lon[0] = -180.0;
    lon[1] = 180.0;
    double lat_err = 90.0;
    double lon_err = 180.0;

    for (int i = 0; i < geohash.length(); i++)
    {
        char c = geohash[i];
        int cd = string(BASE32).find(c);
        for (int j = 0; j < 5; j++)
        {
            int mask = BITS[j];
            if (is_even)
            {
                lon_err /= 2;
                refine_interval(lon, cd, mask);
            }
            else
            {
                lat_err /= 2;
                refine_interval(lat, cd, mask);
            }
            is_even = !is_even;
        }
    }
    lat[2] = (lat[0] + lat[1]) / 2;
    lon[2] = (lon[0] + lon[1]) / 2;

    outLat = lat[2];
    outLongitude = lon[2];
}

// https://stackoverflow.com/a/63767823/2355077
//#include <algorithm>
//#include <iostream>
#include <math.h>
#define PI 3.14159265358979323846
#define RADIO_TERRESTRE 6372797.56085
#define GRADOS_RADIANES PI / 180
//using namespace std;
float calcGPSDistance(float latitud1, float longitud1, float latitud2, float longitud2)
{
    double haversine;
    double temp;
    double distancia_puntos;

    latitud1 = latitud1 * GRADOS_RADIANES;
    longitud1 = longitud1 * GRADOS_RADIANES;
    latitud2 = latitud2 * GRADOS_RADIANES;
    longitud2 = longitud2 * GRADOS_RADIANES;

    haversine = (pow(sin((1.0 / 2) * (latitud2 - latitud1)), 2)) +
                ((cos(latitud1)) * (cos(latitud2)) *
                 (pow(sin((1.0 / 2) * (longitud2 - longitud1)), 2)));
    temp = 2 * asin(min(1.0, sqrt(haversine)));
    distancia_puntos = RADIO_TERRESTRE * temp;

    return distancia_puntos;
}
