///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareKline.cpp
// Purpose:     crawl stock history
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareKline.h"
#include <thread>
#include "net/ConcurrentRequest.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "util/EasyLogger.h"
#include "util/Global.h"
#include "util/Timer.h"

using json = nlohmann::json;

SpiderShareKline::SpiderShareKline(StockDataStorage* storage) : Spider(storage) {
}

SpiderShareKline::SpiderShareKline(StockDataStorage* storage, bool concurrent) : Spider(storage, concurrent) {
}

SpiderShareKline::~SpiderShareKline() {
}

void SpiderShareKline::Crawl(KlineType kline_type) {
    m_timeStart = std::chrono::high_resolution_clock::now();
    DoCrawl(kline_type);
}

void SpiderShareKline::CrawlSync(Share* pShare, KlineType kline_type) {
    std::vector<DataProvider> providers = {
        DataProvider::FinanceBaidu,
        DataProvider::EastMoney,
    };
    std::vector<DataProvider> data_providers;
    int iProvider = rand_int(0, providers.size() - 1);
    DataProvider provider = providers[iProvider];
    std::list<std::string> urls;
    std::vector<void*> user_data;
    urls.push_back(GetKlineUrl(provider, kline_type, pShare->code, pShare->name, pShare->market));
    RequestStatistics* pStatistics = NewRequestStatistics(1, provider);
    if (pStatistics == nullptr) {
        return;
    }

    KlineCrawlExtra* pExtra = new KlineCrawlExtra();
    pExtra->provider = provider;
    pExtra->type = kline_type;
    pExtra->market = pShare->market;
    pExtra->share = pShare;
    pExtra->statistics = pStatistics;
    user_data.push_back(static_cast<void*>(pExtra));
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareKline::ConcurrentResponseCallback, this, std::placeholders::_1);
    if (provider == DataProvider::FinanceBaidu) {
        HttpConcurrentGet("kline " + GetProviderName(provider), urls, callback, user_data, 1, CURL_HTTP_VERSION_1_1);
    } else {
        HttpConcurrentGet("kline " + GetProviderName(provider), urls, callback, user_data);
    }
    MergeShareKlines(m_concurrent_day_klines_adjust, m_pStockStorage->m_day_klines_adjust);
}

void SpiderShareKline::DoCrawl(KlineType kline_type) {
    if (this->IsConcurrentMode()) {
        std::vector<KlineCrawlTask> tasks = {
            {DataProvider::FinanceBaidu, 0.5},
            {DataProvider::EastMoney, 0.5},
        };
        ConurrentCrawl(tasks, kline_type);
    } else {
        std::vector<KlineCrawlTask> tasks = {
            {DataProvider::FinanceBaidu, 0.5},
            {DataProvider::EastMoney, 0.5},
        };
        SingleCrawl(tasks, kline_type);
    }
}

std::string SpiderShareKline::GetProviderName(DataProvider provider) const {
    if (provider == DataProvider::EastMoney) {
        return "EastMoney";
    } else if (provider == DataProvider::FinanceBaidu) {
        return "Baidu";
    }
    return "Unknown";
}

void SpiderShareKline::ConurrentCrawl(std::vector<KlineCrawlTask>& tasks, KlineType kline_type) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    size_t pos_start = 0, pos_end = 0;
    double priority = 0;
    for (size_t i = 0; i < tasks.size(); i++) {
        std::list<std::string> urls;
        std::vector<void*> user_data = {};
        priority += tasks[i].priority;
        pos_end = std::round((shares.size() - 1) * priority);
        RequestStatistics* pStatistics = NewRequestStatistics(pos_end - pos_start + 1, tasks[i].provider);
        if (pStatistics == nullptr) {
            return;
        }
        std::string provider_name = GetProviderName(tasks[i].provider);
        for (size_t j = pos_start; j <= pos_end; j++) {
            urls.push_back(
                GetKlineUrl(tasks[i].provider, kline_type, shares[j].code, shares[j].name, shares[j].market));
            KlineCrawlExtra* pExtra = new KlineCrawlExtra();
            if (!pExtra) {
                std::cout << "[error]: bad memory alloc KlineCrawlExtra" << std::endl;
                return;
            }
            pExtra->provider = tasks[i].provider;
            pExtra->type = kline_type;
            pExtra->market = shares[j].market;
            pExtra->share = &shares[j];
            pExtra->statistics = pStatistics;
            user_data.push_back(static_cast<void*>(pExtra));
        }
        pos_start = pos_end + 1;
        std::function<void(conn_t*)> callback =
            std::bind(&SpiderShareKline::ConcurrentResponseCallback, this, std::placeholders::_1);
        // 启动新线程进行并发请求
        std::thread crawl_thread(std::bind(
            static_cast<void (*)(const std::string&, const std::list<std::string>&, std::function<void(conn_t*)>&,
                                 const std::vector<void*>&, int, int)>(HttpConcurrentGet),
            provider_name, urls, callback, user_data, 3, CURL_HTTP_VERSION_1_1));
        crawl_thread.detach();
    }
    // 启动定时器更新打印速度，并打印爬取进度,以下两种方法绑定成员函数都是OK的
    // std::function<void(uint32_t, void*)> timerCallback =
    //     std::bind(&SpiderShareKline::OnRequestTimer, this, std::placeholders::_1, std::placeholders::_2);
    std::function<void(uint32_t, void*)> timerCallback = [=](uint32_t timer_id, void* args) {
        OnRequestTimer(timer_id, args);
    };

    uint32_t timer_id = Timer::SetInterval(1000, timerCallback);
    if (timer_id == TIMER_CREATE_FAILED) {
        std::cout << "timer created failed!" << std::endl;
    }
}

