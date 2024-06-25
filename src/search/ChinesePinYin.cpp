#include "search/ChinesePinYin.h"
#include <fstream>
#include <iostream>
#include "search/Trie.h"
#include "util/FileTool.h"
#include "util/Global.h"

std::unordered_map<std::string, std::vector<std::string>> ChinesePinYin::pinyin_dict = {};

std::vector<std::string> ChinesePinYin::GetLetters(const std::string& chinese) {
    std::vector<std::string> letters = {};
    if (pinyin_dict.size() == 0) {
        LoadPinYinDictionary(FileTool::CurrentPath() + "ChinesePinYin.dic");
    }
    /* 遍历查找汉字-拼音对照表的内容并将汉字替换为拼音 */
    for (size_t i = 0; i < chinese.length();) {
        NEXT_UTF8_CHAR(i, chinese, letter);
        if (pinyin_dict.find(letter) != pinyin_dict.end()) {
            std::vector<std::string> multi_pinyin = pinyin_dict[letter];
            if (letters.empty()) {
                for (const std::string& py : multi_pinyin) {
                    letters.push_back(py);
                }
            } else {
                std::vector<std::string> new_letters;
                for (const std::string& py : letters) {
                    for (const std::string& py_new : multi_pinyin) {
                        new_letters.push_back(py + py_new);
                    }
                }
                letters = new_letters;
            }
        } else {
            if (letters.empty()) {
                letters.push_back(letter);
            } else {
                for (std::string& py : letters) {
                    py += letter;
                }
            }
        }
    }
    return letters;
}

std::vector<std::string> ChinesePinYin::GetFirstLetters(const std::string& chinese) {
    std::vector<std::string> letters = {};
    if (pinyin_dict.size() == 0) {
        LoadPinYinDictionary(FileTool::CurrentPath() + "ChinesePinYin.dic");
    }
    /* 遍历查找汉字-拼音对照表的内容并将汉字替换为拼音 */
    for (size_t i = 0; i < chinese.length();) {
        NEXT_UTF8_CHAR(i, chinese, letter);
        if (pinyin_dict.find(letter) != pinyin_dict.end()) {
            std::vector<std::string> multi_pinyin = pinyin_dict[letter];
            if (letters.empty()) {
                for (const std::string& py : multi_pinyin) {
                    letters.push_back(py.substr(0, 1));
                }
            } else {
                std::vector<std::string> new_letters;
                for (const std::string& py : letters) {
                    for (const std::string& py_new : multi_pinyin) {
                        new_letters.push_back(py + py_new.substr(0, 1));
                    }
                }
                letters = new_letters;
            }
        } else {
            if (letters.empty()) {
                letters.push_back(letter);
            } else {
                for (std::string& py : letters) {
                    py += letter;
                }
            }
        }
    }
    return letters;
}

bool ChinesePinYin::LoadPinYinDictionary(const std::string& dict_path) {
    std::ifstream file(dict_path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::string> parts = split(line, " ");
            std::vector<std::string> pinyin = split(parts[0], ",");
            pinyin_dict[parts[1]] = pinyin;
        }
        file.close();
        return true;
    } else if (errno == ENOENT) {
        std::cerr << dict_path << " file not found" << std::endl;
        return false;
    } else if (errno == EACCES) {
        std::cerr << dict_path << " file no access permission" << std::endl;
        return false;
    } else {
        std::cerr << dict_path << " open failed, unknown reason" << std::endl;
        return false;
    }
}
