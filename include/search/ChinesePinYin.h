#ifndef ChinesePinYin_H
#define ChinesePinYin_H

#include <string>
#include <unordered_map>
#include <vector>

// UTF8,中文转汉语拼音
class ChinesePinYin {
   public:
    static std::vector<std::string> GetFirstLetters(const std::string& chinese);
    static std::vector<std::string> GetLetters(const std::string& chinese);
    static std::vector<std::string> ToCharList(const std::string& chinese);

   private:
    static bool LoadPinYinDictionary(const std::string& dict_path);
    static std::vector<std::string> RemoveRepeats(std::vector<std::string>& letters);
    static std::unordered_map<std::string, std::vector<std::string>> pinyin_dict;
};

#endif  // ChinesePinYin_H