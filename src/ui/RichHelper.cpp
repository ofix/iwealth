#include "ui/RichHelper.h"
#include "util/Global.h"

wxString RichUnit(double num) {
    if (num == 0) {
        return wxString("---");  // 股票停牌没有成交量和成交额
    }
    std::string result;
    if (num >= 100000000) {
        num /= 100000000;
        result = convert_double(num, 2) + "亿";
    } else if (num >= 10000) {
        num /= 10000;
        result = convert_double(num, 2) + "万";
    } else {
        result = std::to_string(static_cast<int>(num));
    }
    return CN(result);
}
