///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderBasicInfoEastMoney.cpp
// Purpose:     crawl He Xun website's stock list information
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareListHexun.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "wx/string.h"

using json = nlohmann::json;

SpiderShareListHexun::SpiderShareListHexun(StockDataStorage* storage) : Spider(storage) {}

SpiderShareListHexun::~SpiderShareListHexun() {}

void SpiderShareListHexun::DoCrawl() {
    FetchMarketShares(1);     // 沪市A股
    FetchMarketShares(2);     // 深市A股
    FetchMarketShares(6);     // 创业板
    FetchMarketShares(1789);  // 科创板
}

void SpiderShareListHexun::FetchMarketShares(int market) {
    static std::map<int, Market> kv = {
        {1, Market::ShangHai}, {2, Market::ShenZhen}, {6, Market::ChuangYeBan}, {1789, Market::KeChuangBan}};
    std::string url = "https://stocksquote.hexun.com/a/sortlist";
    std::string url_sh = url + "?block=" + std::to_string(market) +
                         "&title=15"
                         "&direction=0"
                         "&start=0"
                         "&number=10000"
                         "&column=code,name,price,updownrate,LastClose,open,high,low,volume,priceweight,amount,"
                         "exchangeratio,VibrationRatio,VolumeRatio";
    std::string data = Fetch(url_sh);
    ParseStockListData(data, kv.at(market));
}

void SpiderShareListHexun::ParseStockListData(std::string& data, Market market) {
    data = regex_replace(data, std::regex{R"(\()"}, "");
    data = regex_replace(data, std::regex{R"(\);)"}, "");
    // std::cout << wxString::FromUTF8(data) << std::endl;
    json o = json::parse(data);
    const int count = o["Total"].template get<int>();
    json arr = o["Data"][0];
    m_pStockStorage->m_market_shares.clear();
    m_pStockStorage->m_market_shares.reserve(6000);  // 避免内存频繁分配
    m_pStockStorage->m_market_share_total = 0;
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
        Share share;
        share.code = (*it)[0].get<std::string>();               // 股票代号
        share.name = (*it)[1].get<std::string>();               // 股票名称
        share.price_now = (*it)[2].get<double>() / 100;         // 最新价
        share.change_rate = (*it)[3].get<double>() / 100;       // 涨跌幅
        share.price_open = (*it)[5].get<double>() / 100;        // 开盘价
        share.price_max = (*it)[6].get<double>() / 100;         // 最高价
        share.price_min = (*it)[7].get<double>() / 100;         // 最低价
        share.volume = (*it)[8].get<double>() / 100;            // 成交量
        share.amount = (*it)[10].get<double>() / 100;           // 成交额
        share.turnover_rate = (*it)[11].get<double>() / 100;    // 换手率
        share.price_amplitude = (*it)[12].get<double>() / 100;  // 股价振幅
        share.qrr = (*it)[13].get<double>() / 100;              // 成交量比
        share.market = market;                                  // 股票市场
        m_pStockStorage->m_market_shares.push_back(share);
    }
    m_pStockStorage->m_market_share_count.insert({market, count});
    m_pStockStorage->m_market_share_total += count;
}