void SpiderShareKline::OnRequestTimer(uint32_t timer_id, void* /*args*/) {
    this->UpdateRequestStatistics();
    std::cout << "kline sync progress: " << GetProgress() << std::endl;
    // 取消周期性定时任务
    if (this->IsConcurrentMode() && this->HasFinish()) {
        Timer::CancelTimer(timer_id);
    }
}

void SpiderShareKline::SingleCrawl(std::vector<KlineCrawlTask>& tasks, KlineType kline_type) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    size_t pos_start = 0, pos_end = 0;
    double priority = 0;
    std::list<std::string> urls;
    std::vector<void*> user_data;
    for (size_t i = 0; i < tasks.size(); i++) {
        priority += tasks[i].priority;
        pos_end = std::round((shares.size() - 1) * priority);
        for (size_t j = pos_start; j <= pos_end; j++) {
            urls.push_back(
                GetKlineUrl(tasks[i].provider, kline_type, shares[j].code, shares[j].name, shares[j].market));
            KlineCrawlExtra* pExtra = new KlineCrawlExtra();
            pExtra->provider = tasks[i].provider;
            pExtra->type = kline_type;
            pExtra->market = shares[j].market;
            pExtra->share = &shares[j];
        }
        pos_start = pos_end + 1;
    }
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareKline::SingleResponseCallback, this, std::placeholders::_1);
    // 启动新线程进行并发请求

    std::thread crawl_thread(
        std::bind(static_cast<void (*)(const std::list<std::string>&, std::function<void(conn_t*)>&,
                                       const std::vector<void*>&, int)>(HttpBatchGet),
                  urls, callback, user_data, CURL_HTTP_VERSION_2_0));
    crawl_thread.detach();
}

std::string SpiderShareKline::GetKlineTypeFinanceBaidu(const KlineType kline_type) {
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
    } else if (kline_type == KlineType::MINUTE) {
        return "minute";
    } else if (kline_type == KlineType::FIVE_DAY) {
        return "five_day";
    }
    return "";
}

std::string SpiderShareKline::GetKlineUrl(const DataProvider provider,    // 供应商
                                          const KlineType kline_type,     // K线类型
                                          const std::string& share_code,  // 股票代码
                                          const std::string& share_name,  // 股票名称
                                          const Market market,            // 股票市场
                                          const std::string& end_date     // 结束日期
) {
    if (provider == DataProvider::FinanceBaidu) {
        if (kline_type == KlineType::MINUTE) {
            return KLINE_URL_FINANCE_BAIDU_MINUTE(share_code);
        } else if (kline_type == KlineType::FIVE_DAY) {
            return KLINE_URL_FINANCE_BAIDU_FIVE_DAY(share_code, share_name);
        } else {
            std::string extra = "";
            if (end_date != "") {
                extra = "&end_time=" + end_date + "&count=3000";
            }
            std::string baidu_kline_type = GetKlineTypeFinanceBaidu(kline_type);
            std::string url = KLINE_URL_FINANCE_BAIDU(share_code, baidu_kline_type, extra);
            return url;
        }
    } else if (provider == DataProvider::EastMoney) {
        int market_code = GetEastMoneyMarketCode(market);
        int east_money_kline_type = GetKlineTypeEastMoney(kline_type);
        if (kline_type == KlineType::MINUTE) {
            return KLINE_URL_EAST_MONEY_MINUTE(share_code, market_code);
        } else if (kline_type == KlineType::FIVE_DAY) {
            return KLINE_URL_EAST_MONEY_FIVE_DAY(share_code, market_code);
        } else {
            return KLINE_URL_EAST_MONEY(share_code, market_code, east_money_kline_type);
        }
    }
    return "";
}

