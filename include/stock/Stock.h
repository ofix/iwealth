#ifndef STOCK_H
#define STOCK_H

#include<string>

#define MARKET_NAME_ZH(i) ((i==1)?_T("沪市"):((i==2)?_T("深市"):_T("创业板")))
#define MARKET_NAME_EN(i) ((i==1)?_T("SHANG HAI"):((i==2)?_T("SHEN ZHEN"):_T("CHUAN YE BAN")))
#define MARKET_SHANG_HAI 1
#define MARKET_SHEN_ZHEN 2
#define MARKET_CHUAN_YE_BAN 3

struct ShareBrief {
    std::string code; // 股票代号
    std::string name; // 股票名称
    short market;     // 股票市场 1-沪市 2-深市 3-创业板 4-科创板 5-北交所
};

struct ShareDetail {
    int id;
    std::string code;       // 股票代号
    std::string name;       // 股票名称
    double price_now;       // 最新价
    double change_amount;   // 涨跌额
    double change_rate;     // 涨跌幅度
    uint64_t volume;        // 成交量
    uint64_t amount;        // 成交额
    double amplitude;       // 振幅
    double price_max;       // 最高价
    double price_min;       // 最低价
    double price_open;      // 开盘价
    double price_close;     // 收盘价
    double qrr;             // 量比 quantity relative ratio
    double pe;              // 市盈率
    double pb;              // 市净率
    double turnover_rate;   // 换手率
    uint64_t total_capital; // 总市值
    uint64_t trade_capital; // 流通股本

};

// K线绘制
struct uiKlineRange {
    int begin; // 起始K线下标
    int end;   // 结束K线下标
};

struct KlineItem {
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

struct TimelineItem {
    std::string code;      // 股票名称
    std::string time;      // 股票时间
    double price;          // 成交价格
    int hand;              // 成交总手数
};

struct ShareItem {
    std::string code;      // 股票代码
    std::string name;      // 股票名称
    double price;          // 当前价格
    double change_percent; // 涨跌幅
    double change;         // 涨跌额
    double to;             // 成交金额 turnover
    double qr;             // 量比 quantity relative
    double pe;             // 市盈率 price earnings ratio (P/E ratio)
    double pb;             // 市净率 price net asset ratio (PBR)
    double roe;            // 净资产收益率 return on equity
};

#endif // STOCK_H
