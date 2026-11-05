//
// Created by Esteban on 11/4/2026.
//

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <set>
#include "../Benchmark.h"
#include "../DataLoader.h"
#include "../HashMapIndex.h"
#include "../TrieIndex.h"

// Testing data structure correctnesss
static set<string> brute(const vector<string>& keys, const string& p) {
    set<string> s;
    for (auto& k : keys)
        if (k.compare(0, p.size(), p) == 0) s.insert(k);
    return s;
}

static void check(CityIndex& idx, const vector<string>& keys) {
    idx.clear();
    for (size_t i = 0; i < keys.size(); i++)
        assert(idx.insert(keys[i], static_cast<int>(i)));

    assert(idx.size() == keys.size());
    assert(!idx.insert(keys[0], 0)); // Rejected dupe

    for (size_t i = 0; i < keys.size(); ++i)
        assert(idx.find(keys[i]) == static_cast<int>(i));

    assert(idx.find("zzzz|zzzz") == -1);
    assert(idx.find("ST. LOUIS|MISSOURI") == idx.find("st. louis|missouri")); // Case IN-sensitive

    for (string p : {"a", "ab", "abc", "q", "zz", "st.", ""}) {
        auto r = idx.prefixSearch(p);
        set<string> got;
        got.insert(r.begin(), r.end());
        assert(got == brute(keys, p) && got.size() == r.size());
    }

    assert(idx.prefixSearch("a", 3).size() <= 3);
    vector<string> kept;

    for (size_t i = 0; i < keys.size(); i++) {
        if (i % 2) assert(idx.remove(keys[i]));
        else kept.push_back(keys[i]);
    }

    assert(!idx.remove(keys[1]) && idx.size() == kept.size());
    for (size_t i = 0; i < keys.size(); i++)
        assert((idx.find(keys[i]) >= 0) == (i % 2 == 0));

    auto r = idx.prefixSearch("a");
    assert(set<string>(r.begin(), r.end()) == brute(kept, "a"));
    cout << idx.name() << ": correctness checks passed (" << idx.stats() << ")\n";
}

int main(int argc, char** argv) {
    auto keys = syntheticKeys(5000);
    keys.push_back("st. louis|missouri");
    keys.push_back("spring hill|florida");
    keys.push_back("springfield|illinois");
    HashMapIndex h;
    TrieIndex t;
    check(h, keys);
    check(t, keys);

    if (argc > 1) {
        vector<WeatherData> db;
        assert(loadWeatherCSV(argv[1], db));
        size_t rows = 0;
        for (auto& w : db)
            rows += w.weeklyRecords.size();

        cout << "CSV: " << rows << " rows, " << db.size() << " stations\n";
        h.clear(); t.clear();

        for (size_t i = 0; i < db.size(); i++) {
            assert(h.insert(stationKey(db[i]), static_cast<int>(i)));
            assert(t.insert(stationKey(db[i]), static_cast<int>(i)));
        }

        for (size_t i = 0; i < db.size(); i++)
            assert(h.find(stationKey(db[i])) == static_cast<int>(i) && t.find(stationKey(db[i])) == static_cast<int>(i));

        cout << "Both structures returned identical station indexes for every station! :]\n";
    }
    for (size_t n : {1000, 100000}) {
        auto ks = syntheticKeys(n);
        vector<CityIndex*> v = {&h, &t};
        for (auto& i : v) {
            auto r = runBenchmark(*i, ks);
            cout << r.name << " n=" << r.n << " insert=" << r.insertMs << "ms lookup=" << r.findNs << "ns prefix=" << r.prefixUs
                 << "us delete=" << r.deleteMs << "ms memory=" << r.memoryBytes/1024 << "KB\n";
        }
    }
}