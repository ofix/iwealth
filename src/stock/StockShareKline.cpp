#include "stock/StockShareKline.h"
#include <iostream>
#include "spider/SpiderShareKline.h"
#include "stock/StockDataStorage.h"
#include "util/DateTime.h"
#include "util/FileTool.h"
#include "util/Global.h"
#include "util/RichResult.h"

StockShareKline::StockShareKline(StockDataStorage* pStorage) : m_pStorage(pStorage) {
}

StockShareKline::~StockShareKline() {
}

RichResult StockShareKline::QueryMinuteKlines(const std::string& share_code, std::vector<minuteKline>** minute_klines) {
    std::vector<minuteKline>* pMinuteKlines = m_minute_klines.Query(share_code);
    if (pMinuteKlines) {
        *minute_klines = pMinuteKlines;
        return Success();
    }
    // 缓存不存在
    std::vector<minuteKline> temp_klines;
    RichResult result = FetchMinuteKlines(share_code, temp_klines);
    if (result.Ok()) {
        m_minute_klines.Add(share_code, temp_klines);  // 缓存最近300条记录
    }
    *minute_klines = m_minute_klines.Query(share_code);

    return result;
}

RichResult StockShareKline::QueryFiveDayMinuteKlines(const std::string& share_code,
                                                     std::vector<minuteKline>** five_day_minute_klines) {
    std::vector<minuteKline>* pFiveDayKlines = m_fiveday_klines.Query(share_code);
    if (pFiveDayKlines) {
        *five_day_minute_klines = pFiveDayKlines;
        return Success();
    }
    // 缓存不存在
    std::vector<minuteKline> temp_klines;
    RichResult result = FetchFiveDayMinuteKlines(share_code, temp_klines);
    if (result.Ok()) {
        m_fiveday_klines.Add(share_code, temp_klines);  // 缓存最近300条记录
    }
    *five_day_minute_klines = m_fiveday_klines.Query(share_code);
    return result;
}

RichResult StockShareKline::QueryYearKlines(const std::string& share_code, std::vector<uiKline>** year_klines) {
    return QueryPeriodKlines(share_code, year_klines, m_year_kline_adjust, StockShareKline::GenerateYearKlines);
}

RichResult StockShareKline::QueryQuarterKlines(const std::string& share_code, std::vector<uiKline>** quarter_klines) {
    return QueryPeriodKlines(share_code, quarter_klines, m_quarter_kline_adjust,
                             StockShareKline::GenerateQuarterKlines);
}

RichResult StockShareKline::QueryMonthKlines(const std::string& share_code, std::vector<uiKline>** month_klines) {
    return QueryPeriodKlines(share_code, month_klines, m_month_kline_adjust, StockShareKline::GenerateMonthKlines);
}

RichResult StockShareKline::QueryWeekKlines(const std::string& share_code, std::vector<uiKline>** week_klines) {
    return QueryPeriodKlines(share_code, week_klines, m_week_kline_adjust, StockShareKline::GenerateWeekKLines);
}

RichResult StockShareKline::QueryDayKlines(const std::string& share_code, std::vector<uiKline>** ptr_day_klines) {
    // 检查内存缓存是否存在
    std::vector<uiKline>* pDayKlines = m_day_kline_adjust.Query(share_code);
    if (pDayKlines) {
        *ptr_day_klines = pDayKlines;
        return Success();
    }

    std::string file_path = GetFilePathOfDayKline(share_code);
    std::vector<uiKline> day_klines;
    RichResult result;
    if (FileTool::IsFileExists(file_path)) {
        if (IsLocalFileExpired(file_path)) {
            result = FetchIncrementalDayKlines(share_code, day_klines);  // 增量爬取
            if (!result.Ok()) {                                          // 爬取增量K线数据失败
                return result;
            }
            result = SaveIncrementalDayKlines(share_code, day_klines);  // 保存到文件
            if (!result.Ok()) {                                         // 保存增量K线数据失败
                return result;
            }
            day_klines.clear();
        }
        result = LoadLocalDayKline(share_code, day_klines);  // 加载全量数据
    } else {
        result = FetchDayKlines(share_code, day_klines);  // 全量爬取
        if (!result.Ok()) {
            return result;  // 全量爬取失败
        }
        result = SaveShareDayKline(share_code, day_klines);  // 保存到文件
    }
    if (!result.Ok()) {
        return result;
    }
    m_day_kline_adjust.Add(share_code, day_klines);  // 缓存到内存
    *ptr_day_klines = m_day_kline_adjust.Query(share_code);
    return result;
}

