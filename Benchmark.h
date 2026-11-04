//
// Created by Esteban on 11/3/2026.
//

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "CityIndex.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <unordered_set>
using namespace chrono;

struct BenchResult {
    string name;
    size_t n = 0;
    double insertMs = 0, findNs = 0, prefixUs = 0, deleteMs = 0;
    size_t memoryBytes = 0;
    string stats;
    long int hits = 0;
    size_t matches = 0;
};

// To make the simulation and comparisons perform at a larger scale, I will be using the random
// library's mt19937 RNG in the benchmark test for the data structures. Basically, in the benchmark,
// we are going to create a variety of fake stations as random unique keys like "abcdef|xyzz" to scale
// past the (approximately) 300 real stations :]

inline vector<string> syntheticKeys(size_t n, unsigned int seed = 42) {
    mt19937 rng(seed);
    unordered_set<string> seen;
    vector<string> out;
    out.reserve(n);

    while (out.size() < n) {
        string k;
        int len = 4 + static_cast<int>(rng()%7);
        for (int i = 0; i < len; i++) k += static_cast<char>('a' + rng()%26);
        k += '|';
        for (int i = 0; i < 4; ++i) k += static_cast<char>('a' + rng()%26);
        if (seen.insert(k).second) out.push_back(k);
    }

    return out;
}

// Same workload for both data structures like insert all, look up all (shuffled, repeated so tiny datasets
// still give measurable time), 50 autocomplete prefix searches (3 chars, first 10), and delete all
inline BenchResult runBenchmark(CityIndex& idx, const vector<string>& keys) {
    using clk = steady_clock;
    auto ms = [](clk::time_point a, clk::time_point b) {
        return duration<double, milli>(b - a).count();
    };

    BenchResult r;
    r.name = idx.name();
    r.n = keys.size();
    idx.clear();

    auto t0 = clk::now();
    for (size_t i = 0; i < keys.size(); i++) idx.insert(keys[i], static_cast<int>(i));

    auto t1 = clk::now();
    r.insertMs = ms(t0, t1);
    r.memoryBytes = idx.memoryBytes();
    r.stats = idx.stats();

    vector<string> order = keys;
    mt19937 rng(7);
    shuffle(order.begin(), order.end(), rng);

    size_t rounds = max<size_t>(1, 1000000 / order.size());
    long int hits = 0;
    t0 = clk::now();
    for (size_t k = 0; k < rounds; k++) {
        for (const auto& s : order) {
            if (idx.find(s) >= 0) hits++;
            else hits--;
        }
    }

    t1 = clk::now();
    r.findNs = (ms(t0, t1)*1000000)/static_cast<double>(order.size()*rounds);

    vector<string> prefixes;
    for (int i = 0; i < 50; i++) {
        const auto& k = keys[rng()%keys.size()];
        prefixes.push_back(k.substr(0, min<size_t>(3, k.size())));
    }

    size_t pRounds = max<size_t>(1, 200000/keys.size());
    size_t found = 0;
    t0 = clk::now();
    for (size_t k = 0; k < pRounds; ++k) {
        for (const auto& p : prefixes) {
            found += idx.prefixSearch(p, 10).size();
        }
    }

    t1 = clk::now();
    r.prefixUs = ms(t0, t1)*1000/static_cast<double>(prefixes.size()*pRounds);

    t0 = clk::now();
    for (const auto& s : order)
        idx.remove(s);

    t1 = clk::now();
    r.deleteMs = ms(t0, t1);
    r.hits = hits;
    r.matches = found;
    return r;
}

#endif //BENCHMARK_H
