#ifndef TRIE_H
#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <unordered_map>
#include <vector>

struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord = false;
    int dataIndex = -1;
};

class Trie {
private:
    TrieNode* root;
    void clear(TrieNode* node); // Clean up the node
    void findMatches(TrieNode* node, std::string currPrefix, std::vector<std::string>& results); // Recursive function that finds matching words with prefix

public:
    Trie(); // Constructor
    ~Trie(); // Destructor

    void insert(const std::string& word, int dataIndex); // Insert into Trie tree
    TrieNode* searchPrefixNode(const std::string& prefix); // Find node for the prefix
    std::vector<std::string> getMatches(const std::string& prefix); // Find matching words using findMatches
};

#endif // TRIE_H
#endif // TRIE_H