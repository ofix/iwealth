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
    SpiderShareCategory(StockDataStorage* storage, int types = ShareCategoryType::Province, bool concurrent = true);
    virtual ~SpiderShareCategory();
    void BuildShareCategoryProvinces();
    void BuildShareCategoryIndustries();
    void BuildShareCategoryConcepts();
    std::unordered_map<std::string, std::vector<std::string>>* GetCategory(ShareCategoryType type);

   protected:
    virtual void DoCrawl();
    void FetchCategoryShares(nlohmann::json& categories, ShareCategoryType type);
    void ParseCategories(std::string& data);
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
    int m_types;  // 爬取的板块种类
};