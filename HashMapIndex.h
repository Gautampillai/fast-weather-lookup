//
// Created by Esteban on 11/2/2026.
//

#ifndef HASHMAPINDEX_H
#define HASHMAPINDEX_H

#include "CityIndex.h"
#include <algorithm>

// Hash table with separate chaining and capacity doubles when LF > 0.75.
class HashMapIndex : public CityIndex {
    struct Node {
        string key;
        int dataIndex;
        Node* next;
    };

    const double MAX_LOAD = 0.75;
    const size_t START_BUCKETS = 16;
    vector<Node*> buckets;
    size_t count = 0;

    static size_t hashKey(const string& k) {
        size_t h = 0;
        for (unsigned char c : k) h = h*31 + c;
        return h;
    }

    void rehash(size_t newSize) {
        vector<Node*> newBucket(newSize, nullptr);
        for (Node* head : buckets)
            while (head) {
                Node* nextBucket = head->next;
                size_t i = hashKey(head->key)%newSize;
                head->next = newBucket[i];
                newBucket[i] = head;
                head = nextBucket;
            }
        buckets.swap(newBucket);
    }

public:
    HashMapIndex() : buckets(START_BUCKETS, nullptr) {}
    HashMapIndex(const HashMapIndex&) = delete;
    HashMapIndex& operator=(const HashMapIndex&) = delete;

    ~HashMapIndex() override {
        clear();
    }

    string name() override {
        return "Hash map";
    }

    bool insert(const string& rawKey, int dataIndex) override {
        string key = normalizeKey(rawKey);
        size_t i = hashKey(key)%buckets.size();

        for (Node* n = buckets[i]; n; n = n->next) {
            if (n->key == key) {
                n->dataIndex = dataIndex;
                return false;
            }
        }

        if (static_cast<double>(count + 1)/buckets.size() > MAX_LOAD) {
            rehash(buckets.size() * 2);
            i = hashKey(key)%buckets.size();
        }

        buckets[i] = new Node{key, dataIndex, buckets[i]};
        count++;
        return true;
    }

    int find(const string& rawKey) override {
        string key = normalizeKey(rawKey);
        size_t i = hashKey(key) % buckets.size();

        for (Node* n = buckets[i]; n; n = n->next)
            if (n->key == key) return n->dataIndex;
        return -1;
    }

    bool remove(const string& rawKey) override {
        string key = normalizeKey(rawKey);
        size_t i = hashKey(key)%buckets.size();

        Node** link = &buckets[i];
        while (*link) {
            if ((*link)->key == key) {
                Node* d = *link;
                *link = d->next;
                delete d;
                count--;
                return true;
            }
            link = &(*link)->next;
        }
        return false;
    }

    // Results are sorted so both structures return comparable outputs
    vector<string> prefixSearch(const string& rawPrefix, size_t limit = SIZE_MAX) override {
        string p = normalizeKey(rawPrefix);
        vector<string> out;
        for (Node* head : buckets) {
            for (Node* n = head; n; n = n->next)
                if (n->key.compare(0, p.size(), p) == 0) {
                    out.push_back(n->key);
                    if (out.size() >= limit) break;
                }
            if (out.size() >= limit) break;
        }
        sort(out.begin(), out.end());
        return out;
    }

    size_t size() override {
        return count;
    }

    double loadFactor() {
        return static_cast<double>(count)/buckets.size();
    }

    size_t longestChain() {
        size_t best = 0;
        for (Node* h : buckets) {
            size_t len = 0;
            for (Node* n = h; n; n = n->next) {
                len++;
                best = max(best, len);
            }
        }
        return best;
    }

    size_t memoryBytes() override {
        size_t b = buckets.size()*sizeof(Node*);
        for (Node* h : buckets) {
            for (Node* n = h; n; n = n->next)
                b += sizeof(Node) + n->key.capacity();
        }
        return b;
    }

    string stats() override {
        return "buckets=" + to_string(buckets.size()) + "  load factor=" + to_string(loadFactor()).substr(0, 4) + "  longest chain=" + to_string(longestChain());
    }

    void clear() override {
        for (Node*& h : buckets) {
            while (h) {
                Node* nextBucket = h->next;
                delete h;
                h = nextBucket;
            }
        }
        count = 0;
        buckets.assign(START_BUCKETS, nullptr);
    }
};

#endif //HASHMAPINDEX_H
