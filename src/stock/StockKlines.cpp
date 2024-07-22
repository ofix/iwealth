#include "stock/StockKlines.h"
#include <iostream>
#include "util/DateTime.h"
#include "util/FileTool.h"
#include "util/Global.h"

RichResult StockKlines::QueryMinuteKlines(const std::string& share_code, std::vector<minuteKline>* minute_klines) {
    std::vector<minuteKline>* pMinuteKlines = m_minute_klines.Query(share_code);
    if (pMinuteKlines) {
        minute_klines = pMinuteKlines;
        return Success();
    }
    // 缓存不存在
    RichResult result = FetchMinuteKlines(share_code, *minute_klines);
    if (result.Ok()) {
        m_minute_klines.Add(share_code, *minute_klines);  // 缓存最近300条记录
    } else {
        minute_klines->clear();  // 如果请求出错，删除错误数据
    }

    return result;
}

RichResult StockKlines::QueryFiveDayMinuteKlines(const std::string& share_code,
                                                 std::vecotr<minuteKline>* five_day_minute_klines) {
    std::vector<minuteKline>* pFiveDayKlines = m_fiveday_klines.Query(share_code);
    if (pFiveDayKlines) {
        five_day_minute_klines = pFiveDayKlines;
        return Success();
    }
    // 缓存不存在
    RichResult result = FetchFiveDayMinuteKlines(share_code, *five_day_minute_klines);
    if (result.Ok()) {
        m_fiveday_klines.Add(share_code, *five_day_minute_klines);  // 缓存最近300条记录
    } else {
        five_day_minute_klines->clear();  // 如果请求出错，删除错误数据
    }

    return result;
}

RichResult StockKlines::QueryDayKlines(const std::string& share_code, std::vector<uiKline>* day_klines) {
    // 检查内存缓存是否存在
    std::vector<uiKline>* pDayKlines = m_day_kline_adjust.Query(share_code);
    if (pDayKlines) {
        day_klines = pDayKlines;
        return Success();
    }
    // 检查本地文件是否存在，
    // 1. 本地文件不存在，远程抓取全量数据
    // 2. 本地文件存在，但数据过期，远程抓取增量数据
    // 3. 将数据
    // 检查本地文件是否过期
    // 如果过期，
    // 如果未过期，
    // 缓存不存在
    RichResult result = FetchFiveDayMinuteKlines(share_code, *five_day_minute_klines);
    if (result.Ok()) {
        m_fiveday_klines.Add(share_code, *five_day_minute_klines);  // 缓存最近300条记录
    } else {
        five_day_minute_klines->clear();  // 如果请求出错，删除错误数据
    }

    return result;
}

RichResult StockKlines::QueryWeekKlines(const std::string& share_code, std::vector<uiKline>* week_klines) {
    std::vector<uiKline>* pWeekKlines = m_week_kline_adjust.Query(share_code);
    if (pWeekKlines) {
        week_klines = pWeekKlines;
        return Success();
    }
    // 检查内存中是否缓存日K线
    std::vector<uiKline>* pDayKlines = m_day_kline_adjust.Query(share_code);
    if (pDayKlines) {
        *week_klines = GenerateWeekKLines(*pDayKlines);  // 计算数据
        return Success();
    }
    // 检查本地文件
    std::string file_path = GetFilePathOfDayKline(share_code);
    std::vector<uiKline> day_klines;
    RichResult result;
    if (FileTool::IsFileExists(file_path)) {
        if (IsLocalFileExpired(file_path)) {
            result = FetchIncrementalDayKlines(file_path, day_klines);  // 增量爬取
            if (result.Ok()) {
                SaveIncrementalDayKlinesInCsvFile(day_klines);  // 保存到文件
            }
        } else {
            result = LoadLocalDayKline(share_code, day_klines);  // 直接加载
        }
    } else {
        result = FetchDayKlines(share_code, day_klines);  // 全量爬取
        if (result.Ok()) {
            SaveDayKlinesInCsvFile(day_klines);  // 保存到文件
        }
    }
    if (!result.Ok()) {
        return result;
    }
    m_day_kline_adjust.Add(share_code, day_klines);  // 缓存到内存
    *week_klines = GenerateWeekKLines(day_klines);   // 计算数据
    return Success();
}

