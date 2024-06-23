
#include "search/Trie.h"

using std::string;
using std::unordered_map;
using std::vector;

Trie::node::node() {
    depth = 0;
    is_word = false;
    child = unordered_map<std::string, node*>();
    share_code = "";
}

Trie::node::node(int depth) : depth(depth) {
    is_word = false;
    child = unordered_map<std::string, node*>();
    share_code = "";
}

Trie::node::~node() {
    for (std::pair<std::string, node*> pair : child) {
        delete pair.second;
    }
}

Trie::Trie() {
    root = new node();
}

void Trie::build(const std::vector<std::pair<std::string, std::string>>& words) {
    for (std::pair<std::string, std::string> word : words) {
        insert(word.first, word.second);
    }
}

// 支持中英文混合
void Trie::insert(const std::string& word, const std::string& share_code) {
    node* n = root;
    for (size_t i = 0; i < word.length();) {  // utf-8字符串
        NEXT_UTF8_CHAR(i, word)
        if (n->child.find(c) == n->child.end()) {  // 没有找到对应的字符
            n->child[c] = new node(n->depth + 1);
        }
        n = n->child[c];
    }
    n->is_word = true;
    n->share_code = share_code;
}

bool Trie::search(const string& word) {
    node* n = root;
    for (size_t i = 0; i < word.length();) {  // utf-8字符串
        NEXT_UTF8_CHAR(i, word)
        if (n->child.find(c) == n->child.end()) {
            return false;
        }
        n = n->child[c];
    }
    return n->is_word;
}

void Trie::remove(const string& word) {
    node* n = root;
    for (size_t i = 0; i < word.length();) {  // utf-8字符串
        NEXT_UTF8_CHAR(i, word)
        if (n->child.find(c) == n->child.end()) {
            return;
        }
        n = n->child[c];
    }
    n->is_word = false;
}

void Trie::removePrefixWith(const string& word) {
    node* n = root;
    for (size_t i = 0; i < word.length();) {  // utf-8字符串
        NEXT_UTF8_CHAR(i, word)
        if (n->child.find(c) == n->child.end()) {
            return;
        }
        n = n->child[c];
    }

    for (std::pair<std::string, node*> pair : n->child) {
        delete pair.second;
    }
    n->child = unordered_map<std::string, node*>();
    n->share_code = "";
    n->is_word = false;
}

void Trie::insertWord(node* n, std::string word, std::vector<std::string>* list) {
    if (n->is_word) {
        list->push_back(n->share_code);  // 返回股票代码
    }
    for (std::pair<std::string, node*> pair : n->child) {
        insertWord(pair.second, word + pair.first, list);
    }
}

vector<string> Trie::listPrefixWith(const string& word) {
    node* n = root;
    vector<string> list;
    for (size_t i = 0; i < word.length();) {  // utf-8字符串
        NEXT_UTF8_CHAR(i, word)
        if (n->child.find(c) == n->child.end()) {
            return list;
        }
        n = n->child[c];
    }

    insertWord(n, word, &list);
    return list;
}

vector<string> Trie::list() {
    vector<string> list;
    insertWord(root, "", &list);
    return list;
}

void Trie::maxDepth(node* n, int* max) {
    if (n->is_word && n->depth > *max) {
        *max = n->depth;
    }
    for (std::pair<std::string, node*> pair : n->child) {
        maxDepth(pair.second, max);
    }
}

int Trie::maxDepth() {
    int max = 0;
    maxDepth(root, &max);
    return max;
}

void Trie::wordCount(node* n, int* num) {
    if (n->is_word) {
        *num = *num + 1;
    }
    for (std::pair<std::string, node*> pair : n->child) {
        wordCount(pair.second, num);
    }
}

int Trie::wordCount() {
    int num = 0;
    wordCount(root, &num);
    return num;
}