RichResult StockShareKline::QueryPeriodKlines(
    const std::string& share_code,
    std::vector<uiKline>** period_klines,
    StockMemoryKline<uiKline>& memory_cache,
    std::function<std::vector<uiKline>(const std::vector<uiKline>& day_klines)> generate_callback) {
    std::vector<uiKline>* ptr_cache_klines = memory_cache.Query(share_code);
    if (ptr_cache_klines) {
        *period_klines = ptr_cache_klines;
        return Success();
    }
    // 检查日K线缓存，如果不存在或者过期，需要重新拉取
    std::vector<uiKline>* ptr_day_klines;
    RichResult result = QueryDayKlines(share_code, &ptr_day_klines);
    if (!result.Ok()) {
        return result;
    }
    // 根据最新的日K线，计算 周/月/季/年K线
    std::vector<uiKline> tmp_klines = generate_callback(*ptr_day_klines);  // 计算数据
    memory_cache.Add(share_code, tmp_klines);  // 将计算好的 周/月/季/年K线缓存到内存中
    *period_klines = memory_cache.Query(share_code);
    return Success();
}

RichResult StockShareKline::SaveShareDayKline(const std::string& share_code, const std::vector<uiKline>& klines) {
    if (klines.size() == 0) {
        return Error(RichStatus::PARAMETER_ERROR);
    }
    std::string file_path = GetFilePathOfDayKline(share_code);
    std::string lines = "";
    for (const auto& kline : klines) {
        std::string line = "";
        line += kline.day + ",";
        line += convert_double(kline.price_open) + ",";
        line += convert_double(kline.price_close) + ",";
        line += convert_double(kline.price_max) + ",";
        line += convert_double(kline.price_min) + ",";
        line += std::to_string(kline.volume) + ",";
        line += convert_double(kline.amount) + ",";
        line += convert_double(kline.change_amount) + ",";
        line += convert_double(kline.change_rate) + ",";
        line += convert_double(kline.turnover_rate) + "\n";
        lines += line;
    }
    bool result = FileTool::SaveFile(file_path, lines);
    if (!result) {
        return Error(RichStatus::FILE_WRITE_FAILED);
    }
    return Success();
}

RichResult StockShareKline::SaveIncrementalDayKlines(const std::string& share_code,
                                                     const std::vector<uiKline>& klines) {
    if (klines.size() == 0) {
        return Error(RichStatus::PARAMETER_ERROR);
    }
    std::string file_path = GetFilePathOfDayKline(share_code);
    std::string lines = "";
    for (const auto& kline : klines) {
        std::string line = "";
        line += kline.day + ",";
        line += convert_double(kline.price_open) + ",";
        line += convert_double(kline.price_close) + ",";
        line += convert_double(kline.price_max) + ",";
        line += convert_double(kline.price_min) + ",";
        line += std::to_string(kline.volume) + ",";
        line += convert_double(kline.amount) + ",";
        line += convert_double(kline.change_amount) + ",";
        line += convert_double(kline.change_rate) + ",";
        line += convert_double(kline.turnover_rate) + "\n";
        lines += line;
    }
    std::ofstream out;
    out.open(file_path, std::ios::app);  // 以追加模式打开文件
    if (out.is_open()) {
        out << lines;
        out.close();
        return Success();
    } else {
        return Error(RichStatus::FILE_WRITE_FAILED);
    }
}

RichResult StockShareKline::FetchDayKlines(const std::string& share_code, std::vector<uiKline>& day_klines) {
    Share* pShare = m_pStorage->FindShare(share_code);
    SpiderShareKline* pSpiderKline = new SpiderShareKline(nullptr);
    bool result = pSpiderKline->CrawlDayKlineSync(pShare, day_klines);
    if (!result) {
        return Error(RichStatus::NETWORK_ERROR);
    }
    return Success();
}

RichResult StockShareKline::FetchIncrementalDayKlines(const std::string& share_code, std::vector<uiKline>& day_klines) {
    Share* pShare = m_pStorage->FindShare(share_code);
    SpiderShareKline* pSpiderKline = new SpiderShareKline(nullptr);
    std::string end_date = get_day_from_now(1);  // 请求需要明天的日期，才能下载当天的K线
    std::string file_path = GetFilePathOfDayKline(share_code);
    std::string last_line = "";
    bool result = FileTool::GetLastLineOfFile(file_path, last_line);
    if (!result || last_line == "") {  // 错误
        return Error(RichStatus::FILE_DIRTY);
    }
    std::string start_date = last_line.substr(0, 10);
    int ndays = diff_days(start_date, end_date);
    if (ndays <= 0) {
        return Error(RichStatus::INNER_ERROR);
    }
    std::vector<uiKline> tmp_klines;
    result = pSpiderKline->CrawlIncrementDayKlineSync(pShare, end_date, ndays + 1, tmp_klines);
    if (!result) {
        return Error(RichStatus::NETWORK_ERROR);
    }
    for (size_t i = 0; i < tmp_klines.size(); i++) {
        uiKline kline = tmp_klines[i];
        if (kline.day == start_date) {
            day_klines.assign(tmp_klines.begin() + i, tmp_klines.end());
            break;
        }
    }
    return Success();
}

