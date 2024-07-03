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
    std::time_t t = std::time(nullptr);
    std::tm* localTime = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(localTime, format.c_str());
    return oss.str();
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
 * @param days 允许为负数，如果是-1，则表示最近交易日的上一个交易日，默认是0，则表示最近交易日
 * @return 交易日期 格式 YYYY-mm-dd,比如 2024-03-05
 */
std::string get_nearest_trade_day(int days) {
    time_t now = std::time(nullptr);
    struct tm* trade_day = localtime(&now);

    while (days > 0) {
        now -= 24 * 60 * 60;
        if (trade_day->tm_wday == 0 || trade_day->tm_wday == 6) {  // 如果当天是周日和周六，排除
            trade_day = localtime(&now);
        } else {
            days -= 1;
        }
    }

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", trade_day);
    return std::string(buf);
}

bool is_chinese_holiday(std::string day) {
    static std::unordered_map<std::string, bool> holidays = {
        {"", true},
        {"", true},
        {"", true},
        {"", true},
    }
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