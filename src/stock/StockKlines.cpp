#include "stock/StockKlines.h"
#include <iostream>

std::vector<uiKline> StockKlines::GetWeekKlines(const std::vector<uiKline>& day_klines) {
    std::vector<uiKline> week_klines;
    uiKline week_kline;
    int prev_week = 0, this_week = 0, ndays = 0, prev_week_price_close = 0.0;
    std::string prev_day = "";

    prev_day = day_klines[0].day;
    struct tm prev_day_tm;
    if (!strptime(prev_day.c_str(), "%Y-%m-%d", &prev_day_tm)) {
        std::cerr << "Error parsing date: " << prev_day << std::endl;
    }
    prev_week = prev_day_tm.tm_yday / 7 + 1;
    prev_week_price_close = day_klines[0].price_open;

    for (size_t i = 0; i < day_klines.size(); i++) {
        uiKline day_kline = day_klines[i];
        time_t current_day_t;
        struct tm day_time;
        if (!strptime(day_kline.day.c_str(), "%Y-%m-%d", &day_time)) {
            std::cerr << "Error parsing date: " << day_kline.day << std::endl;
            continue;
        }
        this_week = day_time.tm_yday / 7 + 1;
        // 如果当前是交易日，且上周不为0，则保存上周的信息，并清空week_kline
        if (this_week != prev_week) {
            week_kline.day = prev_day;                              // 周开盘日
            prev_week_price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为周收盘价
            week_kline.price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为周收盘价
            week_kline.change_rate =
                (week_kline.price_close - prev_week_price_close) / prev_week_price_close;  // 周涨跌幅
            week_kline.change_amount = week_kline.price_close - prev_week_price_close;     // 周涨跌额
            week_klines.push_back(week_kline);

            week_kline.price_open = day_kline.price_open;
            week_kline.price_close = 0;
            week_kline.change_rate = 0;
            week_kline.change_amount = 0;
            week_kline.price_max = -100000000;
            week_kline.price_min = 1000000000;
            week_kline.market_cap = 0;
            week_kline.volume = 0;
            week_kline.amount = 0;
            ndays = 0;
            prev_day = day_kline.day;
            prev_week = this_week;
        }
        week_kline.market_cap = day_kline.market_cap;         // 股票市值
        week_kline.volume += day_kline.volume;                // 周成交量
        week_kline.amount += day_kline.amount;                // 周成交额
        week_kline.turnover_rate += day_kline.turnover_rate;  // 周换手率
        week_kline.price_open = day_kline.price_open;
        week_kline.price_close = day_kline.price_close;
        if (day_kline.price_max > week_kline.price_max) {
            week_kline.price_max = day_kline.price_max;
        }
        if (day_kline.price_min < week_kline.price_min) {
            week_kline.price_min = day_kline.price_min;
        }
    }

    ////////// 最后收尾
    week_kline.price_close = day_klines[day_klines.size() - 1].price_close;  // 周收盘价是上一个交易日的收盘价
    week_kline.change_rate = (week_kline.price_close - week_kline.price_open) / week_kline.price_open;  // 周涨跌幅
    week_kline.change_amount = week_kline.price_close - week_kline.price_open;                          // 周涨跌额
    week_klines.push_back(week_kline);

    return week_klines;
}
// 计算月 K 线
std::vector<uiKline> StockKlines::GetMonthKlines(const std::vector<uiKline>& day_klines) {
    std::vector<uiKline> month_klines;
    uiKline month_kline;
    int prev_month = 0, this_month = 0, prev_month_price_close = 0.0;
    std::string prev_day = "";

    prev_day = day_klines[0].day;
    struct tm prev_day_tm;
    if (!strptime(prev_day.c_str(), "%Y-%m-%d", &prev_day_tm)) {
        std::cerr << "Error parsing date: " << prev_day << std::endl;
        return month_klines;
    }
    prev_month = prev_day_tm.tm_mon + 1;
    prev_month_price_close = day_klines[0].price_open;

    for (size_t i = 0; i < day_klines.size(); i++) {
        uiKline day_kline = day_klines[i];
        time_t current_day_t;
        struct tm day_time;
        if (!strptime(day_kline.day.c_str(), "%Y-%m-%d", &day_time)) {
            std::cerr << "Error parsing date: " << day_kline.day << std::endl;
            continue;
        }
        this_month = day_time.tm_mon + 1;

        if (this_month != prev_month) {
            month_kline.day = prev_day;                              // 月开盘日
            prev_month_price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为月收盘价
            month_kline.price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为月收盘价
            month_kline.change_rate =
                (month_kline.price_close - prev_month_price_close) / prev_month_price_close;  // 月涨跌幅
            month_kline.change_amount = month_kline.price_close - prev_month_price_close;     // 月涨跌额
            month_klines.push_back(month_kline);

            month_kline.price_open = day_kline.price_open;
            month_kline.price_close = 0;
            month_kline.change_rate = 0;
            month_kline.change_amount = 0;
            month_kline.price_max = -100000000;
            month_kline.price_min = 1000000000;
            month_kline.market_cap = 0;
            month_kline.volume = 0;
            month_kline.amount = 0;
            prev_day = day_kline.day;
            prev_month = this_month;
        }
        month_kline.market_cap += day_kline.market_cap;        // 股票市值
        month_kline.volume += day_kline.volume;                // 月成交量
        month_kline.amount += day_kline.amount;                // 月成交额
        month_kline.turnover_rate += day_kline.turnover_rate;  // 月换手率
        month_kline.price_open = day_kline.price_open;
        month_kline.price_close = day_kline.price_close;
        if (day_kline.price_max > month_kline.price_max) {
            month_kline.price_max = day_kline.price_max;
        }
        if (day_kline.price_min < month_kline.price_min) {
            month_kline.price_min = day_kline.price_min;
        }
    }

    // 最后收尾
    month_kline.price_close = day_klines[day_klines.size() - 1].price_close;  // 月收盘价是最后一个交易日的收盘价
    month_kline.change_rate = (month_kline.price_close - month_kline.price_open) / month_kline.price_open;  // 月涨跌幅
    month_kline.change_amount = month_kline.price_close - month_kline.price_open;  // 月涨跌额
    month_klines.push_back(month_kline);

    return month_klines;
}

