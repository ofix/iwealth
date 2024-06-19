#pragma once
#include <chrono>
#include <string>
#include "net/Request.h"
#include "net/RequestStatistics.h"
#include "stock/Stock.h"

struct CrawlExtra {
    RequestStatistics* statistics;  // 统计信息
};

struct KlineCrawlExtra : CrawlExtra {
    DataProvider provider;  // K线爬取网站标识
    KlineType type;         // K线类型，日/周/月/季度/年K线
    Market market;          // 深交所/北交所/上交所
    Share* share;           // StockDataStorage::m_market_shares 元素，
                            // 下载完数据不能释放此指针指向的对象
};

struct HexunCrawlExtra : CrawlExtra {
    Market market;  // 深交所/北交所/上交所
};

struct CategoryCrawlExtra : CrawlExtra {
    ShareCategoryType category_type;  // 板块类型(概念/行业/区域)
    std::string category_name;        // 板块名称
};

struct KlineCrawlTask {
    DataProvider provider;
    double priority;
};

class StockDataStorage;
class Spider {
   public:
    Spider(StockDataStorage* storage);
    Spider(StockDataStorage* storage, bool concurrent);
    std::string Fetch(const std::string& url, int http_version = CURL_HTTP_VERSION_1_1);
    virtual ~Spider();
    void Crawl();
    void Stop();
    void Pause();
    void SetCrawlRange(size_t start_pos, size_t end_pos);
    bool HasFinish();
    double GetProgress();
    bool IsConcurrentMode() const;
    std::string GetTimeConsumed() const;
    static std::string UrlEncode(const std::string& decoded);
    static std::string UrlDecode(const std::string& encoded);
    void UpdateRequestStatistics();
    RequestStatistics* NewRequestStatistics(uint32_t request_count, DataProvider provider = DataProvider::EastMoney);

   protected:
    virtual void DoCrawl();
    virtual void ConcurrentResponseCallback(conn_t* conn);

   protected:
    StockDataStorage* m_pStockStorage;                           // 股票集中化数据中心
    size_t m_posStart;                                           // 股票爬取的起始下标
    size_t m_posEnd;                                             // 股票爬取的结束下标
    bool m_concurrentMode;                                       // 是否是并发请求模式
    bool m_debug;                                                // 是否打印爬虫调试信息
    std::chrono::high_resolution_clock::time_point m_timeStart;  // 爬取起始时间
    std::chrono::high_resolution_clock::time_point m_timeEnd;    // 爬取结束时间
    std::chrono::milliseconds m_timeConsume;                     // 爬虫消耗的时间
    std::vector<RequestStatistics*> m_statisticsList;            // 爬取统计
};
