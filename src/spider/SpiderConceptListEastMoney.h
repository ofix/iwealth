#pragma once

#include <regex>
#include <string>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderConceptListEastMoney : public Spider {
   public:
    SpiderConceptListEastMoney(StockDataStorage* storage);
    SpiderConceptListEastMoney(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderConceptListEastMoney();

   protected:
    virtual void DoCrawl();
    void FetchConceptList();
    std::string GetRequestUrl();
    void ParseResponse(std::string& response);
};