// 计算季度 K 线
std::vector<uiKline> StockKlines::GetQuarterKlines(const std::vector<uiKline>& day_klines) {
    std::vector<uiKline> quarter_klines;
    uiKline quarter_kline;
    int prev_quarter = 0, this_quarter = 0, prev_quarter_price_close = 0.0;
    std::string prev_day = "";

    prev_day = day_klines[0].day;
    struct tm prev_day_tm;
    if (!strptime(prev_day.c_str(), "%Y-%m-%d", &prev_day_tm)) {
        std::cerr << "Error parsing date: " << prev_day << std::endl;
        return quarter_klines;
    }
    this_quarter = (prev_day_tm.tm_mon + 1) / 3 + 1;
    prev_quarter_price_close = day_klines[0].price_open;

    for (size_t i = 0; i < day_klines.size(); i++) {
        uiKline day_kline = day_klines[i];
        time_t current_day_t;
        struct tm day_time;
        if (!strptime(day_kline.day.c_str(), "%Y-%m-%d", &day_time)) {
            std::cerr << "Error parsing date: " << day_kline.day << std::endl;
            continue;
        }
        this_quarter = (day_time.tm_mon + 1) / 3 + 1;

        if (this_quarter != prev_quarter) {
            quarter_kline.day = prev_day;                              // 季度开盘日
            prev_quarter_price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为季度收盘价
            quarter_kline.price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为季度收盘价
            quarter_kline.change_rate =
                (quarter_kline.price_close - prev_quarter_price_close) / prev_quarter_price_close;  // 季度涨跌幅
            quarter_kline.change_amount = quarter_kline.price_close - prev_quarter_price_close;     // 季度涨跌额
            quarter_klines.push_back(quarter_kline);

            quarter_kline.price_open = day_kline.price_open;
            quarter_kline.price_close = 0;
            quarter_kline.change_rate = 0;
            quarter_kline.change_amount = 0;
            quarter_kline.price_max = -100000000;
            quarter_kline.price_min = 1000000000;
            quarter_kline.market_cap = 0;
            quarter_kline.volume = 0;
            quarter_kline.amount = 0;
            prev_day = day_kline.day;
            prev_quarter = this_quarter;
        }
        quarter_kline.market_cap += day_kline.market_cap;        // 股票市值
        quarter_kline.volume += day_kline.volume;                // 季度成交量
        quarter_kline.amount += day_kline.amount;                // 季度成交额
        quarter_kline.turnover_rate += day_kline.turnover_rate;  // 季度换手率
        quarter_kline.price_open = day_kline.price_open;
        quarter_kline.price_close = day_kline.price_close;
        if (day_kline.price_max > quarter_kline.price_max) {
            quarter_kline.price_max = day_kline.price_max;
        }
        if (day_kline.price_min < quarter_kline.price_min) {
            quarter_kline.price_min = day_kline.price_min;
        }
    }

    // 最后收尾
    quarter_kline.price_close = day_klines[day_klines.size() - 1].price_close;  // 季度收盘价是最后一个交易日的收盘价
    quarter_kline.change_rate =
        (quarter_kline.price_close - quarter_kline.price_open) / quarter_kline.price_open;  // 季度涨跌幅
    quarter_kline.change_amount = quarter_kline.price_close - quarter_kline.price_open;     // 季度涨跌额
    quarter_klines.push_back(quarter_kline);

    return quarter_klines;
}

