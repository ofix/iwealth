#include "SpiderShareKlineProvider.h"

SpiderShareKlineProvider::SpiderShareKlineProvider() {
}

SpiderShareKlineProvider::~SpiderShareKlineProvider() {
}

bool SpiderShareKlineProvider::IsNaN(const std::string& data) {
    if (data.length() == 0) {
        return true;  // 空字符串
    }
    if (data.length() >= 2) {
        if (data[1] == '-') {  // 检查第二个字符是否是-,如果是，
            return true;       // ---或者--都是不合法的，为了快速判断，无需判断其他字符
        }
        return false;
    }
    return std::isdigit(data[0]) ? false : true;  // 只有一个字符的情况，判断是否是数字
}