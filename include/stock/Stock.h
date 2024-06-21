#ifndef STOCK_H
#define STOCK_H

#include <stdint.h>
#include <atomic>
#include <mutex>
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
    uint8_t level;     // 行业分类等级(1,2,3)
    std::string name;  // 行业分类名称
};

enum class KlineType {
    Day = 0,      // 日K线
    Week = 1,     // 周K线
    Month = 2,    // 月K线
    Quarter = 3,  // 季度K线
    Year = 4      // 年K线
};

enum class DataProvider {
    FinanceBaidu,  // 百度财经
    EastMoney,     // 东方财富
    Hexun          // 和讯网
};

enum ShareCategoryType {
    Concept = 1,   // 概念板块
    Industry = 2,  // 行业板块
    Province = 4,  // 区域板块
};

struct Share;  // 前项声明
struct ShareConcept {
    std::string name;
    std::vector<Share*> shares;
};

struct ShareBonus {
    uint64_t value;    // 分红金额
    std::string plan;  // 分红方案
    std::string date;  // 分红日期
};

struct ShareCapital {
    uint64_t total;    // 总市值
    uint64_t trade;    // 流通股本
    std::string date;  // 股本变化时间
};

struct ShareHolder {
    uint32_t count;    // 股东人数
    std::string date;  // 股东人数公告日期
};

struct Top10ShareHolder {
    std::string name;         // 股东名称
    uint64_t share_quantity;  // 持股数量
};

struct ShareInvestmentFund {
    std::string name;         // 投资机构名称
    uint64_t share_quantity;  // 持股数量
};

struct ShareBasicInfo {
    std::string company_name;                                 // 公司名称
    std::string ipo_date;                                     // 首发上市日期
    std::string core_bussiness;                               // 主营业务
    std::vector<ShareBonus> history_bonus;                    // 历史分红方案
    uint64_t total_bonus;                                     // 上市总分红金额
    uint64_t total_fund;                                      // 上市总融资金额
    std::vector<ShareCapital> capital_change_history;         // 股本变化历史
    std::vector<ShareHolder> holder_change_history;           // 股东人数变化历史
    Top10ShareHolder top10_share_holders[10];                 // 10大股东
    Top10ShareHolder top10_trade_share_holders[10];           // 10大流通股东
    std::vector<ShareInvestmentFund> share_investment_funds;  // 投资机构
};

struct Share {
    int id;                               // 序号
    std::string code;                     // 股票代号
    std::string name;                     // 股票名称
    std::vector<std::string> old_names;   // 股票曾用名称
    double change_amount;                 // 涨跌额
    double change_rate;                   // 涨跌幅度
    uint64_t volume;                      // 成交量
    uint64_t amount;                      // 成交额
    double price_yesterday_close;         // 昨天收盘价
    double price_now;                     // 最新价
    double price_max;                     // 最高价
    double price_min;                     // 最低价
    double price_open;                    // 开盘价
    double price_close;                   // 收盘价
    double price_amplitude;               // 股价振幅
    double qrr;                           // 量比 quantity relative ratio
    double pe;                            // 市盈率 price earnings ratio (P/E ratio)
    double pb;                            // 市净率 price net asset ratio (PBR)
    double roe;                           // 净资产收益率 return on equity
    double turnover_rate;                 // 换手率
    double revenue;                       // 当前营收
    double bonus;                         // 当前分红
    double history_bonus;                 // 历史分红总额
    double history_fund;                  // 历史融资总额
    std::string operation_analysis;       // 经营评述
    uint64_t total_capital;               // 总市值
    uint64_t trade_capital;               // 流通股本
    ShareIndustry* industry;              // 所处行业分类
    std::string industry_name;            // 行业名称
    Market market;                        // 所在交易所
    std::string province;                 // 所在省份
    uint32_t employee_num;                // 员工数
    uint64_t register_capital;            // 注册资本
    std::vector<ShareConcept*> concepts;  // 所属概念板块
    ShareBasicInfo* ptr_basic_info;       // 股票基本信息
    // 必须定义拷贝构造函数，否则使用std::vector.push_back 栈对象，程序会崩溃
    Share();
    Share& operator=(const Share& other);
    Share(const Share& other);
    bool operator<(const Share& other) const;
};

// K线绘制
struct uiKlineRange {
    int begin;  // 起始K线下标
    int end;    // 结束K线下标
};

struct uiKline {
    std::string day;       // 交易日期
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
    double turnover_rate;  // 换手率
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
