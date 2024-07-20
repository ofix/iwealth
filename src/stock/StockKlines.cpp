#include "stock/StockKlines.h"
#include <iostream>
#include "util/DateTime.h"

bool StockKlines::GetYearWeek(const std::string& day, int& week) {
    struct tm day_tm;
    if (!strptime(day.c_str(), "%Y-%m-%d", &day_tm)) {
        return false;
    }
    week = day_tm.tm_yday / 7 + 1;
    return true;
}

bool StockKlines::GetYearMonth(const std::string& day, int& month) {
    std::string _month = day.substr(5, 2);
    if (_month[0] == '0') {
        month = atoi(_month.substr(1, 1).c_str());
        return true;
    }
    month = atoi(_month.c_str());
    return true;
}

bool StockKlines::GetYearQuarter(const std::string& day, int& quarter) {
    int month;
    GetYearMonth(day, month);
    quarter = month / 4 + 1;
    return true;
}

bool StockKlines::GetYear(const std::string& day, int& year) {
    std::string _year = day.substr(0, 4);
    year = atoi(_year.c_str());
    return true;
}

std::vector<uiKline> StockKlines::GetPeriodKlines(const std::vector<uiKline>& day_klines,
                                                  std::function<bool(const std::string&, int&)> period_func) {
    std::vector<uiKline> _klines;
    uiKline _kline;
    int prev_period = 0, this_period = 0, prev_period_price_close = 0.0;
    std::string period_start_day = "";
    std::string period_day = "";

    period_start_day = day_klines[0].day;
    _kline = day_klines[0];
    period_func(period_start_day, prev_period);
    prev_period_price_close = (day_klines[0]).price_min;  // 将第一天上市的最低价(发行价)作为基准价

    for (size_t i = 1; i < day_klines.size(); i++) {
        uiKline day_kline = day_klines[i];
        period_func(day_kline.day, this_period);  // 当前是第几周，第几月, 第几季，第几年
        if (this_period != prev_period) {         // 保存上一周的周K线
            _kline.price_close = day_klines[i - 1].price_close;  // 将上一个交易日的收盘价作为周期收盘价
            _kline.day = period_start_day;                       // 周期开盘日是周期第一天
            _kline.change_rate = (_kline.price_close - prev_period_price_close) / prev_period_price_close;  // 周涨跌幅
            _kline.change_amount = _kline.price_close - prev_period_price_close;  // 周涨跌额
            _klines.push_back(_kline);
            prev_period_price_close = _kline.price_close;  // 将上一个交易日的收盘价作为周收盘价
            //////////// 初始化本周K线数据
            period_start_day = day_kline.day;
            prev_period = this_period;
            _kline = day_kline;
        } else {
            _kline.market_cap = day_kline.market_cap;         // 股票市值
            _kline.volume += day_kline.volume;                // 周期成交量
            _kline.amount += day_kline.amount;                // 周期成交额
            _kline.turnover_rate += day_kline.turnover_rate;  // 周期换手率
            if (day_kline.price_max > _kline.price_max) {
                _kline.price_max = day_kline.price_max;
            }
            if (day_kline.price_min < _kline.price_min) {
                _kline.price_min = day_kline.price_min;
            }
        }
    }

    ////////// 剩余未添加的记录 ////////

    _kline.price_close = day_klines[day_klines.size() - 1].price_close;  // 周收盘价是最后一个交易日的收盘价
    _kline.change_rate = (_kline.price_close - prev_period_price_close) / prev_period_price_close;  // 周涨跌幅
    _kline.change_amount = _kline.price_close - prev_period_price_close;                            // 周涨跌额
    _klines.push_back(_kline);
    return _klines;
}

// 计算周K线
std::vector<uiKline> StockKlines::GetWeekKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYearWeek, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GetPeriodKlines(day_klines, period_func);
}

// 计算月 K 线
std::vector<uiKline> StockKlines::GetMonthKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYearMonth, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GetPeriodKlines(day_klines, period_func);
}

// 计算季度 K 线
std::vector<uiKline> StockKlines::GetQuarterKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYearQuarter, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GetPeriodKlines(day_klines, period_func);
}

// 计算年 K 线
std::vector<uiKline> StockKlines::GetYearKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYear, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GetPeriodKlines(day_klines, period_func);
}