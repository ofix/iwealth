#include "search/ChinesePinYin.h"

std::string ChinesePinYin::GetFirstLetters(const std::string& chinese) {
    std::string pinyin = "";
    std::vector<std::string> letters = chinese.split("");
    if (!m_inited) {
        LoadPinYinDictionary("./ChinesePinyin.dic");
    }
    /* 遍历查找汉字-拼音对照表的内容并将汉字替换为拼音 */
    for (auto& letter : letters) {
        if (m_pinyin_dict.contains(letter)) {
            pinyin += m_pinyin_dict[letter].first();
        } else {
            pinyin += letter;
        }
    }
    return pinyin;
}

bool ChinesePinYin::LoadPinYinDictionary(const std::string& dict_path) {
    std::FILE file(dict_path);
    if (!file.open()) {
        return m_pinyin_dict;
    }

    /* 读取汉字对照表文件并转换为QMap存储 */
    while (!file.atEnd()) {
        wxString content = wxSting::FromUtf8(file.readLine());
        m_pinyin_dict[content.split(" ").last().trimmed()] = content.split(" ").first().split(",");
    }

    file.close();

    return m_pinyin_dict;
}