RichResult StockKlines::FetchIncrementalDayKlines(const std::string& file_path, std::vector<uiKline>& day_klines) {
    //
}
RichResult StockKlines::QueryMonthKlines(const std::string& share_code, std::vector<uiKline>* month_klines) {
    std::vector<uiKline>* pWeekKlines = m_week_kline_adjust.Query(share_code);
    if (pWeekKlines) {
        week_klines = pWeekKlines;
        return Success();
    }
    // 检查内存中是否缓存日K线
    std::vector<uiKline>* pDayKlines = m_day_kline_adjust.Query(share_code);
    if (pDayKlines) {
        *week_klines = GenerateWeekKLines(*pDayKlines);  // 计算数据
        return Success();
    }
    // 检查本地文件
    std::string file_path = GetFilePathOfDayKline(share_code);
    std::vector<uiKline> day_klines;
    RichResult result;
    if (FileTool::IsFileExists(file_path)) {
        if (IsLocalFileExpired(file_path)) {
            result = FetchIncrementalDayKlines(file_path, day_klines);  // 增量爬取
            if (result.Ok()) {
                SaveIncrementalDayKlinesInCsvFile(day_klines);  // 保存到文件
            }
        } else {
            result = LoadLocalDayKline(share_code, day_klines);  // 直接加载
        }
    } else {
        result = FetchDayKlines(share_code, day_klines);  // 全量爬取
        if (result.Ok()) {
            SaveDayKlinesInCsvFile(day_klines);  // 保存到文件
        }
    }
    if (!result.Ok()) {
        return result;
    }
    m_day_kline_adjust.Add(share_code, day_klines);  // 缓存到内存
    *week_klines = GenerateWeekKLines(day_klines);   // 计算数据
    return Success();
}
RichResult StockKlines::QueryQuarterKlines(const std::string& share_code, std::vector<uiKline>* quarter_klines) {
    std::vector<uiKline>* pWeekKlines = m_week_kline_adjust.Query(share_code);
    if (pWeekKlines) {
        week_klines = pWeekKlines;
        return Success();
    }
    // 检查内存中是否缓存日K线
    std::vector<uiKline>* pDayKlines = m_day_kline_adjust.Query(share_code);
    if (pDayKlines) {
        *week_klines = GenerateWeekKLines(*pDayKlines);  // 计算数据
        return Success();
    }
    // 检查本地文件
    std::string file_path = GetFilePathOfDayKline(share_code);
    std::vector<uiKline> day_klines;
    RichResult result;
    if (FileTool::IsFileExists(file_path)) {
        if (IsLocalFileExpired(file_path)) {
            result = FetchIncrementalDayKlines(file_path, day_klines);  // 增量爬取
            if (result.Ok()) {
                SaveIncrementalDayKlinesInCsvFile(day_klines);  // 保存到文件
            }
        } else {
            result = LoadLocalDayKline(share_code, day_klines);  // 直接加载
        }
    } else {
        result = FetchDayKlines(share_code, day_klines);  // 全量爬取
        if (result.Ok()) {
            SaveDayKlinesInCsvFile(day_klines);  // 保存到文件
        }
    }
    if (!result.Ok()) {
        return result;
    }
    m_day_kline_adjust.Add(share_code, day_klines);  // 缓存到内存
    *week_klines = GenerateWeekKLines(day_klines);   // 计算数据
    return Success();
}
RichResult StockKlines::QueryYearKlines(const std::string& share_code, std::vector<uiKline>* year_klines) {
    std::vector<uiKline>* ptr_year_klines = m_year_kline_adjust.Query(share_code);
    if (ptr_year_klines) {
        year_klines = ptr_year_klines;
        return Success();
    }
    // 检查内存中是否缓存日K线
    std::vector<uiKline>* ptr_day_klines = m_day_kline_adjust.Query(share_code);
    if (ptr_day_klines) {
        *year_klines = GenerateYearKlines(*ptr_day_klines);  // 计算数据
        return Success();
    }
    // 检查本地文件
    std::string file_path = GetFilePathOfDayKline(share_code);
    std::vector<uiKline> day_klines;
    RichResult result;
    if (FileTool::IsFileExists(file_path)) {
        if (IsLocalFileExpired(file_path)) {
            result = FetchIncrementalDayKlines(file_path, day_klines);  // 增量爬取
            if (result.Ok()) {
                SaveIncrementalDayKlinesInCsvFile(day_klines);  // 保存到文件
            }
        } else {
            result = LoadLocalDayKline(share_code, day_klines);  // 直接加载
        }
    } else {
        result = FetchDayKlines(share_code, day_klines);  // 全量爬取
        if (result.Ok()) {
            SaveDayKlinesInCsvFile(day_klines);  // 保存到文件
        }
    }
    if (!result.Ok()) {
        return result;
    }
    m_day_kline_adjust.Add(share_code, day_klines);  // 缓存到内存
    *week_klines = GenerateWeekKLines(day_klines);   // 计算数据
    return Success();
}

RichResult StockKlines::FetchDayKline(const std::string& share_code, std::vector<uiKline>& day_klines) {
    return Success();
}

RichResult StockKlines::FetchMinuteKlines(const std::string& share_code, std::vector<minuteKline>& minute_klines) {
    return Success();
}

RichResult StockKlines::FetchFiveDayMinuteKlines(const std::string& share_code,
                                                 std::vector<minuteKline>& fiveday_klines) {
    return Success();
}

