#include "spider/SpiderShareKlineProviderBaidu.h"
#include "nlohmann/json.hpp"
#include "spider/SpiderShareKlineUrl.h"
#include "util/Global.h"

using json = nlohmann::json;

SpiderShareKlineProviderBaidu::SpiderShareKlineProviderBaidu() {
}

SpiderShareKlineProviderBaidu::~SpiderShareKlineProviderBaidu() {
}

std::string SpiderShareKlineProviderBaidu::GetKlineUrl(const KlineType kline_type,     // K线类型
                                                       const std::string& share_code,  // 股票代码
                                                       const std::string& share_name,  // 股票名称
                                                       const Market /*market*/,        // 股票市场
                                                       const std::string& end_date,    // 结束日期
                                                       const int count                 // 每次请求K线数量
) {
    if (kline_type == KlineType::Minute) {
        return KLINE_URL_FINANCE_BAIDU_MINUTE(share_code);
    } else if (kline_type == KlineType::FiveDay) {
        return KLINE_URL_FINANCE_BAIDU_FIVE_DAY(share_code, share_name);
    } else {
        std::string extra = "";
        if (end_date != "") {
            extra = "&end_time=" + end_date + "&count=" + std::to_string(count);
        }
        std::string baidu_kline_type = ConvertKlineType(kline_type);
        std::string url = KLINE_URL_FINANCE_BAIDU(share_code, baidu_kline_type, extra);
        return url;
    }
    return "";
}

DataProvider SpiderShareKlineProviderBaidu::GetType() {
    return DataProvider::FinanceBaidu;
}

std::string SpiderShareKlineProviderBaidu::ConvertKlineType(const KlineType kline_type) {
    if (kline_type == KlineType::Day) {
        return "day";
    } else if (kline_type == KlineType::Week) {
        return "week";
    } else if (kline_type == KlineType::Month) {
        return "month";
    } else if (kline_type == KlineType::Quarter) {
        return "quarter";
    } else if (kline_type == KlineType::Year) {
        return "year";
    } else if (kline_type == KlineType::Minute) {
        return "minute";
    } else if (kline_type == KlineType::FiveDay) {
        return "five_day";
    }
    return "";
}
void SpiderShareKlineProviderBaidu::ParseMinuteKline(const std::string& response,
                                                     std::vector<minuteKline>& minute_klines) {
    json _response = json::parse(response);
    json days = _response["Result"]["newMarketData"]["marketData"];
    for (json::iterator it = days.begin(); it != days.end(); ++it) {
        std::string data = (*it)["p"];
        std::vector<std::string> rows = split(data, ";");
        for (size_t i = 0; i < rows.size(); i++) {
            minuteKline minute_kline;
            std::vector<std::string> fields = split(rows[i], ",");  // 时间
            minute_kline.time = std::stod(fields[1]);               // 成交时间
            minute_kline.price = std::stod(fields[2]);              // 成交价格
            minute_kline.avg_price = std::stod(fields[3]);          // 成交均价
            minute_kline.change_amount = std::stod(fields[4]);      // 涨跌额
            minute_kline.change_rate = std::stod(fields[5]);        // 涨跌幅
            minute_kline.volume = std::stod(fields[6]);             // 成交量
            minute_kline.amount = std::stod(fields[7]);             // 成交额
            minute_kline.total_volume = std::stod(fields[8]);       // 累计成交量
            minute_kline.total_amount = std::stod(fields[9]);       // 累计成交额
            minute_klines.push_back(minute_kline);                  // 可能包含5天的分时图
        }
    }
}
void SpiderShareKlineProviderBaidu::ParseDayKline(const std::string& response, std::vector<uiKline>& uiKlines) {
    json _response = json::parse(response);
    std::string data = _response["Result"]["newMarketData"]["marketData"];
    if (data != "") {
        std::vector<std::string> rows = split(data, ";");
        for (size_t i = 0; i < rows.size(); i++) {
            uiKline ui_kline;
            std::vector<std::string> fields = split(rows[i], ",");
            ui_kline.day = fields[1];                                                  // 时间
            ui_kline.price_open = std::stod(fields[2]);                                // 开盘价
            ui_kline.price_close = std::stod(fields[3]);                               // 收盘价
            ui_kline.volume = IsNaN(fields[4]) ? 0 : std::stoull(fields[4]) / 100;     // 成交量
            ui_kline.price_max = IsNaN(fields[5]) ? 0.0 : std::stod(fields[5]);        // 最高价
            ui_kline.price_min = IsNaN(fields[6]) ? 0.0 : std::stod(fields[6]);        // 最低价
            ui_kline.amount = IsNaN(fields[7]) ? 0.0 : std::stod(fields[7]);           // 成交额
            ui_kline.change_amount = IsNaN(fields[8]) ? 0.0 : std::stod(fields[8]);    // 涨跌额
            ui_kline.change_rate = IsNaN(fields[9]) ? 0.0 : std::stod(fields[9]);      // 涨跌幅
            ui_kline.turnover_rate = IsNaN(fields[10]) ? 0.0 : std::stod(fields[10]);  // 换手率
            uiKlines.push_back(ui_kline);
        }
    }
}