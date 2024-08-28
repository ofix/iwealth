#ifndef StockDataStorage_H
#define StockDataStorage_H
#include <iostream>
#include <map>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "search/Trie.h"
#include "stock/ShareCategory.h"
#include "stock/Stock.h"
#include "util/RichResult.h"

const int ID_QUOTE_DATA_READY = 100000;
const int ID_ASYNC_PROCESS_ERROR = 100001;

class SpiderShareQuote;
class SpiderShareBriefInfo;
class SpiderConceptListEastMoney;
class SpiderShareKline;
class SpiderShareCategory;
class SpiderCompositeIndex;
class Spider;
class StockShareKline;
class StockDataStorage {
   public:
    enum class AsyncFetchType {
        QuoteData = 0,
        Klines,
        FinancialData,
        BusinessAnalysis,
        OldNames,
    };
    struct AsyncFetchResult {
        AsyncFetchType type;
        RichResult state;
    };
    enum class SpiderType {
        Quote,             // 行情数据爬虫
        Industry,          // 行业板块爬虫
        Province,          // 地区板块爬虫
        Concept,           // 概念板块爬虫
        IndustryProvince,  // 行业|地区板块联合爬虫
        BriefInfo,         // 股票基本信息(曾用名)爬虫
        Kline,             // 分时/五日分时/日K线爬虫
    };
    enum DumpType {
        Quote = 0,  // 打印行情信息
    };
    StockDataStorage(bool gui_mode = false);
    virtual ~StockDataStorage();
    RichResult Init();  // 初始化操作
    void SetEventHandler(wxEvtHandler* event_handler);
    size_t GetStockMarketShareCount();  // 获取市场股票数量
    size_t GetStockMarketShareCountByLocation(const std::string&& location);
    std::vector<Share>* GetStockAllShares();
    std::string GetStockDataSaveDir();

    // 保存股票曾用名和名称到文件
    bool SaveShareNames();
    // 保存股票基本信息
    bool SaveShareBriefInfo(ShareBriefInfo* pBriefInfo, const std::string& share_code);

    // 插入股票到Trie树
    void InsertShareNameToTrie(const std::string& share_name, const std::string& share_code);
    // 根据股票代号精确查找股票
    Share* FindShare(const std::string& share_code);
    // 获取当前显示的股票上一个股票
    Share* FindPrevShare(Share* pCurrentShare);
    // 获取当前显示的股票下一个股票
    Share* FindNextShare(Share* pCurrentShare);
    // 模糊搜索股票
    std::vector<Share*> SearchShares(const std::string& prefix);
    // 清空股票
    bool ClearShares();
    // 删除股票
    bool DeleteShares(size_t pos, size_t numRows);
    // 打印所有股票
    void PrintAllShares(std::vector<Share>& all_shares);

    // 异步抓取数据相关
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

    Spider* GetSpider(SpiderType type);
    RichResult QueryKlines(const std::string& share_code, KlineType kline_type, std::vector<uiKline>** klines);
    RichResult QueryMinuteKlines(const std::string& share_code,
                                 KlineType kline_type,
                                 std::vector<minuteKline>** klines);

   protected:
    RichResult LoadShareQuote();  // 初始化加载股票行情数据
    RichResult LoadLocalShareQuoteFile(std::string& path, std::vector<Share>& shares);  // 加载本地行情数据文件
    RichResult LoadLocalShareCategoryFile(ShareCategoryType type,
                                          const std::string& file_path,
                                          ShareCategory& share_categories);  // 加载本地板块->股票映射文件

    RichResult LoadLocalShareNamesFile();  // 加载本地股票曾用名和名称

    void FetchQuoteIndustryProvince();  // 爬取行情数据+板块数据
    RichResult FetchShareQuoteSync();   // 爬取行情数据
    void FetchShareBriefInfo();         // 爬取股票[曾用名/员工数等基本信息]
    void OnTimeout(uint32_t timer_id, void* args);
    void OnTimerFetchShareQuoteData(uint32_t timer_id, void* args);
    void SetAsyncFetchResult(AsyncFetchResult result);

    bool SaveShareQuoteToFile();  // 保存行情数据到本地文件
    bool SaveShareCategoryToFile(
        ShareCategoryType type,
        std::unordered_map<std::string, std::vector<std::string>>* categories);  // 保存板块信息到本地文件
    void HashShares();                                                           // code->Share* 映射
    void DumpStorage(DumpType dump_type);                                        // 打印信息
    std::string DumpQuoteData(std::vector<Share>& shares);

   protected:
    bool m_inGuiMode;                      // GUI模式下初始化
    bool m_inited;                         // 防止重复初始化
    wxEvtHandler* m_eventHandler;          // 基于事件消息和主窗口进行异步通信,
                                           // 通过依赖注入解决相互依赖的问题，达到解耦的目的
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
    StockShareKline* m_stock_klines;
    // 统计信息
    std::unordered_map<Market, int> m_market_share_count;  // 分市场股票数量统计
    // 爬虫友元类，减少数据拷贝
    friend class SpiderShareQuote;            // 和讯网股票爬虫
    friend class SpiderShareBriefInfo;        // 东方财富股票爬虫
    friend class SpiderShareHistory;          // 网易股票爬虫
    friend class SpiderConceptListEastMoney;  // 东方财富题材概念列表爬虫
    friend class SpiderShareKline;            // 百度财经股票历史K线数据爬虫
    friend class SpiderShareCategory;         // 地域板块/概念板块/行业版块数据爬虫
    friend class SpiderCompositeIndex;        // 各大股指行情和成分股信息爬虫
};

#endif  // StockDataStorage_H
