#ifndef SPIDER_SHARE_KLINE_EAST_MONEY_H
#define SPIDER_SHARE_KLINE_EAST_MONEY_H

#include "spider/SpiderShareKlineProvider.h"

class SpiderShareKlineProviderEastMoney : public SpiderShareKlineProvider {
   public:
    SpiderShareKlineProviderEastMoney();
    virtual ~SpiderShareKlineProviderEastMoney();
    virtual std::string GetKlineUrl(const KlineType kline_type,       // K线类型
                                    const std::string& share_code,    // 股票代码
                                    const std::string& share_name,    // 股票名称
                                    const Market market,              // 股票市场
                                    const std::string& end_date = ""  // 结束日期
    );
    virtual DataProvider GetType();
    int GetMarketCode(const Market market);
    int ConvertKlineType(const KlineType kline_type);
    virtual void ParseMinuteKline(conn_t* conn, std::vector<std::vector<minuteKline>>& minute_klines);
    virtual void ParseDayKline(conn_t* conn, std::vector<uiKline>& uiKlines);
};

#endif