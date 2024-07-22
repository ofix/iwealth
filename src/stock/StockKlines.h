#ifndef STOCKKLINES_H
#define STOCKKLINES_H

#include <queue>
#include <string>
#include <unordered_map>
#include "stock/Stock.h"
#include "stock/StockMemoryKline.h"
#include "util/RichResult.h"

class StockKlines {
   public:
    RichResult QueryMinuteKlines(const std::string& share_code, std::vector<minuteKline>* minute_klines);
    RichResult QueryFiveDayMinuteKlines(const std::string& share_code,
                                        std::vecotr<minuteKline>* five_day_minute_klines);
    RichResult QueryDayKlines(const std::string& share_code, std::vector<uiKline>* day_klines);
    RichResult QueryWeekKlines(const std::string& share_code, std::vector<uiKline>* week_klines);
    RichResult QueryMonthKlines(const std::string& share_code, std::vector<uiKline>* month_klines);
    RichResult QueryQuarterKlines(const std::string& share_code, std::vector<uiKline>* quarter_klines);
    RichResult QueryYearKlines(const std::string& share_code, std::vector<uiKline>* year_klines);

   protected:
    RichResult LoadLocalDayKline(const std::string file_path, std::vector<uiKline>& day_klines);
    RichResult LoadLocalMinuteKline(const std::string file_path, std::vector<uiKline>& minute_klines);
    RichResult LoadLocalFiveDayMinuteKline(const std::string file_path, std::vector<uiKline>& fiveday_klines);
    static bool IsLocalFileExist(const std::string file_path);
    static bool IsLocalFileExpired(const std::string file_path);
    std::string GetFilePathOfDayKline(const std::string& share_code);
    RichResult FetchDayKlines(const std::string& share_code, std::vector<uiKline>& day_klines);
    RichResult FetchIncrementalDayKlines(const std::string& file_path, std::vector<uiKline>& day_klines);
    RichResult FetchMinuteKlines(const std::string& share_code, std::vector<minuteKline>& minute_klines);
    RichResult FetchFiveDayMinuteKlines(const std::string& share_code, std::vector<minuteKline>& fiveday_klines);

    static std::vector<uiKline> GenerateWeekKLines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GenerateMonthKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GenerateQuarterKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GenerateYearKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GeneratePeriodKlines(const std::vector<uiKline>& day_klines,
                                                     std::function<bool(const std::string&, int&)> period_func);
    static bool GetYearWeek(const std::string& day, int& week);
    static bool GetYearMonth(const std::string& day, int& month);
    static bool GetYearQuarter(const std::string& day, int& quarter);
    static bool GetYear(const std::string& day, int& year);

   private:
    // 市场个股前复权历史K线
    StockMemoryKline<minuteKline> m_minute_klines;
    StockMemoryKline<minuteKline> m_fiveday_klines;
    StockMemoryKline<uiKline> m_day_kline_adjust;
    StockMemoryKline<uiKline> m_week_kline_adjust;
    StockMemoryKline<uiKline> m_month_kline_adjust;
    StockMemoryKline<uiKline> m_year_kline_adjust;
};

#endif