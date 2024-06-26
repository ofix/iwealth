///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareBriefInfo.cpp
// Purpose:     crawl He Xun website's stock list information
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareQuote.h"

#include <wx/string.h>

#include "net/ConcurrentRequest.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "util/EasyLogger.h"

using json = nlohmann::json;

SpiderShareQuote::SpiderShareQuote(StockDataStorage* storage) : Spider(storage) {
    m_concurrentMode = true;
}

SpiderShareQuote::~SpiderShareQuote() {
}

void SpiderShareQuote::DoCrawl() {
    if (this->IsConcurrentMode()) {
        ConcurrentCrawl();
    } else {
        SingleCrawl();
    }
}

void SpiderShareQuote::ConcurrentCrawl() {
    m_unique_shares.clear();
    std::list<std::string> urls;
    std::vector<void*> user_data;
    std::vector<int> market_code = {1, 2, 6, 1789};
    RequestStatistics* pStatistics = NewRequestStatistics(4, DataProvider::Hexun);
    for (int code : market_code) {
        urls.push_back(GetFetchUrl(code));
        QuoteCrawlExtra* pExtra = new QuoteCrawlExtra();
        if (!pExtra) {
            std::cout << "[error]: bad memory alloc CrawlExtra" << std::endl;
            return;
        }
        pExtra->market = GetMarket(code);
        pExtra->statistics = pStatistics;
        user_data.push_back(static_cast<void*>(pExtra));
    }

    // 发送并发请求
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareQuote::ConcurrentResponseCallback, this, std::placeholders::_1);
    if (m_synchronize) {
        HttpConcurrentGet("Hexun", urls, callback, user_data, 4);
    } else {
        // 启动新线程进行并发请求
        std::thread crawl_thread(std::bind(
            static_cast<void (*)(const std::string&, const std::list<std::string>&, std::function<void(conn_t*)>&,
                                 const std::vector<void*>&, uint32_t)>(HttpConcurrentGet),
            "Hexun", urls, callback, user_data, 4));
        crawl_thread.detach();
    }
}

void SpiderShareQuote::ConcurrentResponseCallback(conn_t* conn) {
    QuoteCrawlExtra* pExtra = static_cast<QuoteCrawlExtra*>(conn->extra);
    ParseStockListData(conn->response, pExtra->market);
    conn->reuse = false;
}

void SpiderShareQuote::SaveShareListToDataStorage() {
    m_pStockStorage->m_market_shares.clear();
    m_pStockStorage->m_market_shares.reserve(6000);  // 避免内存频繁分配
    // 重新插入
    m_pStockStorage->m_market_shares.insert(m_pStockStorage->m_market_shares.end(), m_unique_shares.begin(),
                                            m_unique_shares.end());
    // 对股票涨幅进行排序
    std::stable_sort(m_pStockStorage->m_market_shares.begin(), m_pStockStorage->m_market_shares.end(),
                     [](Share a, Share b) {
                         return a.change_rate > b.change_rate;
                     });
    // 释放内存
    m_unique_shares.clear();
}

void SpiderShareQuote::SingleCrawl() {
    m_pStockStorage->m_market_shares.clear();
    m_pStockStorage->m_market_shares.reserve(6000);  // 避免内存频繁分配
    m_unique_shares.clear();
    FetchMarketShares(1);          // 沪市A股
    FetchMarketShares(2);          // 深市A股
    FetchMarketShares(6);          // 创业板
    FetchMarketShares(1789);       // 科创板
    SaveShareListToDataStorage();  // 移除重复的股票
}

void SpiderShareQuote::FetchMarketShares(int market) {
    std::string url = GetFetchUrl(market);
    std::string data = Fetch(url);
    ParseStockListData(data, GetMarket(market));
}

Market SpiderShareQuote::GetMarket(int market) {
    static std::map<int, Market> kv = {
        {1, Market::ShangHai},
        {2, Market::ShenZhen},
        {6, Market::ChuangYeBan},
        {1789, Market::KeChuangBan},
    };
    return kv.at(market);
}

/**
 * @brief 获取抓取市场信息的URL
 * @param market 和讯网市场请求参数
 * @return URL
 */
std::string SpiderShareQuote::GetFetchUrl(int market) {
    std::string url = "https://stocksquote.hexun.com/a/sortlist";
    std::string url_sh = url + "?block=" + std::to_string(market) +
                         "&title=15"
                         "&direction=0"
                         "&start=0"
                         "&number=10000"
                         "&column=code,name,price,updownrate,LastClose,open,high,low,volume,priceweight,amount,"
                         "exchangeratio,VibrationRatio,VolumeRatio";
    return url_sh;
}

void SpiderShareQuote::ParseStockListData(std::string& data, Market market) {
    data = regex_replace(data, std::regex{R"(\()"}, "");
    data = regex_replace(data, std::regex{R"(\);)"}, "");
    // std::cout << wxString::FromUTF8(data) << std::endl;
    json o = json::parse(data);
    const int count = o["Total"].template get<int>();
    json arr = o["Data"][0];
    m_pStockStorage->m_market_share_total = 0;
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
        Share share;
        share.code = (*it)[0].get<std::string>();                       // 股票代号
        share.name = (*it)[1].get<std::string>();                       // 股票名称
        int factor = (*it)[9].get<double>();                            // 成交额
        share.price_yesterday_close = (*it)[4].get<double>() / factor;  // 昨天收盘价
        share.price_now = (*it)[2].get<double>() / factor;              // 最新价
        share.change_rate = (*it)[3].get<double>() / 100;               // 涨跌幅
        share.price_open = (*it)[5].get<double>() / factor;             // 开盘价
        share.price_max = (*it)[6].get<double>() / factor;              // 最高价
        share.price_min = (*it)[7].get<double>() / factor;              // 最低价
        share.volume = (*it)[8].get<double>() / 100;                    // 成交量
        share.amount = (*it)[10].get<double>();                         // 成交额
        share.turnover_rate = (*it)[11].get<double>() / 100;            // 换手率
        share.price_amplitude = (*it)[12].get<double>() / 100;          // 股价振幅
        share.qrr = (*it)[13].get<double>() / 100;                      // 成交量比
        share.market = market;                                          // 股票市场
        m_unique_shares.insert(share);
    }
    m_pStockStorage->m_market_share_count.insert({market, count});
    m_pStockStorage->m_market_share_total += count;
}
