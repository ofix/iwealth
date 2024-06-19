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
    void Crawl(int types = ShareCategoryType::Industry | ShareCategoryType::Region | ShareCategoryType::Concept);

   protected:
    virtual void DoCrawl(int types);
    void ParseCategories(std::string& data, int types);
    void FetchCategoryShares(nlohmann::json& categories, ShareCategoryType type);
    std::string GetCategoryKey(std::string name);
    void ConcurrentResponseCallback(conn_t* conn);
    std::string GetCategoryTypeName(ShareCategoryType type);

   private:
};