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

using json = nlohmann::json;

SpiderShareKline::SpiderShareKline(StockDataStorage* storage) : Spider(storage) {
    m_debug = true;
}

SpiderShareKline::SpiderShareKline(StockDataStorage* storage, bool concurrent) : Spider(storage, concurrent) {
    m_debug = true;
}

SpiderShareKline::~SpiderShareKline() {}

void SpiderShareKline::Crawl(KlineType kline_type) {
    m_timeStart = std::chrono::high_resolution_clock::now();
    DoCrawl(kline_type);
}

void SpiderShareKline::DoCrawl(KlineType kline_type) {
    if (this->IsConcurrentMode()) {
        std::vector<KlineCrawlTask> tasks = {
            {KlineProvider::FinanceBaidu, 0.5},
            {KlineProvider::EastMoney, 0.5},
        };
        ConurrentCrawl(tasks, kline_type);
    } else {
        std::vector<KlineCrawlTask> tasks = {
            {KlineProvider::FinanceBaidu, 0.5},
            {KlineProvider::EastMoney, 0.5},
        };
        SingleCrawl(tasks, kline_type);
    }
}

void SpiderShareKline::ConurrentCrawl(std::vector<KlineCrawlTask>& tasks, KlineType kline_type) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    size_t pos_start = 0, pos_end = 0;
    double priority = 0;
    for (size_t i = 0; i < tasks.size(); i++) {
        std::list<std::string> urls;
        std::vector<void*> user_data;
        priority += tasks[i].priority;
        pos_end = std::round((shares.size() - 1) * priority);
        RequestStatistics* pStatistics = new RequestStatistics();
        if (!pStatistics) {
            gLogger->log("[ConcurrentCrawl] allocate memory failed");
            return;
        }
        pStatistics->provider = tasks[i].provider;
        m_statisticsList.push_back(pStatistics);
        for (size_t j = pos_start; j <= pos_end; j++) {
            urls.push_back(GetKlineUrl(tasks[i].provider, kline_type, shares[j].code, shares[j].market));
            KlineCrawlExtra* pExtra = new KlineCrawlExtra();
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
        std::thread crawl_thread(
            std::bind(static_cast<void (*)(const std::list<std::string>&, std::function<void(conn_t*)>&,
                                           const std::vector<void*>&, uint32_t)>(HttpConcurrentGet),
                      urls, callback, user_data, 3));
        crawl_thread.detach();
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
            urls.push_back(GetKlineUrl(tasks[i].provider, kline_type, shares[j].code, shares[j].market));
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
    }
    return "";
}

