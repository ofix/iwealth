#ifndef STOCK_H
#define STOCK_H

#include <stdint.h>
#include <wx/wx.h>
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
    MINUTE = 0,    // 分时图
    FIVE_DAY = 1,  // 近5日分时图
    Day = 2,       // 日K线
    Week = 3,      // 周K线
    Month = 4,     // 月K线
    Quarter = 5,   // 季度K线
    Year = 6,      // 年K线
    All = 7        // 以上所有K线
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
    std::vector<ShareBonus> history_bonus;                    // 历史分红方案
    uint64_t total_bonus;                                     // 上市总分红金额
    uint64_t total_fund;                                      // 上市总融资金额
    std::vector<ShareCapital> capital_change_history;         // 股本变化历史
    std::vector<ShareHolder> holder_change_history;           // 股东人数变化历史
    std::vector<ShareInvestmentFund> share_investment_funds;  // 投资机构
    Top10ShareHolder top10_share_holders[10];                 // 10大股东
    Top10ShareHolder top10_trade_share_holders[10];           // 10大流通股东
};

struct ShareBriefInfo {
    std::string company_name;       // 公司名称
    std::string old_names;          // 公司曾用名
    std::string company_website;    // 公司网址
    std::string register_address;   // 注册地址
    int staff_num;                  // 雇员人数
    double register_capital;        // 注册资本
    std::string law_office;         // 律师事务所
    std::string accounting_office;  // 会计事务所
    std::string ceo;                // 公司董事长
    std::string board_secretary;    // 董秘
    std::string office_address;     // 办公地址
    std::string company_profile;    // 公司简介
};

struct Share {
    int id;                               // 序号
    std::string code;                     // 股票代号
    std::string name;                     // 股票名称
    double change_amount;                 // 涨跌额
    double change_rate;                   // 涨跌幅度
    uint64_t volume;                      // 成交量
    double amount;                        // 成交额
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
    int staff_num;                        // 员工数
    uint64_t register_capital;            // 注册资本
    std::vector<ShareConcept*> concepts;  // 所属概念板块
    ShareBriefInfo* ptr_brief_info;       // 公司简要信息
    ShareBasicInfo* ptr_basic_info;       // 股票基本信息
    // 必须定义拷贝构造函数，否则使用std::vector.push_back 栈对象，程序会崩溃
    Share();
    Share& operator=(const Share& other);
    Share(const Share& other);
    bool operator<(const Share& other) const;
};

// EMA指数移动平均线
struct ShareEmaCurve {
    int period;                     // 周期
    wxColor color;                  // 曲线显示颜色
    bool visible;                   // 是否显示
    std::vector<double> ema_price;  // 收盘价简单移动平均值
};

// 自选股
struct FavoriteShare {
    Share* pShare;                     // share指针
    std::string favoriate_date;        // 添加日期
    double add_price;                  // 添加自选时的股价
    double total_change_rate;          // 加入自选后的涨跌幅
    double recent_5_days_change_rate;  // 最近5日涨跌幅
    double recent_month_change_rate;   // 最近1个月涨跌幅
    double recent_year_change_rate;    // 今年以来涨跌幅
};

// 自选股分组
struct FavoriteShareGroup {
    std::string name;  // 自选股分组名称
    std::vector<FavoriteShare> shares;
};

// 监控股
struct MonitorShare {
    Share* pShare;
    std::string moniter_date;    // 加入监控时的日期
    double monitor_price;        // 加入监控时的股价
    double expect_change_rate;   // 期望跌幅
    double max_change_rate;      // 最大跌幅
    int max_decrease_used_days;  // 跌幅最深耗时天数
    double current_change_rate;  // 当前跌幅
    int monitor_days;            // 监控天数
};

// K线绘制
struct uiKlineRange {
    int begin;  // 起始K线下标
    int end;    // 结束K线下标
};

// (五日)分时图绘制
struct minuteKline {
    time_t timestamp;       // 交易时间戳
    std::string time;       // 交易时间
    double price;           // 价格
    double avg_price;       // 分时均价
    double change_amount;   // 涨跌额
    double change_rate;     // 涨跌幅
    uint64_t volume;        // 成交量
    double amount;          // 成交额
    uint64_t total_volume;  // 累计成交量
    double total_amount;    // 累计成交额
};

struct uiKline {
    std::string day;             // 交易日期
    double market_cap = 0.0;     // 股票市值
    double change_rate = 0.0;    // 涨跌幅
    double change_amount = 0.0;  // 涨跌额
    uint64_t volume = 0;         // 成交量
    double amount = 0.0;         // 成交额
    double price_open = 0.0;     // 开盘价
    double price_close = 0.0;    // 收盘价
    double price_max = 0.0;      // 最高价
    double price_min = 0.0;      // 最低价
    double price_now = 0.0;      // 当前实时价
    double turnover_rate = 0.0;  // 换手率
    uint8_t danger = 0.0;        // 1:security 2:warning 3: danger 4: damage
    uint8_t favorite = 0.0;      // 0:not favorite 1:favorite
};

inline std::string MARKET_NAME(Market market) {
    if (market == Market::ShangHai) {
        return "上海A股";
    } else if (market == Market::ShenZhen) {
        return "深圳A股";
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
