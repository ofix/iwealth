///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareHistory.cpp
// Purpose:     crawl stock history
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareKline.h"
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

void SpiderShareKline::DoCrawl() {
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
void SpiderShareKline::FetchShareAdjustDayKline(const Share& share) {
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
    for (std::vector<std::vector<uiKline>>::const_reverse_iterator it = multi_klines.crbegin();
         it != multi_klines.crend(); it++) {
        adjust_klines.insert(adjust_klines.end(), (*it).begin(), (*it).end());
    }
    m_pStockStorage->m_day_klines_adjust[share.code] = adjust_klines;
}

std::string SpiderShareKline::GetDayKlineUrlFinanceBaidu(const std::string& share_code,
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

std::string SpiderShareKline::GetDayKlineUrlFinanceBaidu(const Share& share, const std::string& end_date) {
    return GetDayKlineUrlFinanceBaidu(share.code, end_date);
}

std::string SpiderShareKline::GetWeekKlineUrlFinanceBaidu(const Share& share,
                                                          const std::string& end_date = "") {
    return GetWeekKlineUrlFinanceBaidu(share.code, end_date);
}

std::string SpiderShareKline::GetWeekKlineUrlFinanceBaidu(const std::string& share_code,
                                                          const std::string& end_date) {
    return "";
}

std::string SpiderShareKline::GetMonthKlineUrlFinanceBaidu(const Share& share,
                                                           const std::string& end_date = "") {
    return GetMonthKlineUrlFinanceBaidu(share.code, end_date);
}

std::string SpiderShareKline::GetMonthKlineUrlFinanceBaidu(const std::string& share_code,
                                                           const std::string& end_date) {
    return "";
}

std::string SpiderShareKline::GetYearKlineUrlFinanceBaidu(const Share& share,
                                                          const std::string& end_date = "") {
    return GetYearKlineUrlFinanceBaidu(share.code, end_date);
}

std::string SpiderShareKline::GetYearKlineUrlFinanceBaidu(const std::string& share_code,
                                                          const std::string& end_date) {
    return "";
}

std::string SpiderShareKline::GetDayKlineUrlEastMoney(const Share& share, const std::string& end_date = "") {
    return GetDayKlineUrlEastMoney(share.code, end_date);
}

std::string SpiderShareKline::GetDayKlineUrlEastMoney(const std::string& share_code,
                                                      const std::string& end_date) {
    // 东方财富行情中心 https://quote.eastmoney.com/concept/sz300729.html
    std::string url =
        "https://push2his.eastmoney.com/api/qt/stock/kline/get?"
        "fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13"
        "&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61"
        "&beg=0"
        "&end=20500101"
        "&ut=fa5fd1943c7b386f172d6893dbfba10b"
        "&rtntype=6"
        "&secid=0." +
        share_code +
        "&klt=101"
        "&fqt=1"
        "&cb=jsonp1716738365308";
    std::string url2 =
        "https://push2his.eastmoney.com/api/qt/stock/kline/get?"
        "&secid+=0." +
        share_code +
        "&ut=fa5fd1943c7b386f172d6893dbfba10b"
        "&fields1=f1,f2,f3,f4,f5,f6"
        "&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61"
        "&klt=101"
        "&fqt=1"
        "&end=20240526"
        "&lmt=570"
        "&cb=quote_jp9";
    return url;
}

std::string SpiderShareKline::GetWeekKlineUrlEastMoney(const Share& share, const std::string& end_date = "") {
    return GetWeekKlineUrlEastMoney(share.code, end_date);
}

std::string SpiderShareKline::GetWeekKlineUrlEastMoney(const std::string& share_code,
                                                       const std::string& end_date) {
    return "";
}

std::string SpiderShareKline::GetMonthKlineUrlEastMoney(const Share& share,
                                                        const std::string& end_date = "") {
    return GetMonthKlineUrlEastMoney(share.code, end_date);
}

std::string SpiderShareKline::GetMonthKlineUrlEastMoney(const std::string& share_code,
                                                        const std::string& end_date) {
    return "";
}

std::string SpiderShareKline::GetYearKlineUrlEastMoney(const Share& share, const std::string& end_date = "") {
    return GetYearKlineUrlEastMoney(share.code, end_date);
}
std::string SpiderShareKline::GetYearKlineUrlEastMoney(const std::string& share_code,
                                                       const std::string& end_date) {
    return "";
}

std::string SpiderShareKline::GetShareCodeFromUrl(const std::string& url) {
    return url;
}

std::vector<uiKline> SpiderShareKline::ParseResponse(std::string& response) {
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
void SpiderShareKline::ConcurrentFetchShareAdjustDayKline(size_t start_pos, size_t end_pos) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    std::list<std::string> urls;
    for (size_t i = start_pos; i <= end_pos; i++) {
        urls.push_back(GetRequestUrl(shares[i]));
    }
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareKline::ConcurrentResponseCallback, this, std::placeholders::_1);
    try {
        HttpConcurrentGet(urls, callback, static_cast<void*>(nullptr), 3);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void SpiderShareKline::ConcurrentResponseCallback(conn_t* conn) {
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
