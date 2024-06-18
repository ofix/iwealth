#pragma once

#include <regex>
#include <set>
#include <string>
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
    void SingleCrawl();
    void ConcurrentCrawl();
    void RemoveRepeatShares();
    void ConcurrentResponseCallback(conn_t* conn);
    std::string GetFetchUrl(int market);
    Market GetMarket(int market);
    std::set<Share> m_unique_shares;  // 和讯网行情中心，股票会有重复
};
