//
// Created by Esteban on 11/3/2026.
//

#ifndef DATALOADER_H
#define DATALOADER_H

#include "WeatherData.h"
using namespace std;

// Key used by BOTH data structures normalized to lowercase "city|state"
string stationKey(const string& city, const string& state);
inline string stationKey(const WeatherData& w) {
    return stationKey(w.cityData, w.stateData);
}

// Reads the weather CSV into database (db); one WeatherData per station, weekly rows appended in file order
bool loadWeatherCSV(const string& filename, vector<WeatherData>& db);

#endif //DATALOADER_H
