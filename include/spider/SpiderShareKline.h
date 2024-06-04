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
    void OnRequestTimer(uint32_t timer_id, void* args);
    static bool IsNaN(const std::string& data);
    static bool ParseKlineBaidu(const std::string& kline, uiKline* uiKline);
    static bool ParseKlineEastMoney(const std::string& kline, uiKline* uiKline);
    void MergeShareKlines(const KlineType kline_type = KlineType::Day);
    size_t GetKlineCount(const std::vector<std::vector<uiKline>>& multi_klines);
    std::string GetProviderName(KlineProvider provider) const;

   protected:
    virtual void DoCrawl(KlineType type = KlineType::Day);
    void MergeShareKlines(std::unordered_map<std::string, std::vector<std::vector<uiKline>>>& concurrent_klines,
                          std::unordered_map<std::string, std::vector<uiKline>>& target_klines);
    void MergeShareKlines(std::unordered_map<std::string, std::vector<uiKline>>& concurrent_klines,
                          std::unordered_map<std::string, std::vector<uiKline>>& target_klines);
    void SingleCrawl(std::vector<KlineCrawlTask>& tasks, KlineType kline_type);
    void ConurrentCrawl(std::vector<KlineCrawlTask>& tasks, KlineType kline_type);
    // 百度财经
    std::string GetKlineTypeFinanceBaidu(const KlineType kline_type);
    std::string GetKlineUrl(const KlineProvider provider,     // 供应商
                            const KlineType kline_type,       // K线类型
                            const std::string& share_code,    // 股票代码
                            const Market market,              // 股票市场
                            const std::string& end_date = ""  // 结束日期
    );
    // 东方财富
    int GetKlineTypeEastMoney(const KlineType kline_type);
    int GetEastMoneyMarketCode(const Market market);

    // 解析响应
    std::vector<uiKline> ParseResponse(conn_t* conn);
    void ParseResponseFinanceBaidu(conn_t* conn, std::vector<uiKline>& uiKlines);
    void ParseResponseEastMoney(conn_t* conn, std::vector<uiKline>& uiKlines);

    void SingleResponseCallback(conn_t* conn);
    void ConcurrentResponseCallback(conn_t* conn);

   private:
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>> m_concurrent_day_klines_adjust;
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>> m_concurrent_week_klines_adjust;
    std::unordered_map<std::string, std::vector<uiKline>> m_concurrent_month_klines_adjust;
    std::unordered_map<std::string, std::vector<uiKline>> m_concurrent_year_klines_adjust;
};
