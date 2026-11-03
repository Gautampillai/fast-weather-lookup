//
// Created by Esteban on 11/2/2026.
//

#ifndef CITYINDEX_H
#define CITYINDEX_H

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
using namespace std;

// Keys are stored lowercase ("birmingham|alabama") so both structures behave the same
inline std::string normalizeKey(std::string s) {
    for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

// Shared interface: the CLI and benchmark (the test) treat both structures identically
// In other words, both the trie and hash map indexes will use the same main algorithms, making it easier to compare time complexities :]
class CityIndex {
public:
    CityIndex() = default; // Constructor
    virtual ~CityIndex() = default; // Destructor
    virtual string name() = 0; // Returns data structure (Trie or Hash Map)
    virtual bool insert(const string& key, int dataIndex) = 0; // Inserts a station and a key to data structure, returns false if key already exists
    virtual int find(const string& key) = 0; // Returns database index of key, if doesn't exist then return -1
    virtual bool remove(const string& key) = 0; // Removes key (city/station) from data structure, returns false if doesn't exist
    virtual vector<string> prefixSearch(const string& prefix, size_t limit = SIZE_MAX) = 0; // Returns max number of keys (cities) starting with the prefix
    virtual size_t size() = 0; // Returns number of keys stored
    virtual size_t memoryBytes() = 0; // Returns data structure space usage estimate
    virtual string stats() = 0; // Returns description of data structure, such as bucket count and node count for instance
    virtual void clear() = 0; // Empties data structure
};

#endif //CITYINDEX_H
