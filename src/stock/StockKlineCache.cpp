#include "stock/StockKlineCache.h"
#include "util/FileTool.h"
#include "util/Global.h"

StockKlineCache::StockKlineCache() {
}

StockKlineCache::~StockKlineCache() {
}

RichResult StockKlineCache::FetchDayKline(const std::string& share_code, std::vector<uiKline>& day_klines) {
    return Success();
}

RichResult StockKlineCache::FetchMinuteKline(const std::string& share_code, std::vector<minuteKline>& minute_klines) {
    return Success();
}

RichResult StockKlineCache::FetchFiveDayMinuteKline(const std::string& share_code,
                                                    std::vector<minuteKline>& fiveday_klines) {
    return Success();
}

RichResult StockKlineCache::LoadLocalDayKline(const std::string share_code, std::vector<uiKline>& day_klines) {
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

RichResult StockKlineCache::LoadLocalMinuteKline(const std::string file_path, std::vector<uiKline>& minute_klines) {
    return Success();
}

RichResult StockKlineCache::LoadLocalFiveDayMinuteKline(const std::string file_path,
                                                        std::vector<uiKline>& fiveday_klines) {
    return Success();
}

bool StockKlineCache::IsLocalFileExist(const std::string file_path) {
    return FileTool::IsFileExists(file_path);
}

bool StockKlineCache::IsLocalFileExpired(const std::string file_path) {
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

std::string StockKlineCache::GetFilePathOfDayKline(const std::string& share_code) {
    return FileTool::CurrentPath() + "data" + DIR_SEPARATOR + "day" + DIR_SEPARATOR + share_code + ".csv";
}
