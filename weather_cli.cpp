//
// Created by Esteban on 11/5/2026.
//

// Combined console app: hash map vs trie over the same weather database
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include "Benchmark.h"
#include "DataLoader.h"
#include "HashMapIndex.h"
#include "TrieIndex.h"

using clk = steady_clock;
static vector<WeatherData> db;
static HashMapIndex hashIdx;
static TrieIndex trieIdx;
static CityIndex* cur = &hashIdx;
static int selected = -1;

static string ask(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) exit(0);
    return s;
}

static int askInt(const string& prompt, int fallback = -1) {
    try {
        return stoi(ask(prompt));
    } catch (...) {
        return fallback;
    }
}

static string label(int i) {
    return db[i].cityData + ", " + db[i].stateData;
}

static void printRow(const WeeklyRecord& r) {
    cout << "  " << setw(12) << left << r.dateFull << right
         << " avg " << setw(4) << r.avgTemp << "  max " << setw(4) << r.maxTemp << "  min " << setw(4) << r.minTemp
         << "  precip " << setw(5) << r.precipitation << "  wind " << r.windSpeed << " (dir " << r.windDirection << ")\n";
}

static void showStation(int i) {
    const WeatherData& d = db[i];
    cout << "\n=== " << label(i) << " (" << d.codeData << ") - " << d.locationData << " | " << d.weeklyRecords.size()
         << " weekly records ===\n";

    string mode = ask("[a] all weeks     [d] week containing a date     [Enter] first 5: ");
    if (mode == "a") {
        for (const auto& r : d.weeklyRecords) printRow(r);
    }
    else if (mode == "d") {
        int m = askInt("Month (1-12): "), day = askInt("Day (1-31): ");
        const WeeklyRecord* best = nullptr; // dateWeek = day of month the week starts on
        for (const auto& r : d.weeklyRecords)
            if (r.dateMonth == m && r.dateWeek <= day && (!best || r.dateWeek > best->dateWeek)) best = &r;

        if (best) printRow(*best);
        else cout << "  No record for that date!\n";
    }
    else {
        for (size_t k = 0; k < d.weeklyRecords.size(); k++) {
            if (k < 5) printRow(d.weeklyRecords[k]);
            else break;
        }
    }
}

static void reportLookup(const string& key) {
    const int N = 10000;
    int last = -1;
    auto t0 = clk::now();
    for (int i = 0; i < N; i++) last = cur->find(key);

    auto t1 = clk::now();
    cout << cur->name() << " exact lookup: " << duration<double, nano>(t1 - t0).count() / N << " ns (found index "
         << last << ")\n";
}

static void searchFlow() {
    string prefix = ask("City prefix (e.g. aus): ");
    auto t0 = clk::now();
    auto keys = cur->prefixSearch(prefix, 15);
    auto t1 = clk::now();
    cout << cur->name() << " prefix search: " << duration<double, micro>(t1 - t0).count() << " us, " << keys.size()
         << " match(es)\n";

    if (keys.empty()) return;
    for (size_t i = 0; i < keys.size(); i++) std::cout << "  " << i + 1 << ". " << label(cur->find(keys[i])) << "\n";

    int c = askInt("Pick a number (Enter to cancel): ");
    if (c < 1 || c > static_cast<int>(keys.size())) return;

    selected = cur->find(keys[c - 1]);
    reportLookup(keys[c - 1]);
    showStation(selected);
}

static void exactFlow() {
    string key = ask("Full key as city|state (e.g. gainesville|florida): ");
    int i = cur->find(key);
    if (i < 0) {
        cout << "Not found in the " << cur->name() << ".\n";
        return;
    }

    selected = i;
    reportLookup(key);
    showStation(i);
}

static void printBench(const vector<BenchResult>& rs) {
    cout << left << setw(10) << "Structure" << right << setw(9) << "Keys" << setw(12) << "Insert ms" << setw(14)
              << "Lookup ns" << setw(14) << "Prefix us" << setw(12) << "Delete ms" << setw(11) << "Mem KB" << "\n";

    for (const auto& r : rs) {
        cout << left << setw(10) << r.name << right << setw(9) << r.n << setw(12) << fixed << setprecision(2)
             << r.insertMs << setw(14) << setprecision(1) << r.findNs << setw(14) << setprecision(2)
             << r.prefixUs << setw(12) << r.deleteMs << setw(11) << setprecision(0) << r.memoryBytes / 1024.0
             << "   " << r.stats << "\n";
    }
}

static void benchFlow(bool real) {
    vector<string> keys;
    if (real)
        for (const auto& w : db) keys.push_back(stationKey(w));
    else {
        cout << "\nSizes:\n1) 1,000\n2) 10,000\n3) 100,000\n4) 500,000\n";
        const size_t sizes[] = {1000, 10000, 100000, 500000};
        int c = askInt("Choose: ");
        if (c < 1 || c > 4) return;

        keys = syntheticKeys(sizes[c - 1]);
    }

    HashMapIndex h;
    TrieIndex t;
    printBench({runBenchmark(h, keys), runBenchmark(t, keys)});
}

int main(int argc, char** argv) {
    string path;

    if (argc > 1) path = argv[1];
    else path = "../resources/weather.csv";
    if (!loadWeatherCSV(path, db)) return 1;

    size_t rows = 0;
    for (const auto& w : db) rows += w.weeklyRecords.size();
    for (size_t i = 0; i < db.size(); i++) {
        hashIdx.insert(stationKey(db[i]), static_cast<int>(i));
        trieIdx.insert(stationKey(db[i]), static_cast<int>(i));
    }
    cout << "Loaded " << rows << " weekly rows for " << db.size() << " stations.\n";

    while (true) {
        cout << "\n--- Active structure: " << cur->name() << " | " << cur->stats() << " | memory "
             << cur->memoryBytes() / 1024.0 << " KB ---\n"
             << "1) Switch structure\n2) Search by prefix (autocomplete)\n3) Exact lookup\n"
             << "4) Delete last-selected station from active structure\n5) Re-insert it\n"
             << "6) Benchmark (synthetic keys)\n7) Benchmark (real station keys)\n0) Quit\n";

        int c = askInt("> ");
        if (c == 0) break;
        else if (c == 1) {
            if (cur == &hashIdx) cur = static_cast<CityIndex*>(&trieIdx);
            else cur = static_cast<CityIndex*>(&hashIdx);
        }
        else if (c == 2) searchFlow();
        else if (c == 3) exactFlow();
        else if (c == 4 || c == 5) {
            if (selected < 0) {
                cout << "Select a station first (option 2 or 3).\n";
                continue;
            }

            string key = stationKey(db[selected]);
            if (c == 4) {
                if (cur->remove(key)) cout << "Removed " << label(selected) << " (" << cur->name() << " only)\n";
                else cout << "Not present: " << label(selected) << " (" << cur->name() << " only)\n";
            }
            else {
                if (cur->insert(key, selected)) cout << "Re-inserted " << label(selected) << "\n";
                else cout << "Already present: " << label(selected) << "\n";
            }
        }
        else if (c == 6) benchFlow(false);
        else if (c == 7) benchFlow(true);
    }
}