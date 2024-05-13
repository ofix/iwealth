#include "util/global.h"

/**
 * @todo 通过管道调用系统命令并获取命令执行输出结果
 * @param cmd 需要运行的命令字符串，支持UTF-8
 * @author songhuabiao@greatwall.com.cn
 */
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

/**
 * @todo 获取当前时间
 * @param
 */
std::string now() {
    std::time_t now = std::time(nullptr);
    struct tm time;
    char buf[80];
    time = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &time);
    return buf;
}

/**
 * @todo 分割字符串
 * @param str 待分割的原始字符串
 * @param separator 分割字符
 * @author songhuabiao@greatwall.com.cn
 */
std::vector<std::string> split(const std::string& str, const char& separator) {
    std::smatch matches;
    std::string origin = str;
    std::regex rule{separator};
    std::vector<std::string> result;
    bool found = false;
    while (std::regex_search(origin, matches, rule)) {
        found = true;
        result.push_back(matches.prefix().str());
        origin = matches.suffix();
    }
    if (!found) {
        result.push_back(str);
    }
    return result;
}

/**
 * @todo 移除字符串左边的空白字符(\r\n\t\s)，修改原始字符串
 * @param str 原始字符串
 * @author songhuabiao@greatwall.com.cn
 */
void ltrim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
                  return !std::isspace(ch);
              }));
}

/**
 * @todo 移除字符串右边的空白字符(\r\n\t\s)，修改原始字符串
 * @param str 原始字符串
 * @author songhuabiao@greatwall.com.cn
 */
void rtrim(std::string& str) {
    str.erase(std::find_if(str.rbegin(), str.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              str.end());
}

/**
 * @todo 移除字符串两边的空白字符(\r\n\t\s)，修改原始字符串
 * @param str 原始字符串
 * @author songhuabiao@greatwall.com.cn
 */
void trim(std::string& str) {
    ltrim(str);
    rtrim(str);
}

/**
 * @todo 移除字符串两边的空白字符(\r\n\t\s)，不修改原始字符串
 * @param str 原始字符串
 * @return 返回移除空白字符后的字符串
 * @author songhuabiao@greatwall.com.cn
 */
std::string trim_copy(std::string str) {
    trim(str);
    return str;
}

/**
 * @todo 移除字符串左边的空白字符(\r\n\t\s)，不修改原始字符串
 * @param str 原始字符串
 * @return 返回移除空白字符后的字符串
 * @author songhuabiao@greatwall.com.cn
 */
std::string ltrim_copy(std::string str) {
    ltrim(str);
    return str;
}

/**
 * @todo 移除字符串右边的空白字符(\r\n\t\s)，不修改原始字符串
 * @param str 原始字符串
 * @return 返回移除空白字符后的字符串
 * @author songhuabiao@greatwall.com.cn
 */
std::string rtrim_copy(std::string str) {
    rtrim_copy(str);
    return str;
}

void padding_left(std::string& str, uint16_t size, const char needle) {
    if (size <= str.length()) {
        return;
    }
    str.insert(0, size - str.length(), needle);
}

void padding_right(std::string& str, uint16_t size, const char needle) {
    if (size <= str.length()) {
        return;
    }
    str.insert(str.length(), size - str.length(), needle);
}

void fill_chars(std::string& str,
                const char needle,
                size_t width,
                const std::string direction) {
    size_t n = width - str.length();
    if (n > 0) {
        std::string spaces = repeat_chars(needle, n);
        if (direction == "left") {
            str = str + spaces;
        } else {
            str = spaces + str;
        }
    }
}

std::string to_upper_case(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string to_lower_case(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void to_upper_case(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void to_lower_case(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::string repeat_chars(char ch, int count) {
    std::string result = "";
    for (int i = 0; i < count; i++) {
        result += ch;
    }
    return result;
}

int substring_count(const std::string& str, const std::string& sub) {
    int count = 0;
    size_t pos = str.find(sub);

    while (pos != std::string::npos) {
        count++;
        pos = str.find(sub, pos + sub.length());
    }

    return count;
}

/**
 * @todo 检查字符串是否以另一字符串结尾
 * @param origin 原始字符串
 * @param target 查找字符串
 * @author songhuabiao@greatwall.com.cn
 */
bool end_with(const std::string& origin, const std::string& target) {
    size_t size_target = target.size();
    size_t size_origin = origin.length();

    if (size_target > size_origin) {
        return false;
    }

    for (size_t i = size_target; i > 0; i--) {
        if (target[size_target - i] != origin[size_origin - i]) {
            return false;
        }
    }
    return true;
}
