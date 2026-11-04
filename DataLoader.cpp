//
// Created by Esteban on 11/3/2026.
//

#include "DataLoader.h"
#include "CityIndex.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

static vector<string> splitCSVLine(const string& line) {
    vector<string> out;
    string cur;
    bool inQ = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQ && i + 1 < line.size() && line[i + 1] == '"') {
                cur += '"';
                i++;
            } else inQ = !inQ;
        }
        else if (c == ',' && !inQ) {
            out.push_back(cur);
            cur.clear();
        }
        else if (c != '\r') cur += c;
    }

    out.push_back(cur);
    return out;
}

static double toD(const string& s) {
    try {
        return stod(s);
    }
    catch (...) {
        return 0.0;
    }
}

string stationKey(const string& city, const string& state) {
    return normalizeKey(city + "|" + state);
}

// Columns: precip, date, month, week of, year, city, code, location, state, avg, max, min, wind dir, wind speed
bool loadWeatherCSV(const string& filename, vector<WeatherData>& db) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: could not open " << filename << "\n";
        return false;
    }

    string line;
    getline(file, line);

    unordered_map<string, int> seen;
    while (getline(file, line)) {
        auto c = splitCSVLine(line);
        if (c.size() < 14 || c[5].empty()) continue;

        string key = stationKey(c[5], c[8]);
        int i;
        auto it = seen.find(key);

        if (it == seen.end()) {
            WeatherData w;
            w.cityData = c[5];
            w.codeData = c[6];
            w.locationData = c[7];
            w.stateData = c[8];
            db.push_back(w); i = static_cast<int>(db.size()) - 1;
            seen[key] = i;
        } else i = it->second;

        WeeklyRecord r;
        r.precipitation = toD(c[0]);
        r.dateFull = c[1];
        r.dateMonth = static_cast<int>(toD(c[2]));
        r.dateWeek = static_cast<int>(toD(c[3]));
        r.dateYear = static_cast<int>(toD(c[4]));
        r.avgTemp = toD(c[9]);
        r.maxTemp = toD(c[10]);
        r.minTemp = toD(c[11]);
        r.windDirection = toD(c[12]);
        r.windSpeed = toD(c[13]);
        db[i].weeklyRecords.push_back(r);
    }
    return true;
}