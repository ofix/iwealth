#include "ui/RichHelper.h"
#include <iomanip>
#include <sstream>

std::string convertDouble(double digit, int precision) {
    std::stringstream sstream;
    sstream << std::fixed << std::setprecision(precision) << digit;
    return sstream.str();
}

wxString RichUnit(double num) {
    std::string result;
    if (num >= 100000000) {
        num /= 100000000;
        result = convertDouble(num, 2) + "亿";
    } else if (num >= 10000) {
        num /= 10000;
        result = convertDouble(num, 2) + "万";
    } else {
        result = std::to_string(num);
    }
    return CN(result);
}
