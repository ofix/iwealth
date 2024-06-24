#ifndef ChinesePinYin_H
#define ChinesePinYin_H

#include <string>
#include <unordered_map>
#include <vector>

class ChinesePinYin {
   public:
    static std::string GetFirstLetters(const std::string& chinese);

   private:
    static bool LoadPinYinDictionary(const std::string& dict_path);
    static bool m_inited = false;
    static std::unordered_map<std::string, std::vector<std::string>> m_pinyin_dict;
};

#endif  // ChinesePinYin_H