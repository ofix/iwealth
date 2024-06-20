#include "util/DateTime.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

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