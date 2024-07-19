#pragma once
#include <chrono>
#include <string>
#include "net/Conn.h"
#include "net/Request.h"
#include "net/RequestStatistics.h"
#include "stock/Stock.h"

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
    void Crawl();      // 异步爬取
    bool CrawlSync();  // 同步爬取
    void Stop();
    void Pause();
    void SetCrawlRange(size_t start_pos, size_t end_pos);
    bool HasFinish();
    double GetProgress();
    bool IsConcurrentMode() const;
    void StartDetachThread(std::vector<CrawlRequest>& requests,
                           std::string thread_name = "",
                           int concurrent_size = 3,
                           int http_version = CURL_HTTP_VERSION_1_1);  // 启动线程
    bool Start(std::vector<CrawlRequest>& requests,
               std::string thread_name,
               int concurrent_size = 3,
               int http_version = CURL_HTTP_VERSION_1_1);
    std::string GetTimeConsumed() const;
    static std::string UrlEncode(const std::string& decoded);
    static std::string UrlDecode(const std::string& encoded);
    static std::string GetProviderName(DataProvider provider);
    void UpdateRequestStatistics();
    RequestStatistics* NewRequestStatistics(uint32_t request_count);

   protected:
    virtual void DoCrawl();
    virtual void ConcurrentResponseCallback(conn_t* conn);

   protected:
    StockDataStorage* m_pStockStorage;                           // 股票集中化数据中心
    size_t m_posStart;                                           // 股票爬取的起始下标
    size_t m_posEnd;                                             // 股票爬取的结束下标
    bool m_concurrentMode;                                       // 是否是并发请求模式
    bool m_synchronize;                                          // 是否是同步爬虫
    bool m_statistics;                                           // 是否统计
    bool m_debug;                                                // 是否打印爬虫调试信息
    std::chrono::high_resolution_clock::time_point m_timeStart;  // 爬取起始时间
    std::chrono::high_resolution_clock::time_point m_timeEnd;    // 爬取结束时间
    std::chrono::milliseconds m_timeConsume;                     // 爬虫消耗的时间
    std::vector<RequestStatistics*> m_statisticsList;            // 爬取统计
};
