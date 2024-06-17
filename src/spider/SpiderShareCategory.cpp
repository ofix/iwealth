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
// https://quote.eastmoney.com/center
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
            FetchCategoryShares(concepts, ShareCategoryType::Concept);
            FetchCategoryShares(industries, ShareCategoryType::Industry);
            FetchCategoryShares(regions, ShareCategoryType::Region);
            break;
        }
    }
}

// 请求URL
// https://2.push2.eastmoney.com/api/qt/clist/get?cb=jQuery112407682394798394985_1718628704355&pn=1&pz=20&po=1&np=1&ut=bd1d9ddb04089700cf9c27f6f7426281&fltt=2&invt=2&dect=1&wbp2u=|0|0|0|web&fid=f3&fs=b:BK0169+f:!50&fields=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f13,f14,f15,f16,f17,f18,f20,f21,f23,f24,f25,f22,f11,f62,f128,f136,f115,f152,f45&_=1718628704356
// https://2.push2.eastmoney.com/api/qt/clist/get?pn=1&pz=2000&po=1&np=1fltt=2&invt=2&dect=1&wbp2u=|0|0|0|web&fid=f3&fs=b:BK0169+f:!50&fields=f12,f14
// https://2.push2.eastmoney.com/api/qt/clist/get?pn=1&pz=20000&po=1&np=1&fltt=2&invt=2&dect=1&fid=f3&fs=b:BK0169&fields=f3,f12,f14
void SpiderShareCategory::FetchCategoryShares(nlohmann::json& categories, ShareCategoryType type) {
    std::list<std::string> urls = {};
    std::vector<void*> user_data = {};
    for (json::iterator it = categories.begin(); it != categories.end(); ++it) {
        std::string category_key = GetCategoryKey((*it)["key"]);
        std::string category_name = (*it)["title"];
        std::string url =
            "https://2.push2.eastmoney.com/api/qt/clist/get"
            "?pn=1"
            "&pz=10000"  // 控制页数
            "&po=1"
            "&np=1"  // 控制返回格式
            "&fltt=2"
            "&invt=2"
            "&dect=1"
            "&fid=f3"  // 决定返回数据顺序
            "&fs=b:" +
            category_key +  // 查询那个板块
            "&fields=f3,f12,f14";
        urls.push_back(url);
        RequestStatistics* pStatistics = new RequestStatistics();
        if (!pStatistics) {
            gLogger->log("[ConcurrentCrawl] allocate memory failed");
            return;
        }
        pStatistics->provider = KlineProvider::EastMoney;
        pStatistics->request_count = categories.size();
        CategoryCrawlExtra* pExtra = new CategoryCrawlExtra();
        if (!pExtra) {
            std::cout << "[error]: bad memory alloc CategoryCrawlExtra" << std::endl;
            return;
        }
        pExtra->category_name = category_name;
        pExtra->category_type = type;
        pExtra->statistics = pStatistics;
        user_data.push_back(static_cast<void*>(pExtra));
    }
    // 发送并发请求
    std::function<void(conn_t*)> callback =
        std::bind(&SpiderShareCategory::ConcurrentResponseCallback, this, std::placeholders::_1);
    // 启动新线程进行并发请求
    std::string thread_name = "EastMoney::" + GetCategoryTypeName(type);
    std::thread crawl_thread(
        std::bind(static_cast<void (*)(const std::string&, const std::list<std::string>&, std::function<void(conn_t*)>&,
                                       const std::vector<void*>&, uint32_t)>(HttpConcurrentGet),
                  thread_name, urls, callback, user_data, 3));
    crawl_thread.detach();
}

std::string SpiderShareCategory::GetCategoryTypeName(ShareCategoryType type) {
    if (type == ShareCategoryType::Concept) {
        return "Concept";
    } else if (type == ShareCategoryType::Industry) {
        return "Industry";
    } else if (type == ShareCategoryType::Region) {
        return "Region";
    }
}

// 此成员函数通常在分离的线程中运行
void SpiderShareCategory::ConcurrentResponseCallback(conn_t* conn) {
    CategoryCrawlExtra* pExtra = static_cast<CategoryCrawlExtra*>(conn->extra);
    json response = json::parse(conn->response);
    json shares = response["data"]["diff"];
    std::string category_name = pExtra->category_name;
    ShareCategory* share_category = nullptr;
    if (pExtra->category_type == ShareCategoryType::Concept) {
        share_category = &m_pStockStorage->m_category_concepts;
    } else if (pExtra->category_type == ShareCategoryType::Industry) {
        share_category = &m_pStockStorage->m_category_industries;
    } else if (pExtra->category_type == ShareCategoryType::Region) {
        share_category = &m_pStockStorage->m_category_regions;
    }
    for (json::iterator it = shares.begin(); it != shares.end(); ++it) {
        std::string share_code = (*it)["f12"];
        std::string share_name = (*it)["f14"];
        Share* pShare = m_pStockStorage->FindShare(share_code);
        share_category->Insert(category_name, pShare);
    }
    conn->reuse = false;  // 不需要复用
}

std::string SpiderShareCategory::GetCategoryKey(std::string name) {
    std::size_t found = name.find_last_of(".");
    return name.substr(found + 1);
}