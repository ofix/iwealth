#include "util/DateTime.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <unordered_map>

/// @brief windows不存在strptime函数
/// @param s 日期字符串
/// @param f 日期格式化字符串
/// @param tm struct tm日期结构指针
/// @return 格式化好的日期字符串
extern "C" char* strptime(const char* s, const char* f, struct tm* tm) {
    // Isn't the C++ standard lib nice? std::get_time is defined such that its
    // format parameters are the exact same as strptime. Of course, we have to
    // create a string stream first, and imbue it with the current C locale, and
    // we also have to make sure we return the right things if it fails, or
    // if it succeeds, but this is still far simpler an implementation than any
    // of the versions in any of the C standard libraries.
    std::istringstream input(s);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(tm, f);
    if (input.fail()) {
        return nullptr;
    }
    return (char*)(s + input.tellg());
}

/**
 * @param start_time 开始时间，格式要求 "09:00" 这种形式
 * @param end_time   结束时间，格式要求 "09:30" 这种形式
 * @result true: 在指定时间范围内，false: 不在指定时间范围内
 */
bool between_time_period(const std::string& start_time, const std::string& end_time) {
    time_t now = time(0);
    struct tm* local = localtime(&now);
    char buff[sizeof("hh:mm")];
    strftime(buff, sizeof(buff), "%H:%M", local);
    return (buff >= start_time) && (buff <= end_time);
}

/**
 * @param format 日期格式化参数，
 * @example now("%Y-%m-%d %H:%M:%S")
 */
std::string now(const std::string& format) {
    time_t t = std::time(nullptr);
    std::tm* localTime = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(localTime, format.c_str());
    return oss.str();
}

std::string format_time(std::time_t t, const std::string format) {
    std::tm* localTime = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(localTime, format.c_str());
    return oss.str();
}

// 检查日期是否是交易日
bool is_trade_day(const std::string& day) {
    std::tm tm = {};
    std::istringstream ss(day);
    // 格式化日期字符串为年-月-日
    char ch;
    ss >> tm.tm_year >> ch >> tm.tm_mon >> ch >> tm.tm_mday;
    // tm_year是从1900年开始的年数，tm_mon是从0开始的月数
    tm.tm_year -= 1900;
    tm.tm_mon--;
    // 转换为time_t
    std::time_t t = mktime(&tm);
    struct tm* trade_day = std::localtime(&t);
    std::string format_trade_day = format_time(t, "%Y-%m-%d");
    if (trade_day->tm_wday != 0 && trade_day->tm_wday != 6 &&
        !is_chinese_holiday(format_trade_day)) {  // 如果当天是周六/周日/节假日，日期往前一天，但交易天数保持不变
        return true;
    }
    return false;
}

/**
 * @param start_time 开始时间，格式要求 "2024-06-20 09:00:12" 这种形式
 * @param end_time   结束时间，格式要求 "2024-06-20 09:30:00" 这种形式
 * @result int: 时间间隔的秒数
 */
long long diff_seconds(const std::string& start_time, const std::string& end_time) {
    struct tm time1, time2;
    if (!strptime(start_time.c_str(), "%Y-%m-%d %H:%M:%S", &time1) ||
        !strptime(end_time.c_str(), "%Y-%m-%d %H:%M:%S", &time2)) {
        return -1;
    }

    auto tp1 = std::chrono::system_clock::from_time_t(mktime(&time1));
    auto tp2 = std::chrono::system_clock::from_time_t(mktime(&time2));

    return std::chrono::duration_cast<std::chrono::seconds>(tp2 - tp1).count();
}

/**
 * @brief  获取最近交易日交易日期，只考虑周末，暂不考虑中国节假日带来的影响
 * @param days 交易日天数，如果是-1，则表示最近交易日的上一个交易日，默认是0，则表示最近交易日或者当天交易日
 * @return 交易日期 格式 YYYY-mm-dd,比如 2024-03-05
 */
std::string get_nearest_trade_day(int days) {
    days = std::abs(days);  // 取整数
    time_t now = std::time(nullptr);
    struct tm* trade_day = localtime(&now);
    std::string format_trade_day = format_time(now, "%Y-%m-%d");
    while (days > 0) {
        if (trade_day->tm_wday != 0 && trade_day->tm_wday != 6 &&
            !is_chinese_holiday(format_trade_day)) {  // 如果当天是周六/周日/节假日，日期往前一天，但交易天数保持不变
            days -= 1;
        }
        now -= 24 * 60 * 60;
        trade_day = localtime(&now);

        format_trade_day = format_time(now, "%Y-%m-%d");
    }

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", trade_day);
    return std::string(buf);
}

