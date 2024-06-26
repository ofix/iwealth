#ifndef StockDataStorage_H
#define StockDataStorage_H
#include <iostream>
#include <map>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "search/Trie.h"
#include "stock/ShareCategory.h"
#include "stock/Stock.h"

const int ID_QUOTE_DATA_READY = 100000;

class SpiderShareQuote;
class SpiderShareBriefInfo;
class SpiderConceptListEastMoney;
class SpiderShareKline;
class SpiderShareCategory;
class Spider;
class StockDataStorage {
   public:
    enum class FetchResult {
        QuoteData = 0,
        Klines,
        FinancialData,
        BusinessAnalysis,
        OldNames,
    };
    enum class SpiderType {
        Quote,             // 行情数据爬虫
        Industry,          // 行业板块爬虫
        Province,          // 地区板块爬虫
        Concept,           // 概念板块爬虫
        IndustryProvince,  // 行业|地区板块联合爬虫
        BriefInfo,         // 股票基本信息(曾用名)爬虫
    };
    StockDataStorage();
    virtual ~StockDataStorage();
    void Init();                          // 初始化操作
    uint16_t GetStockMarketShareCount();  // 获取市场股票数量
    uint16_t GetStockMarketShareCountByLocation(const std::string&& location);
    std::vector<Share> GetMarketAllShares();
    std::string GetStockDataSaveDir();
    void InsertShareNameToTrie(const std::string& share_name, const std::string& share_code);  // 插入股票
    std::vector<Share*> SearchShares(const std::string& prefix);
    bool SaveShareNames();  // 保存股票曾用名和名称到文件
    bool SaveShareBriefInfo(ShareBriefInfo* pBriefInfo, const std::string& share_code);
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
    bool IsLocalQuoteDataExpired();
    Spider* GetSpider(SpiderType type);

   protected:
    void FetchQuoteIndustryProvince();  // 爬取行情数据+板块数据
    void FetchQuoteSync();              // 爬取行情数据
    void FetchKline();                  // 爬取股票历史K线数据
    void FetchFinancial();              // 爬取股票年报数据
    void FetchBusinessAnalysis();       // 爬取股票经营分析内容
    void FetchBriefInfo();              // 爬取股票[曾用名/员工数等基本信息]

    std::string DumpQuoteData(std::vector<Share>& shares);
    void SaveQuote();  // 保存行情数据到本地文件
    void SaveCategory(ShareCategoryType type,
                      std::unordered_map<std::string, std::vector<std::string>>* categories);  // 保存板块信息到本地文件

    bool LoadLocalFileQuote(std::string& path, std::vector<Share>& shares);  // 加载本地行情数据文件
    void LoadLocalFileCategory(ShareCategoryType type,
                               const std::string& file_path,
                               ShareCategory& share_categories);  // 加载本地板块->股票映射文件
    void LoadLocalFileShare();       // 加载本地 日K线/周K线/月K线/季度K线/年K线文件
    void LoadLocalFileShareNames();  // 加载本地股票曾用名和名称

    void OnTimeout(uint32_t timer_id, void* args);
    void OnTimerFetchShareQuoteData(uint32_t timer_id, void* args);

    void HashShares();  // code->Share* 映射

   protected:
    bool SaveShareKlines(const std::string& dir_path,
                         const std::unordered_map<std::string, std::vector<uiKline>>& klines);
    // 数据存储
    std::string m_data_dir;                // 数据保存根目录
    std::string m_path_share_quote;        // 股票行情数据文件路径
    std::string m_path_category_province;  // 股票省份数据文件路径
    std::string m_path_category_industry;  // 股票行业数据文件路径
    std::string m_path_category_concept;   // 股票概念数据文件路径
    std::string m_path_share_names;        // 股票曾用名数据文件路径
    std::string m_path_brief_dir;          // 股票简要信息文件根目录

    uint16_t m_market_share_total;         // 市场所有股票之和
    std::string m_path_all_market_shares;  // 所有股票代号本地保存路径
    Trie m_trie;                           // 股票[简称/曾用名]->关系映射

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
    // 概念->[股票1,股票2] hash映射表
    ShareCategory m_category_concepts;
    // 行业->[股票1,股票2] hash映射表
    ShareCategory m_category_industries;
    // 省份->[股票1,股票2] hash映射表
    ShareCategory m_category_provinces;
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

    // 统计信息
    std::unordered_map<Market, int> m_market_share_count;  // 分市场股票数量统计
    // 爬虫友元类，减少数据拷贝
    friend class SpiderShareQuote;            // 和讯网股票爬虫
    friend class SpiderShareBriefInfo;        // 东方财富股票爬虫
    friend class SpiderShareHistory;          // 网易股票爬虫
    friend class SpiderConceptListEastMoney;  // 东方财富题材概念列表爬虫
    friend class SpiderShareKline;            // 百度财经股票历史K线数据爬虫
    friend class SpiderShareCategory;         // 地域板块/概念板块/行业版块数据爬虫
};

#endif  // StockDataStorage_H
