#ifndef StockDataStorage_H
#define StockDataStorage_H
#include <iostream>
#include <map>
#include "nlohmann/json.hpp"
#include "stock/Stock.h"

class SpiderShareListHexun;
class SpiderBasicInfoEastMoney;
class SpiderShareHistory;
class StockDataStorage {
   public:
    StockDataStorage();
    virtual ~StockDataStorage();
    void Init();                          // 初始化操作
    uint16_t GetStockMarketShareCount();  // 获取市场股票数量
    uint16_t GetStockMarketShareCountByLocation(const std::string&& location);
    void PrintAllShares(std::vector<Share>& all_shares);
    std::vector<Share> GetMarketAllShares();
    std::string GetStockDataSaveDir();
    bool CrawlStockKlinesHistoryData();  // 同步股票历史K线数据
    bool CrawlStockHistoryName();        // 同步股票曾用名

   protected:
    std::string ToJson(std::vector<Share>& shares);
    void LoadStockAllShares();
    bool LoadLocalJsonFile(std::string& path, std::vector<Share>& shares);

   protected:
    // 数据存储
    std::string m_data_dir;                // 数据保存根目录
    std::string m_path_share_brief;        // 股票简称保存
    uint16_t m_market_share_total;         // 市场所有股票之和
    std::string m_path_all_market_shares;  // 所有股票代号本地保存路径
    // 内存数据
    std::vector<Share> m_market_shares;  // 所有股票
    // 统计信息
    std::map<Market, int> m_market_share_count;  // 分市场股票数量统计
    // 爬虫友元类，减少数据拷贝
    friend class SpiderShareListHexun;      // 和讯网股票爬虫
    friend class SpiderBasicInfoEastMoney;  // 东方财富股票爬虫
    friend class SpiderShareHistory;        // 网易股票爬虫
};

#endif  // StockDataStorage_H
