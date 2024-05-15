#pragma once

#include <regex>
#include <string>
#include "nlohmann/json.hpp"
#include "spider/spider.h"
#include "stock/stock.h"

class ShareDataCenter;
class ShareListSpiderHexun : public Spider {
   public:
    ShareListSpiderHexun(ShareDataCenter* data_center);
    virtual ~ShareListSpiderHexun();
    void Run();

   protected:
    void FetchMarketShares(int market);
    void ParseStockListData(std::string& data, Market market);
};