bool is_chinese_holiday(std::string day) {
    static std::unordered_map<std::string, bool> holidays = {
        // 2022年1月份
        {"2022-01-03", true},  // 2022年元旦
        {"2022-01-31", true},  // 2021年除夕
        // 2022年2月份
        {"2022-02-01", true},  // 2021年春节
        {"2022-02-02", true},
        {"2022-02-03", true},
        {"2022-02-04", true},
        // 2022年4月份
        {"2022-04-04", true},  // 2022年清明节
        {"2022-04-05", true},
        // 2022年5月份
        {"2022-05-02", true},  // 2022年劳动节
        {"2022-05-03", true},
        {"2022-05-04", true},
        // 2022年6月份
        {"2022-06-03", true},  // 2022年端午节
        // 2022年9月份
        {"2022-09-12", true},  // 2022年中秋节
        // 2022年10月份
        {"2022-10-03", true},  // 2022年国庆节
        {"2022-10-04", true},
        {"2022-10-05", true},
        {"2022-10-06", true},
        {"2022-10-07", true},
        // 2023年1月份
        {"2023-01-02", true},  // 2023年元旦
        {"2023-01-23", true},  // 2022年春节
        {"2023-01-24", true},
        {"2023-01-25", true},
        {"2023-01-26", true},
        {"2023-01-27", true},
        // 2023年4月份
        {"2023-04-05", true},  // 2023年清明节
        // 2023年5月份
        {"2023-05-01", true},  // 2023年劳动节
        {"2023-05-02", true},
        {"2023-05-03", true},
        // 2023年6月份
        {"2023-06-22", true},  // 2023年端午节
        {"2023-06-23", true},
        // 2023年9月份
        {"2023-09-29", true},  // 2023年中秋节
        // 2023年10月份
        {"2023-10-02", true},  // 2023年国庆节
        {"2023-10-03", true},
        {"2023-10-04", true},
        {"2023-10-05", true},
        {"2023-10-06", true},
        // 2024年1月份
        {"2024-01-01", true},  // 2024年元旦
        // 2024年2月份
        {"2024-02-12", true},  // 2023年春节
        {"2024-02-13", true},
        {"2024-02-14", true},
        {"2024-02-15", true},
        {"2024-02-16", true},
        // 2024年4月份
        {"2024-04-04", true},  // 2024年清明节
        {"2024-04-05", true},
        // 2024年5月份
        {"2024-05-01", true},  // 2024年劳动节
        {"2024-05-02", true},
        {"2024-05-03", true},
        // 2024年6月份
        {"2024-06-10", true},  // 2024年端午节
        {"2024-09-16", true},
        {"2024-09-17", true},
        {"2024-10-01", true},
        {"2024-10-02", true},
        {"2024-10-03", true},
        {"2024-10-04", true},
    };
    if (holidays.find(day) != holidays.end()) {
        return true;
    }
    return false;
}

/// @brief 比较时间大小，格式 YYYY-mm-dd HH:mm:ss 标准格式
/// @return 0: time1 == time2
///        -1: time1  < time2
///         1: time1  > time2
int compare_time(const std::string& time1, const std::string& time2) {
    std::tm tm1, tm2;

    if (!strptime(time1.c_str(), "%Y-%m-%d %H:%M:%S", &tm1) || !strptime(time2.c_str(), "%Y-%m-%d %H:%M:%S", &tm2)) {
        return -1;
    }

    // 比较年、月、日、时、分、秒
    if (tm1.tm_year != tm2.tm_year) {
        return tm1.tm_year - tm2.tm_year;
    } else if (tm1.tm_mon != tm2.tm_mon) {
        return tm1.tm_mon - tm2.tm_mon;
    } else if (tm1.tm_mday != tm2.tm_mday) {
        return tm1.tm_mday - tm2.tm_mday;
    } else if (tm1.tm_hour != tm2.tm_hour) {
        return tm1.tm_hour - tm2.tm_hour;
    } else if (tm1.tm_min != tm2.tm_min) {
        return tm1.tm_min - tm2.tm_min;
    } else if (tm1.tm_sec != tm2.tm_sec) {
        return tm1.tm_sec - tm2.tm_sec;
    } else {
        return 0;
    }
}