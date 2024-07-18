#ifndef SPIDER_SHARE_KLINE_BAIDU_H
#define SPIDER_SHARE_KLINE_BAIDU_H

#include "spider/SpiderShareKlineProvider.h"

class SpiderShareKlineProviderBaidu : public SpiderShareKlineProvider {
   public:
    SpiderShareKlineProviderBaidu();
    virtual ~SpiderShareKlineProviderBaidu();
    virtual std::string GetKlineUrl(const KlineType kline_type,       // K线类型
                                    const std::string& share_code,    // 股票代码
                                    const std::string& share_name,    // 股票名称
                                    const Market market,              // 股票市场
                                    const std::string& end_date = ""  // 结束日期
    );
    virtual DataProvider GetType();
    std::string ConvertKlineType(const KlineType kline_type);
    virtual void ParseMinuteKline(conn_t* conn, std::vector<std::vector<minuteKline>>& minute_klines);
    virtual void ParseDayKline(conn_t* conn, std::vector<uiKline>& uiKlines);
};

#endif