#ifndef SPIDER_SHARE_KLINE_PROVIDER_H
#define SPIDER_SHARE_KLINE_PROVIDER_H

#include <string>
#include <vector>
#include "net/Conn.h"
#include "stock/Stock.h"

class SpiderShareKlineProvider {
   public:
    SpiderShareKlineProvider();
    virtual ~SpiderShareKlineProvider();
    virtual std::string GetKlineUrl(const KlineType kline_type,       // K线类型
                                    const std::string& share_code,    // 股票代码
                                    const std::string& share_name,    // 股票名称
                                    const Market market,              // 股票市场
                                    const std::string& end_date = ""  // 结束日期
                                    ) = 0;
    virtual DataProvider GetType() = 0;
    virtual void ParseMinuteKline(conn_t* conn, std::vector<std::vector<minuteKline>>& minute_klines) = 0;
    virtual void ParseDayKline(conn_t* conn, std::vector<uiKline>& uiKlines) = 0;
    static bool IsNaN(const std::string& data);
};

#endif