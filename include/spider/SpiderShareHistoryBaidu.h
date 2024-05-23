#pragma once

#include <regex>
#include <string>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderShareHistoryBaidu : public Spider {
   public:
    SpiderShareHistoryBaidu(StockDataStorage* storage);
    SpiderShareHistoryBaidu(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareHistoryBaidu();

   protected:
    virtual void DoCrawl();
    std::string GetRequestUrl(const Share& share, const std::string& end_date = "");
    std::vector<uiKline> ParseResponse(std::string& response);
    void FetchShareHistoryKlineAdjust(const Share& share);
};
