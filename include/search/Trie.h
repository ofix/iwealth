#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class Trie {
   public:
    class node {
       public:
        node();
        node(int);
        ~node();
        int depth;
        bool is_word;
        std::unordered_map<char, node*> child;
    };

   public:
    Trie();
    void build(const std::vector<std::string>& words);
    void insert(const std::string& word);
    bool search(const std::string& word);
    void remove(const std::string& word);
    void removePrefixWith(const std::string& word);
    std::vector<std::string> listPrefixWith(const std::string& word);
    std::vector<std::string> list();
    int maxDepth();
    int wordCount();

   private:
    void insertWord(node*, std::string, std::vector<std::string>*);
    void maxDepth(node*, int*);
    void wordCount(node*, int*);
    node* root;
};