#ifndef STOCKKLINES_H
#define STOCKKLINES_H

#include "stock/Stock.h"

class StockKlines {
   public:
    static std::vector<uiKline> GetWeekKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GetMonthKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GetQuarterKlines(const std::vector<uiKline>& day_klines);
    static std::vector<uiKline> GetYearKlines(const std::vector<uiKline>& day_klines);
};

#endif