std::string SpiderShareKline::GetKlineUrl(const KlineProvider provider,   // 供应商
                                          const KlineType kline_type,     // K线类型
                                          const std::string& share_code,  // 股票代码
                                          const Market market,            // 股票市场
                                          const std::string& end_date     // 结束日期
) {
    if (provider == KlineProvider::FinanceBaidu) {
        std::string extra = "";
        if (end_date != "") {
            extra = "&end_time=" + end_date + "&count=3000";
        }
        std::string baidu_kline_type = GetKlineTypeFinanceBaidu(kline_type);
        std::string url = KLINE_URL_FINANCE_BAIDU(share_code, baidu_kline_type, extra);
        return url;
    } else if (provider == KlineProvider::EastMoney) {
        int market_code = GetEastMoneyMarketCode(market);
        int east_money_kline_type = GetKlineTypeEastMoney(kline_type);
        std::string url = KLINE_URL_EAST_MONEY(share_code, market_code, east_money_kline_type);
        return url;
    }
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

/// @brief 响应处理
/// @param conn
std::vector<uiKline> SpiderShareKline::ParseResponse(conn_t* conn) {
    KlineCrawlExtra* pExtra = static_cast<KlineCrawlExtra*>(conn->extra);
    std::vector<uiKline> uiKlines = {};
    if (pExtra->provider == KlineProvider::FinanceBaidu) {
        ParseResponseFinanceBaidu(conn, uiKlines);
    } else if (pExtra->provider == KlineProvider::EastMoney) {
        ParseResponseEastMoney(conn, uiKlines);
    }
    return uiKlines;
}

// 解析百度财经返回数据
// 百度财经数据完整历史K线需要请求多次，因此这里做统计最合适
void SpiderShareKline::ParseResponseFinanceBaidu(conn_t* conn, std::vector<uiKline>& uiKlines) {
    json _response = json::parse(conn->response);
    std::string data = _response["Result"]["newMarketData"]["marketData"];
    if (data == "") {
        conn->reuse = false;  // 所有数据已经请求完毕，不再复用conn
    } else {
        std::vector<std::string> klines = split(data, ';');
        std::string end_time = "";
        for (size_t i = 0; i < klines.size(); i++) {
            std::vector<std::string> fields = split(klines[i], ',');
            try {
                uiKline kline;
                kline.day = fields[1];                     // 时间
                kline.price_open = std::stod(fields[2]);   // 开盘价
                kline.price_close = std::stod(fields[3]);  // 收盘价
                if (fields[4] != "-") {
                    kline.trade_volume = std::stod(fields[4]);  // 成交量
                } else {
                    kline.trade_volume = -1000;  // 成交量
                }
                if (fields[5] != "-") {
                    kline.price_max = std::stod(fields[5]);  // 最高价
                }
                if (fields[6] != "-") {
                    kline.price_min = std::stod(fields[6]);  // 最低价
                }
                kline.trade_amount = std::stod(fields[7]);    // 成交额
                kline.change_amount = std::stod(fields[8]);   // 涨跌额
                kline.change_rate = std::stod(fields[9]);     // 涨跌幅
                kline.turnover_rate = std::stod(fields[10]);  // 换手率
                uiKlines.push_back(kline);
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

// 解析东方财富返回数据
void SpiderShareKline::ParseResponseEastMoney(conn_t* conn, std::vector<uiKline>& uiKlines) {
    json _response = json::parse(conn->response);
    json klines = _response["data"]["klines"];
    if (klines != "") {
        for (json::iterator it = klines.begin(); it != klines.end(); ++it) {
            std::vector<std::string> fields = split(*it, ',');
            uiKline kline;
            kline.day = fields[0];                     // 时间
            kline.price_open = std::stod(fields[1]);   // 开盘价
            kline.price_close = std::stod(fields[2]);  // 收盘价
            if (fields[3] != "-") {
                kline.price_max = std::stod(fields[3]);  // 最高价
            }
            if (fields[4] != "-") {
                kline.price_min = std::stod(fields[4]);  // 最低价
            }
            if (fields[5] != "-") {
                kline.trade_volume = std::stod(fields[5]);  // 成交量
            } else {
                kline.trade_volume = -1000;  // 成交量
            }
            if (fields[6] != "-") {
                kline.trade_amount = std::stod(fields[6]);  // 成交额
            } else {
                kline.trade_volume = -1000;  // 成交额
            }
            kline.change_rate = std::stod(fields[8]);     // 涨跌幅
            kline.change_amount = std::stod(fields[9]);   // 涨跌额
            kline.turnover_rate = std::stod(fields[10]);  // 换手率
            uiKlines.push_back(kline);
        }
    }
}

void SpiderShareKline::SingleResponseCallback(conn_t* conn) {
    KlineCrawlExtra* pExtra = static_cast<KlineCrawlExtra*>(conn->extra);
    std::vector<uiKline> multi_kline = ParseResponse(conn);
    if (pExtra->provider == KlineProvider::FinanceBaidu && multi_kline.size() > 0) {
        std::string end_date = multi_kline[0].day;
        std::string share_code = pExtra->share->code;
        conn->url = GetKlineUrl(pExtra->provider, pExtra->type, share_code, pExtra->market, end_date);
        conn->reuse = true;  // 需要复用
        this->m_concurrent_klines[share_code].push_back(multi_kline);
    }
}

// 此成员函数通常在分离的线程中运行
void SpiderShareKline::ConcurrentResponseCallback(conn_t* conn) {
    KlineCrawlExtra* pExtra = static_cast<KlineCrawlExtra*>(conn->extra);
    std::vector<uiKline> multi_kline = ParseResponse(conn);
    if (pExtra->provider == KlineProvider::FinanceBaidu && multi_kline.size() > 0) {
        std::string end_date = multi_kline[0].day;
        std::string share_code = pExtra->share->code;
        conn->url = GetKlineUrl(pExtra->provider, pExtra->type, share_code, pExtra->market, end_date);
        conn->reuse = true;  // 需要复用
        this->m_concurrent_klines[share_code].push_back(multi_kline);
    }
}