#ifndef SPIDER_SHARE_KLINE_BAIDU_H
#define SPIDER_SHARE_KLINE_BAIDU_H

#include "spider/SpiderShareKlineProvider.h"

class SpiderShareKlineProviderBaidu : public SpiderShareKlineProvider {
   public:
    SpiderShareKlineProviderBaidu();
    virtual ~SpiderShareKlineProviderBaidu();
    virtual std::string GetKlineUrl(const KlineType kline_type,        // K线类型
                                    const std::string& share_code,     // 股票代码
                                    const std::string& share_name,     // 股票名称
                                    const Market market,               // 股票市场
                                    const std::string& end_date = "",  // 结束日期
                                    const int count = 3000             // 每次请求K线数量
    );
    virtual DataProvider GetType();
    virtual void ParseMinuteKline(const std::string& response, std::vector<minuteKline>& minute_klines);
    virtual void ParseDayKline(const std::string& response, std::vector<uiKline>& uiKlines);
    std::string ConvertKlineType(const KlineType kline_type);
};

#endif