#pragma once

#include <regex>
#include <string>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class ShareListSpiderHexun : public Spider {
   public:
    ShareListSpiderHexun(StockDataStorage* storage);
    virtual ~ShareListSpiderHexun();
    void Run();

   protected:
    void FetchMarketShares(int market);
    void ParseStockListData(std::string& data, Market market);
};
