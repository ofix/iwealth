#pragma once

#include <regex>
#include <string>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderShareBasicInfo : public Spider {
   public:
    SpiderShareBasicInfo(StockDataStorage* storage);
    SpiderShareBasicInfo(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareBasicInfo();
    uint8_t GetIndustryLevel(const std::string& industry_name);

   protected:
    virtual void DoCrawl();
    virtual void ConcurrentResponseCallback(conn_t* conn);
    void FetchBasicInfo(Share& share);
    void ConcurrentFetchBasicInfo();
    std::string GetRequestUrl(const Share& share);
    void ParseResponse(std::string& response, Share& share);
};
