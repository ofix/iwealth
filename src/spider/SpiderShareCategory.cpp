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

SpiderShareCategory::SpiderShareCategory(StockDataStorage* storage, int types, bool concurrent)
    : Spider(storage, concurrent), m_types(types) {
}

SpiderShareCategory::~SpiderShareCategory() {
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
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {
        std::string key = (*it)["key"];
        if (key == "hsbroad") {
            json concepts = (*it)["next"][0]["next"];
            json provinces = (*it)["next"][1]["next"];
            json industries = (*it)["next"][2]["next"];
            if (m_types & ShareCategoryType::Province) {
                m_provinces.clear();
                FetchCategoryShares(provinces, ShareCategoryType::Province);
            }
            if (m_types & ShareCategoryType::Industry) {
                m_industries.clear();
                FetchCategoryShares(industries, ShareCategoryType::Industry);
            }
            if (m_types & ShareCategoryType::Concept) {
                m_concepts.clear();
                FetchCategoryShares(concepts, ShareCategoryType::Concept);
            }
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

    RequestStatistics* pStatistics = NewRequestStatistics(categories.size(), DataProvider::EastMoney);
    if (pStatistics == nullptr) {
        return;
    }
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
    std::string thread_name = "EastMoney::" + GetCategoryTypeName(type);
    if (m_synchronize) {
        HttpConcurrentGet(thread_name, urls, callback, user_data, 3);
    } else {
        // 启动新线程进行并发请求
        std::thread crawl_thread(std::bind(
            static_cast<void (*)(const std::string&, const std::list<std::string>&, std::function<void(conn_t*)>&,
                                 const std::vector<void*>&, uint32_t)>(HttpConcurrentGet),
            thread_name, urls, callback, user_data, 3));
        crawl_thread.detach();
    }
}

std::string SpiderShareCategory::GetCategoryTypeName(ShareCategoryType type) {
    if (type == ShareCategoryType::Concept) {
        return "Concept";
    } else if (type == ShareCategoryType::Industry) {
        return "Industry";
    } else if (type == ShareCategoryType::Province) {
        return "Province";
    }
    return "Unknown";
}

// 此成员函数通常在分离的线程中运行
void SpiderShareCategory::ConcurrentResponseCallback(conn_t* conn) {
    CategoryCrawlExtra* pExtra = static_cast<CategoryCrawlExtra*>(conn->extra);
    json response = json::parse(conn->response);
    json shares = response["data"]["diff"];
    std::string category_name = pExtra->category_name;
    std::unordered_map<std::string, std::vector<std::string>>* hash_map;
    if (pExtra->category_type == ShareCategoryType::Concept) {
        hash_map = &m_concepts;
    } else if (pExtra->category_type == ShareCategoryType::Industry) {
        hash_map = &m_industries;
    } else if (pExtra->category_type == ShareCategoryType::Province) {
        hash_map = &m_provinces;
    }
    for (json::iterator it = shares.begin(); it != shares.end(); ++it) {
        std::string share_code = (*it)["f12"];
        // std::string share_name = (*it)["f14"];
        InsertCategory(*hash_map, category_name, share_code);
    }
    conn->reuse = false;  // 不需要复用
}

void SpiderShareCategory::BuildShareCategoryProvinces() {
    for (auto& province : m_provinces) {
        for (auto& share_code : province.second) {
            Share* pShare = m_pStockStorage->FindShare(share_code);
            if (pShare != nullptr) {  // 可能不包含北交所的一些垃圾股票
                m_pStockStorage->m_category_provinces.Insert(province.first, pShare);
                pShare->province = province.first;
            }
        }
    }
}

void SpiderShareCategory::BuildShareCategoryIndustries() {
    for (auto& industry : m_industries) {
        for (auto& share_code : industry.second) {
            Share* pShare = m_pStockStorage->FindShare(share_code);
            if (pShare != nullptr) {  // 可能不包含北交所的一些垃圾股票
                m_pStockStorage->m_category_industries.Insert(industry.first, pShare);
                pShare->industry_name = industry.first;
            }
        }
    }
}

void SpiderShareCategory::BuildShareCategoryConcepts() {
}

std::unordered_map<std::string, std::vector<std::string>>* SpiderShareCategory::GetCategory(ShareCategoryType type) {
    if (type == ShareCategoryType::Province) {
        return &m_provinces;
    } else if (type == ShareCategoryType::Industry) {
        return &m_industries;
    } else if (type == ShareCategoryType::Concept) {
        return &m_concepts;
    }
}

std::string SpiderShareCategory::GetCategoryKey(std::string name) {
    std::size_t found = name.find_last_of(".");
    return name.substr(found + 1);
}

void SpiderShareCategory::InsertCategory(std::unordered_map<std::string, std::vector<std::string>>& hash_map,
                                         std::string& category_name,
                                         std::string& share_code) {
    if (hash_map.find(category_name) != hash_map.end()) {
        hash_map[category_name].push_back(share_code);
    } else {
        std::vector<std::string> shares;
        shares.push_back(share_code);
        hash_map[category_name] = shares;
    }
}