/////////////////// 东方财富 ///////////////////
int SpiderShareKline::GetEastMoneyMarketCode(const Market market) {
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

int SpiderShareKline::GetKlineTypeEastMoney(const KlineType kline_type) {
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

// 检查是否是合法的double数字
bool SpiderShareKline::IsNaN(const std::string& data) {
    if (data.length() == 0) {
        return true;  // 空字符串
    }
    if (data.length() >= 2) {
        if (data[1] == '-') {  // 检查第二个字符是否是-,如果是，
            return true;       // ---或者--都是不合法的，为了快速判断，无需判断其他字符
        }
        return false;
    }
    return std::isdigit(data[0]) ? false : true;  // 只有一个字符的情况，判断是否是数字
}

void SpiderShareKline::ParseBaiduMinuteKline(conn_t* conn, std::vector<std::vector<minuteKline>>& minute_klines) {
    json _response = json::parse(conn->response);
    json days = _response["Result"]["newMarketData"]["marketData"];
    for (json::iterator it = days.begin(); it != days.end(); ++it) {
        std::string data = (*it)["p"];
        std::vector<std::string> rows = split(data, ";");
        std::vector<minuteKline> one_day_minute_klines;
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
            one_day_minute_klines.push_back(minute_kline);
        }
        minute_klines.push_back(one_day_minute_klines);  // 可能包含5天的分时图
    }
}

// 解析百度财经返回数据
// 百度财经数据完整历史K线需要请求多次，因此这里做统计最合适
void SpiderShareKline::ParseBaiduDayKline(conn_t* conn, std::vector<uiKline>& uiKlines) {
    json _response = json::parse(conn->response);
    std::string data = _response["Result"]["newMarketData"]["marketData"];
    if (data != "") {
        std::vector<std::string> rows = split(data, ";");
        for (size_t i = 0; i < rows.size(); i++) {
            uiKline ui_kline;
            std::vector<std::string> fields = split(rows[i], ",");
            ui_kline.day = fields[1];                                                  // 时间
            ui_kline.price_open = std::stod(fields[2]);                                // 开盘价
            ui_kline.price_close = std::stod(fields[3]);                               // 收盘价
            ui_kline.volume = IsNaN(fields[4]) ? 0 : std::stoull(fields[4]);           // 成交量
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

void SpiderShareKline::ParseEastMoneyMinuteKline(conn_t* conn, std::vector<std::vector<minuteKline>>& minute_klines) {
    json _response = json::parse(conn->response);
    std::string data = _response["data"]["trends"];
    std::vector<std::string> rows = split(data, ";");
    std::vector<minuteKline> one_day_minute_klines;
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
        one_day_minute_klines.push_back(minute_kline);
    }
}

// 解析东方财富返回数据
void SpiderShareKline::ParseEastMoneyDayKline(conn_t* conn, std::vector<uiKline>& uiKlines) {
    json _response = json::parse(conn->response);
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

void SpiderShareKline::SingleResponseCallback(conn_t* conn) {
    KlineCrawlExtra* pExtra = static_cast<KlineCrawlExtra*>(conn->extra);
    if (pExtra->provider == DataProvider::FinanceBaidu) {
        // 分时，近五日分时
        if (pExtra->type == KlineType::MINUTE || pExtra->type == KlineType::FIVE_DAY) {
            m_minute_klines.empty();
            ParseBaiduMinuteKline(conn, m_minute_klines);
        } else {  // 日/周/月/季/年
            std::vector<uiKline> multi_kline = {};
            ParseBaiduDayKline(conn, multi_kline);
            if (multi_kline.size() > 0) {
                std::string end_date = multi_kline[0].day;
                std::string share_code = pExtra->share->code;
                conn->url = GetKlineUrl(pExtra->provider, pExtra->type, share_code, pExtra->share->name, pExtra->market,
                                        end_date);
                conn->reuse = true;  // 需要复用
                this->m_concurrent_day_klines_adjust[share_code].push_back(multi_kline);
            } else {
                conn->reuse = false;  // 不需要复用
            }
        }
    }
}

// 此成员函数通常在分离的线程中运行
void SpiderShareKline::ConcurrentResponseCallback(conn_t* conn) {
    KlineCrawlExtra* pExtra = static_cast<KlineCrawlExtra*>(conn->extra);
    std::string share_code = pExtra->share->code;
    if (pExtra->provider == DataProvider::FinanceBaidu) {
        if (pExtra->type == KlineType::MINUTE || pExtra->type == KlineType::FIVE_DAY) {
            m_minute_klines.empty();
            ParseBaiduMinuteKline(conn, m_minute_klines);
            conn->reuse = false;
        } else {
            std::vector<uiKline> multi_kline = {};
            ParseBaiduDayKline(conn, multi_kline);
            if (multi_kline.size() > 0) {
                std::string end_date = multi_kline[0].day;
                conn->url = GetKlineUrl(pExtra->provider, pExtra->type, share_code, pExtra->share->name, pExtra->market,
                                        end_date);
                conn->reuse = true;  // 需要复用
                this->m_concurrent_day_klines_adjust[share_code].push_back(multi_kline);
            } else {
                conn->reuse = false;  // 不需要复用
            }
        }

    } else if (pExtra->provider == DataProvider::EastMoney) {  // 东方财富只需要请求一次，即可获取所有数据
        if (pExtra->type == KlineType::MINUTE || pExtra->type == KlineType::FIVE_DAY) {
            m_minute_klines.empty();
            ParseEastMoneyMinuteKline(conn, m_minute_klines);
        } else {
            std::vector<uiKline> multi_kline = {};
            ParseEastMoneyDayKline(conn, multi_kline);
            this->m_concurrent_day_klines_adjust[share_code].push_back(multi_kline);
        }
        conn->reuse = false;
    }
}

// 合并K线，可以考虑移动语义
void SpiderShareKline::MergeShareKlines(const KlineType kline_type) {
    if (kline_type == KlineType::Day) {
        MergeShareKlines(m_concurrent_day_klines_adjust, m_pStockStorage->m_day_klines_adjust);
    } else if (kline_type == KlineType::Week) {
        MergeShareKlines(m_concurrent_week_klines_adjust, m_pStockStorage->m_week_klines_adjust);
    } else if (kline_type == KlineType::Month) {
        MergeShareKlines(m_concurrent_month_klines_adjust, m_pStockStorage->m_month_klines_adjust);
    } else if (kline_type == KlineType::Year) {
        MergeShareKlines(m_concurrent_year_klines_adjust, m_pStockStorage->m_year_klines_adjust);
    }
}

void SpiderShareKline::MergeShareKlines(
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>>& concurrent_klines,
    std::unordered_map<std::string, std::vector<uiKline>>& target_klines) {
    for (std::unordered_map<std::string, std::vector<std::vector<uiKline>>>::const_iterator it =
             concurrent_klines.begin();
         it != concurrent_klines.end(); ++it) {
        const std::string& share_code = it->first;
        const std::vector<std::vector<uiKline>>& multi_klines = it->second;
        size_t kline_count = GetKlineCount(multi_klines);
        std::vector<uiKline> history_kline;
        history_kline.reserve(kline_count);  // 提升std::vector插入性能，避免频繁申请内存
        std::vector<std::vector<uiKline>>::const_reverse_iterator it_klines;
        for (it_klines = multi_klines.crbegin(); it_klines != multi_klines.crend(); ++it_klines) {
            const std::vector<uiKline>& kline = *it_klines;
            history_kline.insert(history_kline.end(), kline.begin(), kline.end());
        }
        // 对键和值进行相应操作
        target_klines.insert({share_code, history_kline});
    }
}
void SpiderShareKline::MergeShareKlines(std::unordered_map<std::string, std::vector<uiKline>>& /*klines*/,
                                        std::unordered_map<std::string, std::vector<uiKline>>& /*target_klines*/) {
}

// 获取所有历史向量K线的数量，避免std::vector容器反复分配内存带来的性能损失
size_t SpiderShareKline::GetKlineCount(const std::vector<std::vector<uiKline>>& multi_klines) {
    std::vector<std::vector<uiKline>>::const_iterator it;
    std::size_t count = 0;
    for (it = multi_klines.cbegin(); it != multi_klines.cend(); ++it) {
        count += (*it).size();
    }
    return count;
}

void SpiderShareKline::DumpKline(uiKline& kline) {
    std::cout << kline.day << "," << std::fixed << std::setprecision(2) << kline.price_now << "," << kline.price_max
              << "," << kline.price_min << "," << kline.price_open << "," << kline.price_close << ","
              << kline.change_rate << "," << kline.change_amount << "," << kline.volume << "," << kline.amount << ","
              << kline.turnover_rate;
    std::cout << std::endl;
}