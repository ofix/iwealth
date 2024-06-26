///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareBriefInfo.cpp
// Purpose:     crawl east money website's stock basic information
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareBriefInfo.h"
#include <list>
#include "net/ConcurrentRequest.h"
#include "stock/StockDataStorage.h"
#include "util/Global.h"
#include "util/Timer.h"

using json = nlohmann::json;

SpiderShareBriefInfo::SpiderShareBriefInfo(StockDataStorage* storage) : Spider(storage) {
}

SpiderShareBriefInfo::SpiderShareBriefInfo(StockDataStorage* storage, bool concurrent) : Spider(storage, concurrent) {
}

SpiderShareBriefInfo::~SpiderShareBriefInfo() {
}

void SpiderShareBriefInfo::DoCrawl() {
    if (this->IsConcurrentMode()) {
        ConcurrentFetchBriefInfo();
    } else {
        std::vector<Share> shares = m_pStockStorage->m_market_shares;
        for (size_t i = 0; i < shares.size(); i++) {
            Share share = shares[i];
            FetchBriefInfo(share);
        }
    }
}

void SpiderShareBriefInfo::FetchBriefInfo(Share& share) {
    std::string url = GetRequestUrl(share);
    std::string data = Fetch(url);
    ParseResponse(data, &share);
}

std::string SpiderShareBriefInfo::GetRequestUrl(const Share& share) {
    static std::map<Market, std::string> kv = {{Market::ShangHai, ".SH"},
                                               {Market::ShenZhen, ".SZ"},
                                               {Market::ChuangYeBan, ".SZ"},
                                               {Market::KeChuangBan, ".SH"}};
    // https://emweb.securities.eastmoney.com/pc_hsf10/pages/index.html?type=web&code=SZ002252&color=b#/gsgk
    std::string url =
        "https://datacenter.eastmoney.com/securities/api/data/v1/get?"
        "reportName=RPT_F10_BASIC_ORGINFO"
        "&columns=SECUCODE,SECURITY_CODE,SECURITY_NAME_ABBR,ORG_CODE,ORG_NAME,ORG_NAME_EN,FORMERNAME,STR_CODEA,STR_"
        "NAMEA,STR_CODEB,STR_NAMEB,STR_CODEH,STR_NAMEH,SECURITY_TYPE,EM2016,TRADE_MARKET,INDUSTRYCSRC1,PRESIDENT,LEGAL_"
        "PERSON,SECRETARY,CHAIRMAN,SECPRESENT,INDEDIRECTORS,ORG_TEL,ORG_EMAIL,ORG_FAX,ORG_WEB,ADDRESS,REG_ADDRESS,"
        "PROVINCE,ADDRESS_POSTCODE,REG_CAPITAL,REG_NUM,EMP_NUM,TATOLNUMBER,LAW_FIRM,ACCOUNTFIRM_NAME,ORG_PROFILE,"
        "BUSINESS_SCOPE,TRADE_MARKETT,TRADE_MARKET_CODE,SECURITY_TYPEE,SECURITY_TYPE_CODE,EXPAND_NAME_ABBR,EXPAND_NAME_"
        "PINYIN"
        "&pageNumber=1"
        "&pageSize=1"
        "&source=HSF10"
        "&client=PC"
        "&filter=(SECUCODE=%22" +
        share.code + kv.at(share.market) + "%22)";
    return url;
}

uint8_t SpiderShareBriefInfo::GetIndustryLevel(const std::string& industry_name) {
    uint8_t level = 0;
    for (char c : industry_name) {
        if (c == '-') {
            level += 1;
        }
    }
    level += 1;
    return level;
}

