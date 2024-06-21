#ifndef DATETIME_H
#define DATETIME_H

#include <string>

// 检查当前时间是否在指定时间范围
bool between_time_period(const std::string& start_time, const std::string& end_time);
// 获取当前时间字符串，支持格式化
std::string now(const std::string& format = "%Y-%m-%d %H:%M:%S");
// 检查两个时间字符串间隔的秒数
long long diff_seconds(const std::string& start_time, const std::string& end_time);
// 获取最近交易日
std::string get_nearest_trade_day();
// 比较时间大小，格式 YYYY-mm-dd HH:mm:ss 标准格式
int compare_time(const std::string& time1, const std::string& time2);

#endif  // DATETIME_H