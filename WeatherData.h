//
// Created by Gautam on 10/28/2026.
//

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <string>
#include <vector>

class WeeklyRecord {
public:
    // Dates
    std::string dateFull;
    int dateMonth;
    int dateWeek;
    int dateYear;

    // Temperatures
    double avgTemp;
    double maxTemp;
    double minTemp;

    // Precipitation & Wind
    double precipitation;
    double windDirection;
    double windSpeed;
};

class WeatherData {
public:
    std::string cityData;
    std::string codeData;
    std::string locationData;
    std::string stateData;
    std::vector<WeeklyRecord> weeklyRecords;
};

#endif // WEATHERDATA_H