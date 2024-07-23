#include "spider/SpiderShareKlineProviderEastMoney.h"
#include "nlohmann/json.hpp"
#include "spider/SpiderShareKlineUrl.h"
#include "util/Global.h"

using json = nlohmann::json;

SpiderShareKlineProviderEastMoney::SpiderShareKlineProviderEastMoney() {
}

SpiderShareKlineProviderEastMoney::~SpiderShareKlineProviderEastMoney() {
}

std::string SpiderShareKlineProviderEastMoney::GetKlineUrl(const KlineType kline_type,         // K线类型
                                                           const std::string& share_code,      // 股票代码
                                                           const std::string& /*share_name*/,  // 股票名称
                                                           const Market market,                // 股票市场
                                                           const std::string& /*end_date*/,    // 结束日期
                                                           const int /*count*/                 // 每次请求K线数量
) {
    int market_code = GetMarketCode(market);
    int east_money_kline_type = ConvertKlineType(kline_type);
    if (kline_type == KlineType::Minute) {
        return KLINE_URL_EAST_MONEY_MINUTE(share_code, market_code);
    } else if (kline_type == KlineType::FiveDay) {
        return KLINE_URL_EAST_MONEY_FIVE_DAY(share_code, market_code);
    } else {
        return KLINE_URL_EAST_MONEY(share_code, market_code, east_money_kline_type);
    }
}

DataProvider SpiderShareKlineProviderEastMoney::GetType() {
    return DataProvider::EastMoney;
}

int SpiderShareKlineProviderEastMoney::GetMarketCode(const Market market) {
    if (market == Market::ShenZhen) {
        return 0;
    } else if (market == Market::ShangHai) {
        return 1;
    } else if (market == Market::ChuangYeBan) {
        return 0;
    } else if (market == Market::KeChuangBan) {
        return 1;
    } else if (market == Market::BeiJiaoSuo) {
        return 0;
    }
    return 0;
}

int SpiderShareKlineProviderEastMoney::ConvertKlineType(const KlineType kline_type) {
    if (kline_type == KlineType::Day) {
        return 101;
    } else if (kline_type == KlineType::Week) {
        return 102;
    } else if (kline_type == KlineType::Month) {
        return 103;
    } else if (kline_type == KlineType::Quarter) {
        return 104;
    } else if (kline_type == KlineType::Year) {
        return 106;
    }
    return 0;
}

void SpiderShareKlineProviderEastMoney::ParseMinuteKline(const std::string& response,
                                                         std::vector<minuteKline>& minute_klines) {
    json _response = json::parse(response);
    std::string data = _response["data"]["trends"];
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

void SpiderShareKlineProviderEastMoney::ParseDayKline(const std::string& response, std::vector<uiKline>& uiKlines) {
    json _response = json::parse(response);
    json klines = _response["data"]["klines"];
    if (klines != "") {
        for (json::iterator it = klines.begin(); it != klines.end(); ++it) {
            uiKline ui_kline;
            std::vector<std::string> fields = split(*it, ",");
            ui_kline.day = fields[0];                                                  // 时间
            ui_kline.price_open = std::stod(fields[1]);                                // 开盘价
            ui_kline.price_close = std::stod(fields[2]);                               // 收盘价
            ui_kline.price_max = IsNaN(fields[3]) ? 0.0 : std::stod(fields[3]);        // 最高价
            ui_kline.price_min = IsNaN(fields[4]) ? 0.0 : std::stod(fields[4]);        // 最低价
            ui_kline.volume = IsNaN(fields[5]) ? 0.0 : std::stod(fields[5]);           // 成交量
            ui_kline.amount = IsNaN(fields[6]) ? 0.0 : std::stod(fields[6]);           // 成交额
            ui_kline.change_rate = IsNaN(fields[8]) ? 0.0 : std::stod(fields[8]);      // 涨跌幅
            ui_kline.change_amount = IsNaN(fields[9]) ? 0.0 : std::stod(fields[9]);    // 涨跌额
            ui_kline.turnover_rate = IsNaN(fields[10]) ? 0.0 : std::stod(fields[10]);  // 换手率

            uiKlines.push_back(ui_kline);
        }
    }
}