RichResult StockShareKline::FetchMinuteKlines(const std::string& share_code, std::vector<minuteKline>& minute_klines) {
    Share* pShare = m_pStorage->FindShare(share_code);
    if (pShare == nullptr) {
        return Error(RichStatus::SHARE_NOT_EXIST);
    }
    SpiderShareKline* pSpiderKline = new SpiderShareKline(nullptr);
    if (pSpiderKline == nullptr) {
        return Error(RichStatus::MEMORY_ALLOC_FAILED);
    }
    bool result = pSpiderKline->CrawlMinuteKlineSync(pShare, minute_klines);
    if (!result) {
        return Error(RichStatus::NETWORK_ERROR);
    }
    delete pSpiderKline;
    return Success();
}

RichResult StockShareKline::FetchFiveDayMinuteKlines(const std::string& share_code,
                                                     std::vector<minuteKline>& fiveday_klines) {
    Share* pShare = m_pStorage->FindShare(share_code);
    if (pShare == nullptr) {
        return Error(RichStatus::SHARE_NOT_EXIST);
    }
    SpiderShareKline* pSpiderKline = new SpiderShareKline(nullptr);
    if (pSpiderKline == nullptr) {
        return Error(RichStatus::MEMORY_ALLOC_FAILED);
    }
    bool result = pSpiderKline->CrawlFiveDayMinuteKlineSync(pShare, fiveday_klines);
    if (!result) {
        return Error(RichStatus::NETWORK_ERROR);
    }
    delete pSpiderKline;
    return Success();
}

RichResult StockShareKline::LoadLocalDayKline(const std::string share_code, std::vector<uiKline>& day_klines) {
    std::string file_path = GetFilePathOfDayKline(share_code);
    if (!FileTool::IsFileExists(file_path)) {
        return Error(RichStatus::FILE_NOT_FOUND);
    }
    std::string lines = FileTool::LoadFile(file_path);
    std::vector<std::string> klines = split(lines, "\n");
    bool data_dirty = false;
    for (auto& kline : klines) {
        size_t size = kline.length();
        if (size == 0) {
            continue;
        }
        if (size < 10) {
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
        day_klines.push_back(ui_kline);
    }

    if (data_dirty) {  // 数据污染了？
        day_klines.clear();
        return Error(RichStatus::FILE_DIRTY);
    }
    return Success();
}

bool StockShareKline::IsLocalFileExist(const std::string file_path) {
    return FileTool::IsFileExists(file_path);
}

bool StockShareKline::IsLocalFileExpired(const std::string file_path) {
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

std::string StockShareKline::GetFilePathOfDayKline(const std::string& share_code) {
    return FileTool::CurrentPath() + "data" + DIR_SEPARATOR + "day" + DIR_SEPARATOR + share_code + ".csv";
}

bool StockShareKline::GetYearWeek(const std::string& day, int& week) {
    struct tm day_tm;
    if (!_strptime(day.c_str(), "%Y-%m-%d", &day_tm)) {
        return false;
    }
    week = day_tm.tm_yday / 7 + 1;
    return true;
}

bool StockShareKline::GetYearMonth(const std::string& day, int& month) {
    std::string _month = day.substr(5, 2);
    if (_month[0] == '0') {
        month = atoi(_month.substr(1, 1).c_str());
        return true;
    }
    month = atoi(_month.c_str());
    return true;
}

bool StockShareKline::GetYearQuarter(const std::string& day, int& quarter) {
    int month;
    GetYearMonth(day, month);
    quarter = month / 4 + 1;
    return true;
}

bool StockShareKline::GetYear(const std::string& day, int& year) {
    std::string _year = day.substr(0, 4);
    year = atoi(_year.c_str());
    return true;
}

std::vector<uiKline> StockShareKline::GeneratePeriodKlines(const std::vector<uiKline>& day_klines,
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
std::vector<uiKline> StockShareKline::GenerateWeekKLines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockShareKline::GetYearWeek, std::placeholders::_1, std::placeholders::_2);
    return StockShareKline::GeneratePeriodKlines(day_klines, period_func);
}

// 计算月 K 线
std::vector<uiKline> StockShareKline::GenerateMonthKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockShareKline::GetYearMonth, std::placeholders::_1, std::placeholders::_2);
    return StockShareKline::GeneratePeriodKlines(day_klines, period_func);
}

// 计算季度 K 线
std::vector<uiKline> StockShareKline::GenerateQuarterKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockShareKline::GetYearQuarter, std::placeholders::_1, std::placeholders::_2);
    return StockShareKline::GeneratePeriodKlines(day_klines, period_func);
}

// 计算年 K 线
std::vector<uiKline> StockShareKline::GenerateYearKlines(const std::vector<uiKline>& day_klines) {
    auto period_func = std::bind(&StockShareKline::GetYear, std::placeholders::_1, std::placeholders::_2);
    return StockShareKline::GeneratePeriodKlines(day_klines, period_func);
}