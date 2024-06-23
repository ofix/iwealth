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
    ParseResponse(data, &share);
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
    std::vector<void*> user_data = {};
    RequestStatistics* pStatistics = NewRequestStatistics(shares.size(), DataProvider::EastMoney);
    if (pStatistics == nullptr) {
        return;
    }
    for (size_t i = 0; i <= shares.size(); i++) {
        urls.emplace_back(GetRequestUrl(shares[i]));
        BasicInfoCrawlExtra* pExtra = new BasicInfoCrawlExtra();
        if (!pExtra) {
            std::cout << "[error]: bad memory alloc BasicInfoCrawlExtra" << std::endl;
            return;
        }
        pExtra->share = &shares[i];
        pExtra->statistics = pStatistics;
        user_data.push_back(static_cast<void*>(pExtra));
    }
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareBasicInfo::ConcurrentResponseCallback, this, std::placeholders::_1);
    try {
        HttpConcurrentGet("EastMoney::BasicInfo", urls, callback, user_data, 3);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void SpiderShareBasicInfo::ConcurrentResponseCallback(conn_t* conn) {
    try {
        BasicInfoCrawlExtra* pExtra = static_cast<BasicInfoCrawlExtra*>(conn->extra);
        ParseResponse(conn->response, pExtra->share);
    } catch (std::exception& e) {
        std::cout << e.what() << " ,exception occurs in " << __func__ << std::endl;
    }
}

void SpiderShareBasicInfo::ParseResponse(std::string& response, Share* pShare) {
    json _response = json::parse(response);
    json o = _response["result"]["data"][0];
    if (!o["FORMERNAME"].is_null()) {
        std::string former_name = o["FORMERNAME"].template get<std::string>();
        std::vector<std::string> old_names = split(former_name, ",");
        for (auto& old_name : old_names) {
            m_pStockStorage->InsertShareNameToTrie(old_name, pShare->code);
        }
    }
    m_pStockStorage->InsertShareNameToTrie(pShare->name, pShare->code);
    // 保存信息到文件
    pShare->staff_num = o["EMP_NUM"];             // 公司员工数量
    pShare->register_capital = o["REG_CAPITAL"];  // 公司注册资本
    ShareBriefInfo* pBriefInfo = new ShareBriefInfo();
    pBriefInfo->company_name = o["ORG_NAME"];
    pBriefInfo->old_names = !o["FORMERNAME"].is_null() ? o["FORMERNAME"] : "";
    pBriefInfo->company_website = o["ORG_WEB"];
    pBriefInfo->registered_address = o["REG_ADDRESS"];
    pBriefInfo->staff_num = o["EMP_NUM"];
    pBriefInfo->registered_capital = o["REG_CAPITAL"];
    pBriefInfo->law_office = o["LAW_FIRM"];
    pBriefInfo->accounting_office = o["ACCOUNTFIRM_NAME"];
    pBriefInfo->ceo = o["CHAIRMAN"];
    pBriefInfo->board_secretary = o["SECRETARY"];
    pBriefInfo->office_address = o["ADDRESS"];
    pBriefInfo->company_profile = o["ORG_PROFILE"];
    pShare->ptr_brief_info = pBriefInfo;
}