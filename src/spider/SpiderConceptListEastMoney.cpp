///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderConceptListEastMoney.cpp
// Purpose:     crawl east money website's concept list
// Author:      songhuabiao
// Created:     2024-05-21 10:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderConceptListEastMoney.h"
#include "net/ConcurrentRequest.h"
#include "stock/StockDataStorage.h"
#include "util/Global.h"

using json = nlohmann::json;

SpiderConceptListEastMoney::SpiderConceptListEastMoney(StockDataStorage* storage)
    : Spider(storage) {
    m_debug = true;
}

SpiderConceptListEastMoney::SpiderConceptListEastMoney(StockDataStorage* storage,
                                                       bool concurrent)
    : Spider(storage, concurrent) {
    m_debug = true;
}

SpiderConceptListEastMoney::~SpiderConceptListEastMoney() {}

void SpiderConceptListEastMoney::DoCrawl() {
    FetchConceptList();
}

void SpiderConceptListEastMoney::FetchConceptList() {
    std::string url = GetRequestUrl();
    std::string data = Fetch(url);
    ParseResponse(data);
}

std::string SpiderConceptListEastMoney::GetRequestUrl() {
    std::string url =
        "http://57.push2.eastmoney.com/api/qt/clist/"
        "get?pn=1&pz=20000&po=1&np=1&fs=m:90+t:3+f:!50&fields=f14";
    return url;
}

void SpiderConceptListEastMoney::ParseResponse(std::string& response) {
    json _response = json::parse(response);
    json arr = _response["data"]["diff"];
    m_pStockStorage->m_market_concepts.clear();  // 清空概念个股映射表
    m_pStockStorage->m_market_concepts.reserve(arr.size());  // 防止频繁和移动底层数组
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
        std::string concept_name = (*it)["f14"];
        ShareConcept* pConcept = new ShareConcept();
        pConcept->name = concept_name;
        pConcept->shares = std::vector<Share*>();
        m_pStockStorage->m_market_concepts[concept_name] = pConcept;
        if (m_debug) {
            std::cout << concept_name << ",";
        }
    }
    if (m_debug) {
        std::cout << std::endl;
    }
}
