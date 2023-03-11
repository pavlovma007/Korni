#include <string>

using namespace std;

string encodeGeoHash(double latitude, double longitude);

void decodeGeoHash(const string& geohash, double& outLat, double& outLongitude);

// in meters
float calcGPSDistance(float latitud1, float longitud1, float latitud2, float longitud2);
