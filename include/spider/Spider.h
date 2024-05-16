#pragma once
#include <chrono>
#include <string>
#include "net/Request.h"

class StockDataStorage;
class Spider {
   public:
    Spider(StockDataStorage* storage);
    std::string Fetch(const std::string& url);
    virtual ~Spider();
    void Crawl();
    void Stop();
    void Pause();
    bool HasFinish();
    double GetProgress();
    bool IsConcurrentMode() const;
    std::chrono::milliseconds GetTimeConsumed() const;

   protected:
    virtual void DoCrawl();

   protected:
    StockDataStorage* m_pStockStorage;  // 股票集中化数据中心
    size_t m_posStart;                  // 股票爬取的起始下标
    size_t m_posEnd;                    // 股票爬取的结束下标
    bool m_concurrentMode;              // 是否是并发请求模式
    std::chrono::high_resolution_clock::time_point m_timeStart;  // 爬取起始时间
    std::chrono::high_resolution_clock::time_point m_timeEnd;    // 爬取结束时间
    std::chrono::milliseconds m_timeConsume;                     // 爬虫消耗的时间
    double m_progress;                                           // 爬取的进度
};