// 计算年 K 线
std::vector<uiKline> StockKlines::GetYearKlines(const std::vector<uiKline>& day_klines) {
    std::vector<uiKline> year_klines;
    uiKline year_kline;
    int prev_year = 0, this_year = 0, prev_year_price_close = 0.0;
    std::string prev_day = "";

    prev_day = day_klines[0].day;
    struct tm prev_day_tm;
    if (!strptime(prev_day.c_str(), "%Y-%m-%d", &prev_day_tm)) {
        std::cerr << "Error parsing date: " << prev_day << std::endl;
        return year_klines;
    }
    prev_year = prev_day_tm.tm_year + 1900;
    prev_year_price_close = day_klines[0].price_open;

    for (size_t i = 0; i < day_klines.size(); i++) {
        uiKline day_kline = day_klines[i];
        struct tm day_time;
        if (!strptime(day_kline.day.c_str(), "%Y-%m-%d", &day_time)) {
            std::cerr << "Error parsing date: " << day_kline.day << std::endl;
            continue;
        }
        this_year = day_time.tm_year + 1900;

        if (this_year != prev_year) {
            year_kline.day = prev_day;                              // 年开盘日
            prev_year_price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为年收盘价
            year_kline.price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为年收盘价
            year_kline.change_rate =
                (year_kline.price_close - prev_year_price_close) / prev_year_price_close;  // 年涨跌幅
            year_kline.change_amount = year_kline.price_close - prev_year_price_close;     // 年涨跌额
            year_klines.push_back(year_kline);

            year_kline.price_open = day_kline.price_open;
            year_kline.price_close = 0;
            year_kline.change_rate = 0;
            year_kline.change_amount = 0;
            year_kline.price_max = -100000000;
            year_kline.price_min = 1000000000;
            year_kline.market_cap = 0;
            year_kline.volume = 0;
            year_kline.amount = 0;
            prev_day = day_kline.day;
            prev_year = this_year;
        }
        year_kline.market_cap += day_kline.market_cap;        // 股票市值
        year_kline.volume += day_kline.volume;                // 年成交量
        year_kline.amount += day_kline.amount;                // 年成交额
        year_kline.turnover_rate += day_kline.turnover_rate;  // 年换手率
        year_kline.price_open = day_kline.price_open;
        year_kline.price_close = day_kline.price_close;
        if (day_kline.price_max > year_kline.price_max) {
            year_kline.price_max = day_kline.price_max;
        }
        if (day_kline.price_min < year_kline.price_min) {
            year_kline.price_min = day_kline.price_min;
        }
    }

    // 最后收尾
    year_kline.price_close = day_klines[day_klines.size() - 1].price_close;  // 年收盘价是最后一个交易日的收盘价
    year_kline.change_rate = (year_kline.price_close - year_kline.price_open) / year_kline.price_open;  // 年涨跌幅
    year_kline.change_amount = year_kline.price_close - year_kline.price_open;                          // 年涨跌额
    year_klines.push_back(year_kline);

    return year_klines;
}