void SpiderShareBriefInfo::ConcurrentFetchBriefInfo() {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    std::list<std::string> urls;
    std::vector<void*> user_data = {};
    RequestStatistics* pStatistics = NewRequestStatistics(shares.size(), DataProvider::EastMoney);
    if (pStatistics == nullptr) {
        return;
    }
    for (size_t i = 0; i < shares.size(); i++) {
        urls.emplace_back(GetRequestUrl(shares[i]));
        BriefInfoCrawlExtra* pExtra = new BriefInfoCrawlExtra();
        if (!pExtra) {
            std::cout << "[error]: bad memory alloc BriefInfoCrawlExtra" << std::endl;
            return;
        }
        pExtra->share = &shares[i];
        pExtra->statistics = pStatistics;
        user_data.emplace_back(static_cast<void*>(pExtra));
    }
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareBriefInfo::ConcurrentResponseCallback, this, std::placeholders::_1);
    try {
        if (m_synchronize) {
            HttpConcurrentGet("EastMoney::BriefInfo", urls, callback, user_data, 6);
        } else {
            // 启动新线程进行并发请求
            std::thread crawl_thread(std::bind(
                static_cast<void (*)(const std::string&, const std::list<std::string>&, std::function<void(conn_t*)>&,
                                     const std::vector<void*>&, uint32_t)>(HttpConcurrentGet),
                "EastMoney::BriefInfo", urls, callback, user_data, 6));
            crawl_thread.detach();
        }
        std::function<void(uint32_t, void*)> timer_cb = [=](uint32_t timer_id, void* args) {
            OnTimerFetchBriefInfo(timer_id, args);
        };
        Timer::SetInterval(1000, timer_cb, 0);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void SpiderShareBriefInfo::OnTimerFetchBriefInfo(uint32_t timer_id, void* args) {
    if (this->HasFinish()) {
        m_pStockStorage->SaveShareNames();
        Timer::CancelTimer(timer_id);
        delete this;  // 要求 SpiderShareBriefInfo 必须堆中创建的
    } else {
        std::cout << "[BriefInfo]::progress: " << this->GetProgress() << std::endl;
    }
}

void SpiderShareBriefInfo::ConcurrentResponseCallback(conn_t* conn) {
    try {
        BriefInfoCrawlExtra* pExtra = static_cast<BriefInfoCrawlExtra*>(conn->extra);
        ParseResponse(conn->response, pExtra->share);
    } catch (std::exception& e) {
        std::cout << e.what() << " ,exception occurs in " << __func__ << std::endl;
    }
}

void SpiderShareBriefInfo::ParseResponse(std::string& response, Share* pShare) {
    json _response = json::parse(response);
    json o = _response["result"]["data"][0];
    if (!o["FORMERNAME"].is_null()) {
        std::string former_name = o["FORMERNAME"].template get<std::string>();
        std::vector<std::string> old_names = split(former_name, "→");
        for (auto& old_name : old_names) {
            m_pStockStorage->InsertShareNameToTrie(old_name, pShare->code);
        }
    }

    // 保存信息到文件
    pShare->staff_num = o["EMP_NUM"];             // 公司员工数量
    pShare->register_capital = o["REG_CAPITAL"];  // 公司注册资本
    ShareBriefInfo* pBriefInfo = new ShareBriefInfo();
    pBriefInfo->company_name = o["ORG_NAME"];
    pBriefInfo->old_names = o["FORMERNAME"].is_null() ? "" : o["FORMERNAME"];
    pBriefInfo->company_website = o["ORG_WEB"].is_null() ? "" : o["ORG_WEB"];
    pBriefInfo->registered_address = o["REG_ADDRESS"].is_null() ? "" : o["REG_ADDRESS"];
    pBriefInfo->staff_num = o["EMP_NUM"];
    pBriefInfo->registered_capital = o["REG_CAPITAL"].is_null() ? "" : o["REG_CAPITAL"];
    pBriefInfo->law_office = o["LAW_FIRM"].is_null() ? "" : o["LAW_FIRM"];
    pBriefInfo->accounting_office = o["ACCOUNTFIRM_NAME"].is_null() ? "" : o["ACCOUNTFIRM_NAME"];
    pBriefInfo->ceo = o["CHAIRMAN"].is_null() ? "" : o["CHAIRMAN"];
    pBriefInfo->board_secretary = o["SECRETARY"].is_null() ? "" : o["SECRETARY"];
    pBriefInfo->office_address = o["ADDRESS"].is_null() ? "" : o["ADDRESS"];
    pBriefInfo->company_profile = o["ORG_PROFILE"].is_null() ? "" : o["ORG_PROFILE"];
    pShare->ptr_brief_info = pBriefInfo;
    m_pStockStorage->SaveShareBriefInfo(pBriefInfo, pShare->code);
}