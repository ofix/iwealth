///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/util/DateTime.cpp
// Purpose:     date time global help functions
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-17 12:33
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "util/DateTime.h"
#include <chrono>
#include <clocale>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

/// @brief windows不存在_strptime函数
/// @param s 日期字符串
/// @param f 日期格式化字符串
/// @param tm struct tm日期结构指针
/// @return 格式化好的日期字符串
extern "C" char* _strptime(const char* s, const char* f, struct tm* tm) {
    // Isn't the C++ standard lib nice? std::get_time is defined such that its
    // format parameters are the exact same as _strptime. Of course, we have to
    // create a string stream first, and imbue it with the current C locale, and
    // we also have to make sure we return the right things if it fails, or
    // if it succeeds, but this is still far simpler an implementation than any
    // of the versions in any of the C standard libraries.
    std::istringstream input(s);
    char* _local = setlocale(LC_ALL, nullptr);
    input.imbue(std::locale(_local));
    input >> std::get_time(tm, f);
    if (input.fail()) {
        return nullptr;
    }
    // 修正 tm_yday 不正确的问题
    tm->tm_yday = day_of_year(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    tm->tm_wday = week_of_day(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    return (char*)(s + input.tellg());
}

int day_of_year(int year, int month, int day) {
    static int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // If current year is a leap year and the date
    // given is after the 28th of February then
    // it must include the 29th February
    if (month > 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        ++day;
    }
    // Add the days in the previous months
    while (month-- > 1) {
        day = day + days[month - 1];
    }
    return day - 1;
}

int week_of_day(int year, int month, int day) {
    if (month == 1 || month == 2) {
        month += 12;
        year--;
    }
    // 使用基姆拉尔森计算公式
    int week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    if (week == 6) {
        return 0;
    }
    return week + 1;
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

/**
 * @brief  基于当天日期计算未来/过去的某一天日期
 * @param ndays 从当天算起，间隔天数
 * @return std::string YYYY-mm-dd 格式，比如 2024-07-25
 */
std::string get_day_from_now(int ndays) {
    // using namespace std::literals::chrono_literals;
    auto clock = std::chrono::system_clock::now();
    clock += std::chrono::hours(24 * ndays);  // 24h
    auto rawtime = std::chrono::system_clock::to_time_t(clock);

    std::tm* local_time = std::localtime(&rawtime);
    char buffer[80] = {};
    std::strftime(buffer, 80, "%Y-%m-%d", local_time);
    return std::string(buffer);
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
    if ((trade_day->tm_wday != 0) && (trade_day->tm_wday != 6) &&
        (!is_chinese_holiday(format_trade_day))) {  // 如果当天是周六/周日/节假日，日期往前一天，但交易天数保持不变
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
    if (!_strptime(start_time.c_str(), "%Y-%m-%d %H:%M:%S", &time1) ||
        !_strptime(end_time.c_str(), "%Y-%m-%d %H:%M:%S", &time2)) {
        return -1;
    }

    auto tp1 = std::chrono::system_clock::from_time_t(mktime(&time1));
    auto tp2 = std::chrono::system_clock::from_time_t(mktime(&time2));

    return std::chrono::duration_cast<std::chrono::seconds>(tp2 - tp1).count();
}

/**
 * @param start_day 开始时间，格式要求 "2024-06-20" 这种形式
 * @param end_day  结束时间，格式要求 "2024-06-21" 这种形式
 * @result int: 时间间隔的天数
 */

int diff_days(const std::string& start_day, const std::string& end_day) {
    struct tm t1, t2;
    if (!_strptime(start_day.c_str(), "%Y-%m-%d", &t1) || !_strptime(end_day.c_str(), "%Y-%m-%d", &t2)) {
        return -1;
    }
    // 时分秒，并不会被正确初始化
    t1.tm_hour = 0;
    t1.tm_min = 0;
    t1.tm_sec = 0;
    t2.tm_hour = 0;
    t2.tm_min = 0;
    t2.tm_sec = 0;

    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);

    double diff_seconds = difftime(time2, time1);
    int days = static_cast<int>(diff_seconds / (60 * 60 * 24));
    return days;
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

    if (days == 0) {
        while (trade_day->tm_wday == 0 || trade_day->tm_wday == 6 ||
               is_chinese_holiday(format_trade_day)) {  // 如果当天是周六/周日/节假日，日期往前一天，但交易天数保持不变
            now -= 24 * 60 * 60;
            trade_day = localtime(&now);
            format_trade_day = format_time(now, "%Y-%m-%d");
        }
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

    if (!_strptime(time1.c_str(), "%Y-%m-%d %H:%M:%S", &tm1) || !_strptime(time2.c_str(), "%Y-%m-%d %H:%M:%S", &tm2)) {
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