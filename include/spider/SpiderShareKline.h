#pragma once

#include <regex>
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderShareKline : public Spider {
   public:
    SpiderShareKline(StockDataStorage* storage);
    SpiderShareKline(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareKline();

   protected:
    virtual void DoCrawl();
    // 百度财经
    std::string GetDayKlineUrlFinanceBaidu(const Share& share, const std::string& end_date = "");
    std::string GetDayKlineUrlFinanceBaidu(const std::string& share_code, const std::string& end_date);
    std::string GetWeekKlineUrlFinanceBaidu(const Share& share, const std::string& end_date = "");
    std::string GetWeekKlineUrlFinanceBaidu(const std::string& share_code, const std::string& end_date);
    std::string GetMonthKlineUrlFinanceBaidu(const Share& share, const std::string& end_date = "");
    std::string GetMonthKlineUrlFinanceBaidu(const std::string& share_code, const std::string& end_date);
    std::string GetYearKlineUrlFinanceBaidu(const Share& share, const std::string& end_date = "");
    std::string GetYearKlineUrlFinanceBaidu(const std::string& share_code, const std::string& end_date);
    // 东方财富
    std::string GetDayKlineUrlEastMoney(const Share& share, const std::string& end_date = "");
    std::string GetDayKlineUrlEastMoney(const std::string& share_code, const std::string& end_date);
    std::string GetWeekKlineUrlEastMoney(const Share& share, const std::string& end_date = "");
    std::string GetWeekKlineUrlEastMoney(const std::string& share_code, const std::string& end_date);
    std::string GetMonthKlineUrlEastMoney(const Share& share, const std::string& end_date = "");
    std::string GetMonthKlineUrlEastMoney(const std::string& share_code, const std::string& end_date);
    std::string GetYearKlineUrlEastMoney(const Share& share, const std::string& end_date = "");
    std::string GetYearKlineUrlEastMoney(const std::string& share_code, const std::string& end_date);

    std::vector<uiKline> ParseResponse(std::string& response);
    void FetchShareAdjustDayKline(const Share& share);
    void ConcurrentFetchShareAdjustDayKline(size_t start_pos, size_t end_pos);
    void ConcurrentResponseCallback(conn_t* conn);
    std::string GetShareCodeFromUrl(const std::string& url);
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>> m_concurrent_klines;
};
