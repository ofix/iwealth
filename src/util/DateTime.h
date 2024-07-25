#ifndef DATETIME_H
#define DATETIME_H

#include <string>

extern "C" char* _strptime(const char* s, const char* f, struct tm* tm);
// 计算某一天属于一年中的第几天
int day_of_year(int year, int month, int day);
// 计算某一天星期几
int week_of_day(int year, int month, int day);
// 检查当前时间是否在指定时间范围
bool between_time_period(const std::string& start_time, const std::string& end_time);
// 获取当前时间字符串，支持格式化
std::string now(const std::string& format = "%Y-%m-%d %H:%M:%S");
// 检查两个时间字符串间隔的秒数
long long diff_seconds(const std::string& start_time, const std::string& end_time);
int diff_days(const std::string& start_time, const std::string& end_time);
// 获取最近交易日
std::string get_nearest_trade_day(int days = 0);
// 检查日期是否是中国节假日
bool is_chinese_holiday(std::string day);
// 检查日期是否是交易日
bool is_trade_day(const std::string& day);
// 比较时间大小，格式 YYYY-mm-dd HH:mm:ss 标准格式
int compare_time(const std::string& time1, const std::string& time2);
// 格式化日期
std::string format_time(const time_t t, const std::string format);

#endif  // DATETIME_H