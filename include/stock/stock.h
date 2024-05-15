#ifndef STOCK_H
#define STOCK_H

#include <string>
#include <vector>

enum class Market {
    ShangHai = 1,     // 沪市
    ShenZhen = 2,     // 深市
    ChuangYeBan = 3,  // 创业板
    KeChuangBan = 4,  // 科创板
    BeiJiaoSuo = 5    // 北交所
};

struct ShareIndustry {
    uint8_t source;    // 行业分类来源(申万行业分类/中信行业分类)
    std::string code;  // 股票代号
    uint8_t level;     // 行业分类等级(1,2,3)
    std::string name;  // 行业分类名称
};

struct Share {
    int id;                              // 序号
    std::string code;                    // 股票代号
    std::string name;                    // 股票名称
    std::vector<std::string> old_names;  // 股票曾用名称
    double price_now;                    // 最新价
    double change_amount;                // 涨跌额
    double change_rate;                  // 涨跌幅度
    uint64_t volume;                     // 成交量
    uint64_t amount;                     // 成交额
    double amplitude;                    // 振幅
    double price_max;                    // 最高价
    double price_min;                    // 最低价
    double price_open;                   // 开盘价
    double price_close;                  // 收盘价
    double qrr;                          // 量比 quantity relative ratio
    double pe;                           // 市盈率 price earnings ratio (P/E ratio)
    double pb;                           // 市净率 price net asset ratio (PBR)
    double roe;                          // 净资产收益率 return on equity
    double turnover_rate;                // 换手率
    double revenue;                      // 当前营收
    double bonus;                        // 当前分红
    double history_bonus;                // 历史分红总额
    double history_fund;                 // 历史融资总额
    std::string operation_analysis;      // 经营评述
    uint64_t total_capital;              // 总市值
    uint64_t trade_capital;              // 流通股本
    ShareIndustry* industry;             // 所处行业分类
    Market market;                       // 所在交易所
};

// K线绘制
struct uiKlineRange {
    int begin;  // 起始K线下标
    int end;    // 结束K线下标
};

struct uiKline {
    std::string day;       // 交易日期
    std::string code;      // 股票代号
    std::string name;      // 股票名称
    double market_cap;     // 股票市值
    double change_rate;    // 涨跌幅
    double change_amount;  // 涨跌额
    double trade_volume;   // 成交量
    double trade_amount;   // 成交额
    double price_open;     // 开盘价
    double price_close;    // 收盘价
    double price_max;      // 最高价
    double price_min;      // 最低价
    double price_now;      // 当前实时价
    uint8_t danger;        // 1:security 2:warning 3: danger 4: damage
    uint8_t favorite;      // 0:not favorite 1:favorite
};

inline std::string MARKET_NAME(Market market) {
    if (market == Market::ShangHai) {
        return "沪市";
    } else if (market == Market::ShenZhen) {
        return "深市";
    } else if (market == Market::ChuangYeBan) {
        return "创业板";
    } else if (market == Market::KeChuangBan) {
        return "科创板";
    } else if (market == Market::BeiJiaoSuo) {
        return "北交所";
    }
    return "";
}

#endif  // STOCK_H
