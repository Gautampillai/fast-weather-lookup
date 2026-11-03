//
// Created by Esteban on 11/3/2026.
//

#ifndef TRIEINDEX_H
#define TRIEINDEX_H

#include "CityIndex.h"
#include "Trie.h"
#include <algorithm>

// Supporting file for Gautam's Trie.h and Trie.cpp files :O
// Just like the HashMapIndex.h file, helps with data structure comparisons
class TrieIndex : public CityIndex {
    Trie* trie;
    size_t count = 0;

    static size_t countNodes(const TrieNode* n) {
        size_t c = 1;
        for (const auto& p : n->children)
            c += countNodes(p.second);
        return c;
    }

public:
    TrieIndex() : trie(new Trie()) {}
    TrieIndex(const TrieIndex&) = delete;
    TrieIndex& operator=(const TrieIndex&) = delete;

    ~TrieIndex() override {
        delete trie;
    }

    string name() override {
        return "Trie";
    }

    bool insert(const string& rawKey, int dataIndex) override {
        string key = normalizeKey(rawKey);
        bool isNew = find(key) < 0;
        trie->insert(key, dataIndex);
        if (isNew) ++count;
        return isNew;
    }

    int find(const string& rawKey) override {
        TrieNode* n = trie->searchPrefixNode(normalizeKey(rawKey));
        if (n && n->isEndOfWord)
            return n->dataIndex;
        return -1;
    }

    bool remove(const string& rawKey) override {
        string key = normalizeKey(rawKey);
        TrieNode* n = trie->searchPrefixNode("");
        vector<pair<TrieNode*, char>> path;

        for (char c : key) {
            auto it = n->children.find(c);
            if (it == n->children.end()) return false;
            path.push_back({n, c});
            n = it->second;
        }

        if (!n->isEndOfWord) return false;
        n->isEndOfWord = false;
        n->dataIndex = -1;
        count--;

        for (size_t i = path.size(); i > 0; i--) {
            TrieNode* child = path[i-1].first->children[path[i-1].second];
            if (child->isEndOfWord || !child->children.empty()) break;
            delete child;
            path[i-1].first->children.erase(path[i-1].second);
        }

        return true;
    }

    vector<string> prefixSearch(const std::string& rawPrefix, size_t limit = SIZE_MAX) override {
        auto all = trie->getMatches(normalizeKey(rawPrefix));
        sort(all.begin(), all.end());
        if (all.size() > limit) all.resize(limit);
        return all;
    }

    size_t size() override {
        return count;
    }

    size_t memoryBytes() override {
        size_t nodes = countNodes(trie->searchPrefixNode(""));
        return nodes*sizeof(TrieNode) + (nodes - 1)*40;
    }

    string stats() override {
        size_t nodes = countNodes(trie->searchPrefixNode(""));
        string nodesPerKey = "";
        if (count > 0) nodesPerKey = to_string(static_cast<double>(nodes)/count).substr(0, 5);
        else nodesPerKey = to_string(count).substr(0, 5);

        return "nodes=" + to_string(nodes) + "  nodes per key=" + nodesPerKey;
    }

    void clear() override {
        delete trie;
        trie = new Trie();
        count = 0;
    }
};

#endif //TRIEINDEX_H
