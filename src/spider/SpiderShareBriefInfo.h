#pragma once

#include <regex>
#include <string>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderShareBriefInfo : public Spider {
   public:
    SpiderShareBriefInfo(StockDataStorage* storage);
    SpiderShareBriefInfo(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareBriefInfo();
    uint8_t GetIndustryLevel(const std::string& industry_name);

   protected:
    virtual void DoCrawl();
    virtual void ConcurrentResponseCallback(conn_t* conn);
    void FetchBriefInfo(Share& share);
    void ConcurrentFetchBriefInfo();
    void OnTimerFetchBriefInfo(uint32_t timer_id, void* args);
    std::string GetRequestUrl(const Share& share);
    void ParseResponse(std::string& response, Share* pShare);
};