RichResult StockKlines::LoadLocalDayKline(const std::string share_code, std::vector<uiKline>& day_klines) {
    std::string file_path = GetFilePathOfDayKline(share_code);
    if (!FileTool::IsFileExists(file_path)) {
        return Error(RichStatus::FILE_NOT_FOUND);
    }
    std::string file_path = GetFilePathShareKline(share_code);
    std::string lines = FileTool::LoadFile(file_path);
    std::vector<std::string> klines = split(lines, "\n");
    bool data_dirty = false;
    for (auto& kline : klines) {
        if (kline.length() < 10) {
            data_dirty = true;
            break;
        }
        // 检查最后一个字符是否是\r,需要排除掉，否则会导致通过share_code无法找到Share*,进而股票行业和地域无法显示
        if (kline[kline.size() - 1] == '\r') {
            kline.pop_back();
        }
        std::vector<std::string> fields = split(kline, ",");
        uiKline ui_kline;
        ui_kline.day = fields[0];
        ui_kline.price_open = std::stod(fields[1]);     // 开盘价
        ui_kline.price_close = std::stod(fields[2]);    // 收盘价
        ui_kline.price_max = std::stod(fields[3]);      // 最高价
        ui_kline.price_min = std::stod(fields[4]);      // 最低价
        ui_kline.volume = std::stoull(fields[5]);       // 成交量
        ui_kline.amount = std::stod(fields[6]);         // 成交额
        ui_kline.change_amount = std::stod(fields[7]);  // 涨跌额
        ui_kline.change_rate = std::stod(fields[8]);    // 涨跌幅
        ui_kline.turnover_rate = std::stod(fields[9]);  // 换手率
        day_klines->push_back(ui_kline);
    }

    if (data_dirty) {  // 数据污染了？
        day_klines.clear();
        return Error(RichStatus::FILE_DIRTY);
    }
    return Success();
}

RichResult StockKlines::LoadLocalMinuteKline(const std::string file_path, std::vector<uiKline>& minute_klines) {
    return Success();
}

RichResult StockKlines::LoadLocalFiveDayMinuteKline(const std::string file_path, std::vector<uiKline>& fiveday_klines) {
    return Success();
}

bool StockKlines::IsLocalFileExist(const std::string file_path) {
    return FileTool::IsFileExists(file_path);
}

bool StockKlines::IsLocalFileExpired(const std::string file_path) {
    // 获取本地行情数据文件修改时间
    std::string local_quote_file_modified_time = FileTool::GetFileModifiedTime(file_path);
    std::string today = now("%Y-%m-%d");
    std::string now_time = now("%Y-%m-%d %H:%M:%S");
    if (is_trade_day(today)) {  // 如果当天是交易日
        std::string last_trade_day = get_nearest_trade_day(-1);
        std::string last_trade_close_time = last_trade_day + " 15:00:00";  // 上一个交易日的收盘时间
        std::string current_trade_day = get_nearest_trade_day();
        std::string current_trade_open_time = current_trade_day + " 09:30:00";          // 当天开盘时间
        std::string current_trade_close_time = current_trade_day + " 15:00:00";         // 当天收盘时间
        if (compare_time(local_quote_file_modified_time, last_trade_close_time) > 0 &&  // 文件时间大于昨天收盘时间
            compare_time(now_time, current_trade_open_time) < 0 &&                      // 当前时间未开盘
            compare_time(local_quote_file_modified_time, current_trade_open_time) < 0  // 文件时间小于今天开盘时间
        ) {
            return false;
        }

        if (compare_time(local_quote_file_modified_time, current_trade_close_time) > 0) {  // 文件时间大于当天收盘时间
            return false;
        }

        return true;
    } else {
        std::string last_trade_day = get_nearest_trade_day();
        std::string last_trade_close_time = last_trade_day + " 15:00:00";
        // 检查文件修改时间是否 > 最近交易日收盘时间
        if (compare_time(local_quote_file_modified_time, last_trade_close_time) > 0) {
            return false;
        }
        return true;
    }
}

std::string StockKlines::GetFilePathOfDayKline(const std::string& share_code) {
    return FileTool::CurrentPath() + "data" + DIR_SEPARATOR + "day" + DIR_SEPARATOR + share_code + ".csv";
}

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

std::vector<uiKline> StockKlines::GeneratePeriodKlines(const std::vector<uiKline>& day_klines,
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
std::vector<uiKline> StockKlines::GenerateWeekKLines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYearWeek, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GeneratePeriodKlines(day_klines, period_func);
}

// 计算月 K 线
std::vector<uiKline> StockKlines::GenerateMonthKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYearMonth, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GeneratePeriodKlines(day_klines, period_func);
}

// 计算季度 K 线
std::vector<uiKline> StockKlines::GenerateQuarterKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYearQuarter, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GeneratePeriodKlines(day_klines, period_func);
}

// 计算年 K 线
std::vector<uiKline> StockKlines::GenerateYearKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockKlines::GetYear, std::placeholders::_1, std::placeholders::_2);
    return StockKlines::GeneratePeriodKlines(day_klines, period_func);
}