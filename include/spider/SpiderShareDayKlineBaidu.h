#pragma once

#include <regex>
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderShareDayKlineBaidu : public Spider {
   public:
    SpiderShareDayKlineBaidu(StockDataStorage* storage);
    SpiderShareDayKlineBaidu(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareDayKlineBaidu();

   protected:
    virtual void DoCrawl();
    std::string GetRequestUrl(const std::string& share_code, const std::string& end_date);
    std::string GetRequestUrl(const Share& share, const std::string& end_date = "");
    std::vector<uiKline> ParseResponse(std::string& response);
    void FetchShareAdjustDayKline(const Share& share);
    void ConcurrentFetchShareAdjustDayKline(size_t start_pos, size_t end_pos);
    void ConcurrentResponseCallback(conn_t* conn);
    std::string GetShareCodeFromUrl(const std::string& url);
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>>
        m_concurrent_klines;
};
