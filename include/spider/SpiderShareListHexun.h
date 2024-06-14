#pragma once

#include <regex>
#include <string>
#include <set>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderShareListHexun : public Spider {
   public:
    SpiderShareListHexun(StockDataStorage* storage);
    virtual ~SpiderShareListHexun();
    virtual void DoCrawl();

   protected:
    void FetchMarketShares(int market);
    void ParseStockListData(std::string& data, Market market);
    std::set<Share> m_unique_shares; // 和讯网行情中心，股票会有重复
};
