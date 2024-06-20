#ifndef DATETIME_H
#define DATETIME_H

#include <string>

// 检查当前时间是否在指定时间范围
bool between_time_period(const std::string& start_time, const std::string& end_time);
// 获取当前时间字符串，支持格式化
std::string now(const std::string& format = "%Y-%m-%d %H:%M:%S");
// 检查两个时间字符串间隔的秒数
long long diff_seconds(const std::string& start_time, const std::string& end_time);

#endif  // DATETIME_H