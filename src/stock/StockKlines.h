#ifndef STOCKKLINES_H
#define STOCKKLINES_H

#include "stock/Stock.h"

class StockKlines {
   public:
    static std::vector<uiKline> GetWeekKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GetMonthKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GetQuarterKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GetYearKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GetPeriodKlines(const std::vector<uiKline>& day_klines,
                                                std::function<bool(const std::string&, int&)> period_func);
    static bool GetYearWeek(const std::string& day, int& week);
    static bool GetYearMonth(const std::string& day, int& month);
    static bool GetYearQuarter(const std::string& day, int& quarter);
    static bool GetYear(const std::string& day, int& year);
};

#endif