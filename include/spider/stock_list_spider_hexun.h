#pragma once
#include <map>
#include <regex>
#include <vector>
#include "json.hpp"
#include "spider.h"

class StockListSpiderHexun : public Spider {
   public:
    StockListSpiderHexun(const std::string& save_path);
    virtual ~StockListSpiderHexun();
    void Run();
    void FetchStockListData(int market);
    void ParseStockListData(std::string& data, std::string market);
    std::vector<StockItem> GetAllStockItems();
    void PrintAllStockItems(std::vector<StockItem>& all_stock_items);

   private:
    std::string m_stock_list_file;
    std::map<std::string, std::vector<StockItem>> m_stock_list;  // 市场所有股票
    int m_total;                                      // 市场所有股票之和
    std::map<std::string, int> m_market_stock_count;  // 分市场股票数量统计
};