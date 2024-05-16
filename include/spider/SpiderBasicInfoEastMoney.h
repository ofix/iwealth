#pragma once

#include <regex>
#include <string>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderBasicInfoEastMoney : public Spider {
   public:
    SpiderBasicInfoEastMoney(StockDataStorage* storage);
    virtual ~SpiderBasicInfoEastMoney();
    virtual void DoCrawl();

   protected:
    void FetchBasicInfo(Share& share);
    std::string GetRequestUrl(const Share& share);
    void ConcurrentFetchBasicInfo(size_t start_pos, size_t end_pos);
    void ParseResponse(std::string& response, Share& share);
};
