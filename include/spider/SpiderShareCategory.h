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
    void Crawl(int types = ShareCategoryType::Industry | ShareCategoryType::Province | ShareCategoryType::Concept);
    void BuildShareCategoryProvinces();
    void BuildShareCategoryIndustries();
    void BuildShareCategoryConcepts();

   protected:
    virtual void DoCrawl(int types);
    void ParseCategories(std::string& data, int types);
    void FetchCategoryShares(nlohmann::json& categories, ShareCategoryType type);
    std::string GetCategoryKey(std::string name);
    void ConcurrentResponseCallback(conn_t* conn);
    std::string GetCategoryTypeName(ShareCategoryType type);
    void InsertCategory(std::unordered_map<std::string, std::vector<std::string>>& hash_map,
                        std::string& category_name,
                        std::string& share_code);

   private:
    std::unordered_map<std::string, std::vector<std::string>> m_industries;
    std::unordered_map<std::string, std::vector<std::string>> m_provinces;
    std::unordered_map<std::string, std::vector<std::string>> m_concepts;
};