#ifndef StockDataStorage_H
#define StockDataStorage_H
#include <iostream>
#include <map>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "stock/ShareCategory.h"
#include "stock/Stock.h"

const int ID_QUOTE_DATA_READY = 100000;

class SpiderShareListHexun;
class SpiderBasicInfoEastMoney;
class SpiderConceptListEastMoney;
class SpiderShareKline;
class SpiderShareCategory;
class StockDataStorage {
   public:
    enum class FetchResult {
        QuoteData = 0,
        Klines,
        FinancialData,
        BusinessAnalysis,
        OldNames,
    };
    StockDataStorage();
    virtual ~StockDataStorage();
    void Init();                          // 初始化操作
    uint16_t GetStockMarketShareCount();  // 获取市场股票数量
    uint16_t GetStockMarketShareCountByLocation(const std::string&& location);
    std::vector<Share> GetMarketAllShares();
    std::string GetStockDataSaveDir();
    bool SaveShareKLines(const KlineType kline_type);
    Share* FindShare(std::string& share_code);

    void PrintAllShares(std::vector<Share>& all_shares);
    inline bool IsQuoteDataReady() const {
        return m_fetch_quote_data_ok;
    };
    inline bool IsShareKlinesReady() const {
        return m_fetch_klines_ok;
    };
    inline bool IsShareFinancialDataReady() const {
        return m_fetch_financial_data_ok;
    };
    inline bool IsShareBusinessAnalysisReady() const {
        return m_fetch_business_analysis_ok;
    };
    inline bool IsShareOldNamesReady() const {
        return m_fetch_old_name_ok;
    };

    void SetFetchResultOk(FetchResult result);

   protected:
    void AsyncFetchShareQuoteData();         // 爬取行情数据
    void AsyncFetchShareKlines();            // 爬取股票历史K线数据
    void AsyncFetchShareFinancialData();     // 爬取股票年报数据
    void AsyncFetchShareBusinessAnalysis();  // 爬取股票经营分析内容
    void AsyncFetchShareOldNames();          // 爬取股票曾用名

    void OnTimeout(uint32_t timer_id, void* args);
    void OnTimerFetchShareQuoteData(uint32_t timer_id, void* args);

    std::string DumpQuoteData(std::vector<Share>& shares);
    void LoadStockAllShares();
    bool LoadLocalQuoteData(std::string& path, std::vector<Share>& shares);
    void HashShares();  // code->share 映射

   protected:
    bool SaveShareKlines(const std::string& dir_path,
                         const std::unordered_map<std::string, std::vector<uiKline>>& klines);
    // 数据存储
    std::string m_data_dir;                // 数据保存根目录
    std::string m_path_share_quote;        // 股票简称保存
    uint16_t m_market_share_total;         // 市场所有股票之和
    std::string m_path_all_market_shares;  // 所有股票代号本地保存路径
    // 行情数据是否OK
    bool m_inited;                      // 防止重复初始化
    bool m_fetch_quote_data_ok;         // 行情数据是否爬取完成
    bool m_fetch_klines_ok;             // 股票历史K线是否爬取完成
    bool m_fetch_financial_data_ok;     // 股票年报数据是否爬取完成
    bool m_fetch_business_analysis_ok;  // 股票经营分析数据是否爬取完成
    bool m_fetch_old_name_ok;           // 股票历史名称是否爬取完成

    // 内存数据
    std::vector<Share> m_market_shares;  // 所有股票
    // 股票代号->股票指针
    std::unordered_map<std::string, Share*> m_code_share_hash;
    // 统计信息
    std::unordered_map<Market, int> m_market_share_count;  // 分市场股票数量统计
    // 概念->[股票1,股票2] hash映射表
    ShareCategory m_category_concepts;
    // 行业->[股票1,股票2] hash映射表
    ShareCategory m_category_industries;
    // 省份->[股票1,股票2] hash映射表
    ShareCategory m_category_regions;
    // 市场个股前复权历史K线
    std::unordered_map<std::string, std::vector<uiKline>> m_day_klines_adjust;
    std::unordered_map<std::string, std::vector<uiKline>> m_week_klines_adjust;
    std::unordered_map<std::string, std::vector<uiKline>> m_month_klines_adjust;
    std::unordered_map<std::string, std::vector<uiKline>> m_year_klines_adjust;
    // 市场个股不复权历史K线
    std::unordered_map<std::string, std::vector<uiKline>> m_day_klines;
    std::unordered_map<std::string, std::vector<uiKline>> m_week_klines;
    std::unordered_map<std::string, std::vector<uiKline>> m_month_klines;
    std::unordered_map<std::string, std::vector<uiKline>> m_year_klines;

    // 爬虫友元类，减少数据拷贝
    friend class SpiderShareListHexun;        // 和讯网股票爬虫
    friend class SpiderBasicInfoEastMoney;    // 东方财富股票爬虫
    friend class SpiderShareHistory;          // 网易股票爬虫
    friend class SpiderConceptListEastMoney;  // 东方财富题材概念列表爬虫
    friend class SpiderShareKline;            // 百度财经股票历史K线数据爬虫
    friend class SpiderShareCategory;         // 地域板块/概念板块/行业版块数据爬虫
};

#endif  // StockDataStorage_H
