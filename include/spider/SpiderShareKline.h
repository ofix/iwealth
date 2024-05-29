#pragma once

#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "spider/Spider.h"

// 百度股市通 https://gushitong.baidu.com/stock/ab-300059
#define KLINE_URL_FINANCE_BAIDU(share_code, kline_type, extra) \
    "https://finance.pae.baidu.com/vapi/v1/getquotation"       \
    "?srcid=5353"                                              \
    "&pointType=string"                                        \
    "&group=quotation_kline_ab"                                \
    "&query=" +                                                \
        share_code + "&code=" + share_code +                   \
        "&market_type=ab"                                      \
        "&newFormat=1"                                         \
        "&is_kc=0"                                             \
        "&ktype=" +                                            \
        kline_type + "&finClientType=pc" + extra + "&finClientType=pc"

// 东方财富行情中心 https://quote.eastmoney.com/concept/sz300729.html
#define KLINE_URL_EAST_MONEY(share_code, market, kline_type) \
    "https://push2his.eastmoney.com/api/qt/stock/kline/get"  \
    "?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13"    \
    "&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61"   \
    "&begin=0"                                               \
    "&end=20500101"                                          \
    "&rtntype=6"                                             \
    "&lmt=1000000"                                           \
    "&secid=" +                                              \
        std::to_string(market) + "." + share_code + "&klt=" + std::to_string(kline_type) + "&fqt=1"

class StockDataStorage;
class SpiderShareKline : public Spider {
   public:
    SpiderShareKline(StockDataStorage* storage);
    SpiderShareKline(StockDataStorage* storage, bool concurrent);
    virtual ~SpiderShareKline();
    void Crawl(KlineType type = KlineType::Day);

   protected:
    virtual void DoCrawl(KlineType type = KlineType::Day);
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
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>> m_concurrent_klines;
};
