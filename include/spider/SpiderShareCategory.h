#pragma once

#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

using json = nlohmann::json;
class StockDataStorage;
class SpiderShareCategory : public Spider {
   public:
    SpiderShareCategory(StockDataStorage* storage);
    SpiderShareCategory(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareCategory();
    void Crawl();

   protected:
    virtual void DoCrawl();
    void ParseCategories(std::string& data);

   private:
};