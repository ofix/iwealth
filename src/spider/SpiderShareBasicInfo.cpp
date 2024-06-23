///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareBasicInfo.cpp
// Purpose:     crawl east money website's stock basic information
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareBasicInfo.h"
#include <list>
#include "net/ConcurrentRequest.h"
#include "stock/StockDataStorage.h"
#include "util/Global.h"

using json = nlohmann::json;

SpiderShareBasicInfo::SpiderShareBasicInfo(StockDataStorage* storage) : Spider(storage) {
}

SpiderShareBasicInfo::SpiderShareBasicInfo(StockDataStorage* storage, bool concurrent) : Spider(storage, concurrent) {
}

SpiderShareBasicInfo::~SpiderShareBasicInfo() {
}

void SpiderShareBasicInfo::DoCrawl() {
    if (this->IsConcurrentMode()) {
        ConcurrentFetchBasicInfo();
    } else {
        std::vector<Share> shares = m_pStockStorage->m_market_shares;
        for (size_t i = 0; i < shares.size(); i++) {
            Share share = shares[i];
            FetchBasicInfo(share);
        }
    }
}

void SpiderShareBasicInfo::FetchBasicInfo(Share& share) {
    std::string url = GetRequestUrl(share);
    std::string data = Fetch(url);
    ParseResponse(data, share);
}

std::string SpiderShareBasicInfo::GetRequestUrl(const Share& share) {
    static std::map<Market, std::string> kv = {{Market::ShangHai, ".SH"},
                                               {Market::ShenZhen, ".SZ"},
                                               {Market::ChuangYeBan, ".SZ"},
                                               {Market::KeChuangBan, ".SH"}};
    // https://emweb.securities.eastmoney.com/pc_hsf10/pages/index.html?type=web&code=SZ002252&color=b#/gsgk
    std::string url =
        "https://datacenter.eastmoney.com/securities/api/data/v1/get?"
        "reportName=RPT_F10_BASIC_ORGINFO"
        "&columns=SECUCODE,"
        "SECURITY_CODE,"
        "SECURITY_NAME_ABBR,"
        "EM2016,"
        "INDUSTRYCSRC1,"
        "PROVINCE,"
        "REG_CAPITAL,"
        "FORMERNAME,"
        "REG_NUM,"
        "EMP_NUM"
        "&pageNumber=1"
        "&pageSize=1"
        "&source=HSF10"
        "&client=PC"
        "&filter=(SECUCODE=%22" +
        share.code + kv.at(share.market) + "%22)";
    return url;
}

uint8_t SpiderShareBasicInfo::GetIndustryLevel(const std::string& industry_name) {
    uint8_t level = 0;
    for (char c : industry_name) {
        if (c == '-') {
            level += 1;
        }
    }
    level += 1;
    return level;
}

void SpiderShareBasicInfo::ConcurrentFetchBasicInfo() {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    std::list<std::string> urls;
    for (size_t i = 0; i <= shares.size(); i++) {
        urls.push_back(GetRequestUrl(shares[i]));
    }
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareBasicInfo::ConcurrentResponseCallback, this, std::placeholders::_1);
    try {
        HttpConcurrentGet("EastMoney", urls, callback, static_cast<void*>(&shares), 3);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void SpiderShareBasicInfo::ConcurrentResponseCallback(conn_t* conn) {
    try {
        Share share;
        ParseResponse(conn->response, share);
    } catch (std::exception& e) {
        std::cout << e.what() << " ,exception occurs in " << __func__ << std::endl;
    }
}

void SpiderShareBasicInfo::ParseResponse(std::string& response, Share& share) {
    json _response = json::parse(response);
    json o = _response["result"]["data"][0];
    if (!o["FORMERNAME"].is_null()) {
        std::string old_names = o["FORMERNAME"].template get<std::string>();
        // share.old_names = split(old_names, ",");
    }
    share.province = o["PROVINCE"];                                                       // 公司所属省份
    share.employee_num = o["EMP_NUM"];                                                    // 公司员工数量
    share.register_capital = o["REG_CAPITAL"];                                            // 公司注册资本
    ShareIndustry* pIndustry = new ShareIndustry();                                       // 公司所处行业
    pIndustry->source = 1;                                                                // 行业来源
    pIndustry->name = o["INDUSTRYCSRC1"];                                                 // 行业名称
    pIndustry->level = GetIndustryLevel(o["INDUSTRYCSRC1"].template get<std::string>());  // 所处行业等级
    share.industry = pIndustry;
}