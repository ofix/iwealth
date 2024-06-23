#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define NEXT_UTF8_CHAR(i, word)            \
    int wsize = 1;                         \
    if ((word[i] & 0xf8) == 0xf0) {        \
        wsize = 4;                         \
    } else if ((word[i] & 0xf0) == 0xe0) { \
        wsize = 3;                         \
    } else if ((word[i] & 0xe0) == 0xc0) { \
        wsize = 2;                         \
    }                                      \
    if ((i + wsize) > word.length()) {     \
        wsize = 1;                         \
    }                                      \
    std::string c = word.substr(i, wsize); \
    i += wsize;

class Trie {
   public:
    class node {
       public:
        node();
        node(int);
        ~node();
        int depth;
        bool is_word;
        std::unordered_map<std::string, node*> child;
        std::string share_code;
    };

   public:
    Trie();
    void build(const std::vector<std::pair<std::string, std::string>>& words);
    void insert(const std::string& word, const std::string& share_code);
    bool search(const std::string& word);
    void remove(const std::string& word);
    void removePrefixWith(const std::string& word);
    std::vector<std::string> listPrefixWith(const std::string& word);
    std::unordered_map<std::string, std::vector<std::string>> list();
    int maxDepth();
    int wordCount();

   private:
    void insertWord(node* n, std::string word, std::vector<std::string>* words);
    void insertWord(node* n, std::string word, std::unordered_map<std::string, std::vector<std::string>>* pMap);
    void maxDepth(node*, int*);
    void wordCount(node*, int*);
    node* root;
};