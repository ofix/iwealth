///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareHistory.cpp
// Purpose:     crawl stock history
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareDayKlineBaidu.h"
#include "net/ConcurrentRequest.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "util/Global.h"

using json = nlohmann::json;

SpiderShareDayKlineBaidu::SpiderShareDayKlineBaidu(StockDataStorage* storage)
    : Spider(storage) {
    m_debug = true;
}

SpiderShareDayKlineBaidu::SpiderShareDayKlineBaidu(StockDataStorage* storage,
                                                   bool concurrent)
    : Spider(storage, concurrent) {
    m_debug = true;
}

SpiderShareDayKlineBaidu::~SpiderShareDayKlineBaidu() {}

void SpiderShareDayKlineBaidu::DoCrawl() {
    if (this->IsConcurrentMode()) {
        ConcurrentFetchShareAdjustDayKline(m_posStart, m_posEnd);
    } else {
        std::vector<Share> shares = m_pStockStorage->m_market_shares;
        for (size_t i = m_posStart; i <= m_posEnd; i++) {
            Share share = shares[i];
            FetchShareAdjustDayKline(share);
        }
    }
}

// 分批抓取股票历史K线
void SpiderShareDayKlineBaidu::FetchShareAdjustDayKline(const Share& share) {
    std::string url = GetRequestUrl(share);
    std::string data = Fetch(url, CURL_HTTP_VERSION_2_0);

    std::vector<std::vector<uiKline>> multi_klines;
    std::vector<uiKline> multi_kline = ParseResponse(data);
    multi_klines.push_back(multi_kline);
    std::string end_date = multi_kline[0].day;
    for (;;) {
        std::string url = GetRequestUrl(share, end_date);
        std::string data = Fetch(url, CURL_HTTP_VERSION_2_0);
        std::vector<uiKline> multi_kline = ParseResponse(data);
        if (multi_kline.size() == 0) {
            break;
        }
        // 必须在push_back之前提前赋值，因为multi_kline指向的内存地址可能发生变更了
        end_date = multi_kline[0].day;
        multi_klines.push_back(multi_kline);
    }
    std::vector<uiKline> adjust_klines = {};
    for (std::vector<std::vector<uiKline>>::const_reverse_iterator it =
             multi_klines.crbegin();
         it != multi_klines.crend(); it++) {
        adjust_klines.insert(adjust_klines.end(), (*it).begin(), (*it).end());
    }
    m_pStockStorage->m_day_klines_adjust[share.code] = adjust_klines;
}

std::string SpiderShareDayKlineBaidu::GetRequestUrl(const std::string& share_code,
                                                    const std::string& end_date) {
    std::string extra = "";
    if (end_date != "") {
        extra = "&end_time=" + end_date + "&count=3000";
    }
    std::string url =
        "https://finance.pae.baidu.com/vapi/v1/getquotation"
        "?srcid=5353"
        "&pointType=string"
        "&group=quotation_kline_ab"
        "&query=" +
        share_code + "&code=" + share_code +
        "&market_type=ab"
        "&newFormat=1"
        "&is_kc=0"
        "&ktype=day"
        "&finClientType=pc" +
        extra + "&finClientType=pc";
    return url;
}

std::string SpiderShareDayKlineBaidu::GetRequestUrl(const Share& share,
                                                    const std::string& end_date) {
    return GetRequestUrl(share.code, end_date);
}

std::string SpiderShareDayKlineBaidu::GetShareCodeFromUrl(const std::string& url) {
    return url;
}

std::vector<uiKline> SpiderShareDayKlineBaidu::ParseResponse(std::string& response) {
    json _response = json::parse(response);
    std::string data = _response["Result"]["newMarketData"]["marketData"];
    std::vector<uiKline> uiKlines = {};
    if (data != "") {
        std::vector<std::string> klines = split(data, ';');
        std::string end_time = "";
        for (size_t i = 0; i < klines.size(); i++) {
            std::vector<std::string> fields = split(klines[i], ',');
            try {
                uiKline kline;
                kline.day = fields[1];                        // 时间
                kline.price_open = std::stod(fields[2]);      // 开盘价
                kline.price_close = std::stod(fields[3]);     // 收盘价
                kline.trade_volume = std::stod(fields[4]);    // 成交量
                kline.price_max = std::stod(fields[5]);       // 最高价
                kline.price_min = std::stod(fields[6]);       // 最低价
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
    return uiKlines;
}

// 并发请求
void SpiderShareDayKlineBaidu::ConcurrentFetchShareAdjustDayKline(size_t start_pos,
                                                                  size_t end_pos) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    std::vector<std::string> urls;
    for (size_t i = start_pos; i <= end_pos; i++) {
        urls.push_back(GetRequestUrl(shares[i]));
    }
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareDayKlineBaidu::ConcurrentResponseCallback, this,
                  std::placeholders::_1);
    try {
        HttpConcurrentGet(urls, callback, static_cast<void*>(nullptr), 3);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void SpiderShareDayKlineBaidu::ConcurrentResponseCallback(conn_t* conn) {
    try {
        std::vector<uiKline> multi_kline = ParseResponse(conn->response);
        if (multi_kline.size() > 0) {
            std::string end_date = multi_kline[0].day;
            std::string share_code = GetShareCodeFromUrl(conn->url);
            conn->url = GetRequestUrl(share_code, end_date);
            conn->reuse = true;  // 需要复用
            this->m_concurrent_klines[share_code].push_back(multi_kline);
        }
    } catch (std::exception& e) {
        std::cout << e.what() << " ,exception occurs in " << __func__ << std::endl;
    }
}
