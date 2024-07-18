#pragma once

#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include "SpiderShareKlineUrl.h"
#include "nlohmann/json.hpp"
#include "spider/Spider.h"
#include "stock/Stock.h"

class StockDataStorage;
class SpiderShareKline : public Spider {
   public:
    SpiderShareKline(StockDataStorage* storage);
    SpiderShareKline(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareKline();
    void Crawl(KlineType type = KlineType::Day);
    void CrawlSync(Share* pShare, KlineType kline_type);
    void OnRequestTimer(uint32_t timer_id, void* args);
    static bool IsNaN(const std::string& data);

    void MergeShareKlines(const KlineType kline_type = KlineType::Day);
    size_t GetKlineCount(const std::vector<std::vector<uiKline>>& multi_klines);
    // std::string GetProviderName(DataProvider provider) const;
    static void DumpKline(uiKline& kline);

   protected:
    virtual void DoCrawl(KlineType type = KlineType::Day);
    void MergeShareKlines(std::unordered_map<std::string, std::vector<std::vector<uiKline>>>& concurrent_klines,
                          std::unordered_map<std::string, std::vector<uiKline>>& target_klines);
    void MergeShareKlines(std::unordered_map<std::string, std::vector<uiKline>>& concurrent_klines,
                          std::unordered_map<std::string, std::vector<uiKline>>& target_klines);
    void ConurrentCrawl(std::vector<KlineCrawlTask>& tasks, KlineType kline_type);
    void ConcurrentResponseCallback(conn_t* conn);

   private:
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>> m_concurrent_day_klines_adjust;
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>> m_concurrent_week_klines_adjust;
    std::unordered_map<std::string, std::vector<uiKline>> m_concurrent_month_klines_adjust;
    std::unordered_map<std::string, std::vector<uiKline>> m_concurrent_year_klines_adjust;
    std::vector<std::vector<minuteKline>> m_minute_klines;  // 当天分时图/五日分时图
};
