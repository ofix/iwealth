///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareCategory.cpp
// Purpose:     crawl stock industry classification
// Author:      songhuabiao
// Created:     2024-06-17 16:19
// Copyright:   (C) songhuabiao, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareCategory.h"
#include <string>
#include <thread>
#include "net/ConcurrentRequest.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "util/EasyLogger.h"
#include "util/Global.h"
#include "util/Timer.h"

SpiderShareCategory::SpiderShareCategory(StockDataStorage* storage) : Spider(storage) {
}

SpiderShareCategory::SpiderShareCategory(StockDataStorage* storage, bool concurrent) : Spider(storage, concurrent) {
}

SpiderShareCategory::~SpiderShareCategory() {
}

void SpiderShareCategory::Crawl() {
    m_timeStart = std::chrono::high_resolution_clock::now();
    DoCrawl();
}

// 爬取 地域板块/概念板块/行业版块
void SpiderShareCategory::DoCrawl() {
    std::string url = "http://quote.eastmoney.com/center/api/sidemenu.json";
    std::string data = Fetch(url);
    ParseCategories(data);
}

void SpiderShareCategory::ParseCategories(std::string& data) {
    json arr = json::parse(data);
    std::vector<std::string> share_industries;
    std::vector<std::string> share_concepts;
    std::vector<std::string> share_regions;
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
        std::string key = (*it)["key"];
        if (key == "hsbroad") {
            json concepts = (*it)["next"][0]["next"];
            json regions = (*it)["next"][1]["next"];
            json industries = (*it)["next"][2]["next"];
            break;
        }
    }
}