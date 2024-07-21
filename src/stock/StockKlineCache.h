#ifndef STOCK_KLINE_CACHE_H
#define STOCK_KLINE_CACHE_H

#include <queue>
#include <string>
#include <unordered_map>
#include "stock/Stock.h"
#include "stock/StockMemoryKline.h"
#include "util/RichResult.h"

class StockKlineCache {
   public:
    StockKlineCache();
    ~StockKlineCache();
    RichResult LoadLocalDayKline(const std::string file_path, std::vector<uiKline>& day_klines);
    RichResult LoadLocalMinuteKline(const std::string file_path, std::vector<uiKline>& minute_klines);
    RichResult LoadLocalFiveDayMinuteKline(const std::string file_path, std::vector<uiKline>& fiveday_klines);
    static bool IsLocalFileExist(const std::string file_path);
    static bool IsLocalFileExpired(const std::string file_path);
    std::string GetFilePathOfDayKline(const std::string& share_code);
    RichResult FetchDayKline(const std::string& share_code, std::vector<uiKline>& day_klines);
    RichResult FetchMinuteKline(const std::string& share_code, std::vector<minuteKline>& minute_klines);
    RichResult FetchFiveDayMinuteKline(const std::string& share_code, std::vector<minuteKline>& fiveday_klines);

    // 市场个股前复权历史K线
    StockMemoryKline<minuteKline> m_minute_klines;
    StockMemoryKline<minuteKline> m_fiveday_klines;
    StockMemoryKline<uiKline> m_day_kline_adjust;
    StockMemoryKline<uiKline> m_week_kline_adjust;
    StockMemoryKline<uiKline> m_month_kline_adjust;
    StockMemoryKline<uiKline> m_year_kline_adjust;
};

#endif