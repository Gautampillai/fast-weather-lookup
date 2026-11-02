#include "Trie.h"
#include <cctype>

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    clear(root);
}

void Trie::clear(TrieNode* node) {
    if (!node) {
        return;
    }
    for (auto& pair : node->children) {
        clear(pair.second);
    }
    delete node;
}

void Trie::insert(const std::string& word, int dataIndex) {
    TrieNode* current = root;
    for (char ch : word) {
        char lowerCh = std::tolower(ch);
        if (current->children.find(lowerCh) == current->children.end()) {
            current->children[lowerCh] = new TrieNode();
        }
        current = current->children[lowerCh];
    }
    current->isEndOfWord = true;
    current->dataIndex = dataIndex;
}

TrieNode* Trie::searchPrefixNode(const std::string& prefix) {
    TrieNode* current = root;
    for (char ch : prefix) {
        char lowerCh = std::tolower(ch);
        if (current->children.find(lowerCh) == current->children.end()) {
            return nullptr;
        }
        current = current->children[lowerCh];
    }
    return current;
}

void Trie::findMatches(TrieNode* node, std::string currPrefix, std::vector<std::string>& results) {
    if (!node) {
        return;
    }
    if (node->isEndOfWord) {
        results.push_back(currPrefix);
    }
    for (auto& pair : node->children) {
        findMatches(pair.second, currPrefix + pair.first, results);
    }
}

std::vector<std::string> Trie::getMatches(const std::string& prefix) {
    std::vector<std::string> results;
    TrieNode* prefixNode = searchPrefixNode(prefix);
    if (!prefixNode) {
        return results;
    }

    findMatches(prefixNode, prefix, results);
    return results;
}