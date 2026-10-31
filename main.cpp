#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "Trie.h"
#include "WeatherData.h"

double toDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

int toInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return 0;
    }
}

std::string removeQuotes(const std::string& str) {
    std::string result = str;

    if (result.size() >= 2 && result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.size() - 2);
    }

    return result;
}

bool loadWeatherData(const std::string& filename, Trie& trie, std::vector<WeatherData>& db) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);

    std::unordered_map<std::string, int> cityIndices;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string precip, dateFull, month, weekOf, year;
        std::string city, code, location_one, location_two, location, state;
        std::string avgTemp, maxTemp, minTemp, windDir, windSpeed;

        std::getline(ss, precip, ',');
        std::getline(ss, dateFull, ',');
        std::getline(ss, month, ',');
        std::getline(ss, weekOf, ',');
        std::getline(ss, year, ',');
        std::getline(ss, city, ',');
        std::getline(ss, code, ',');
        std::getline(ss, location_one, ',');
        std::getline(ss, location_two, ',');
        location = location_one + "," + location_two;
        std::getline(ss, state, ',');
        std::getline(ss, avgTemp, ',');
        std::getline(ss, maxTemp, ',');
        std::getline(ss, minTemp, ',');
        std::getline(ss, windDir, ',');
        std::getline(ss, windSpeed, ',');

        precip = removeQuotes(precip);
        dateFull = removeQuotes(dateFull);
        month = removeQuotes(month);
        weekOf = removeQuotes(weekOf);
        year = removeQuotes(year);
        city = removeQuotes(city);
        code = removeQuotes(code);
        location = removeQuotes(location);
        state = removeQuotes(state);
        avgTemp = removeQuotes(avgTemp);
        maxTemp = removeQuotes(maxTemp);
        minTemp = removeQuotes(minTemp);
        windDir = removeQuotes(windDir);
        windSpeed = removeQuotes(windSpeed);

        if (city.empty()) {
            continue;
        }

        int dbIndex;
        if (cityIndices.find(city) == cityIndices.end()) {
            WeatherData newStation;
            newStation.cityData = city;
            newStation.codeData = code;
            newStation.locationData = location;
            newStation.stateData = state;

            db.push_back(newStation);
            dbIndex = static_cast<int>(db.size() - 1);
            cityIndices[city] = dbIndex;
            trie.insert(city, dbIndex);
        }
        else {
            dbIndex = cityIndices[city];
        }

        WeeklyRecord record;
        record.dateFull = dateFull;
        record.dateMonth = toInt(month);
        record.dateWeek = toInt(weekOf);
        record.dateYear = toInt(year);
        record.precipitation = toDouble(precip);
        record.avgTemp = toDouble(avgTemp);
        record.maxTemp = toDouble(maxTemp);
        record.minTemp = toDouble(minTemp);
        record.windDirection = toDouble(windDir);
        record.windSpeed = toDouble(windSpeed);

        db[dbIndex].weeklyRecords.push_back(record);
    }

    file.close();
    return true;
}

int main() {
    Trie cityTrie;
    std::vector<WeatherData> weatherDatabase;

    if (!loadWeatherData("../resources/weather.csv", cityTrie, weatherDatabase)) {
        return 1;
    }

    while (true) {
        std::cout << "\nEnter city prefix: ";
        std::string query;
        std::cin >> query;

        if (query == "exit") {
            break;
        }

        std::vector<std::string> suggestions = cityTrie.getMatches(query);

        if (suggestions.empty()) {
            std::cout << "No matching cities found.\n";
            continue;
        }

        std::cout << "\n[City Suggestions]:\n";
        for (size_t i = 0; i < suggestions.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << suggestions[i] << "\n";
        }

        std::cout << "Select a number: ";
        int choice;
        std::cin >> choice;

        if (choice > 0 && choice <= static_cast<int>(suggestions.size())) {
            std::string selectedCity = suggestions[choice - 1];

            TrieNode* node = cityTrie.searchPrefixNode(selectedCity);
            if (node && node->isEndOfWord) {
                const WeatherData& data = weatherDatabase[node->dataIndex];
                std::cout << "\n==================================================\n";
                std::cout << "Station: " << data.cityData << ", " << data.stateData
                          << " (" << data.codeData << ") - " << data.locationData << "\n";
                std::cout << "Total Records: " << data.weeklyRecords.size() << " weeks\n";
                std::cout << "--------------------------------------------------\n";

                int limit = std::min(static_cast<int>(data.weeklyRecords.size()), 5);
                for (int i = 0; i < limit; ++i) {
                    const auto& rec = data.weeklyRecords[i];
                    std::cout << "  [Date: " << rec.dateFull << " | Year: " << rec.dateYear
                              << " | Week: " << rec.dateWeek << "]\n"
                              << "    - Temp (Avg/Max/Min): " << rec.avgTemp << " / "
                              << rec.maxTemp << " / " << rec.minTemp << "\n"
                              << "    - Precipitation: " << rec.precipitation << " mm\n"
                              << "    - Wind Speed: " << rec.windSpeed << " (Dir: " << rec.windDirection << ")\n";
                }
                if (data.weeklyRecords.size() > 5) {
                    std::cout << "  ... and " << (data.weeklyRecords.size() - 5) << " more weekly records.\n";
                }
                std::cout << "==================================================\n";
            }
        }
    }

    return 0;
}