///////////////////////////////////////////////////////////////////////////////
// Name:           iwealth/core/StockDataStorage.cpp
// Purpose:     AsyncTaskManger
// Author:      songhuabiao
// Created:     2024-05-15-10.58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/StockDataStorage.h"
#include "spider/SpiderBasicInfoEastMoney.h"
#include "spider/SpiderShareListHexun.h"
#include "util/EasyLogger.h"
#include "util/FileTool.h"

// #include <windows.h>

using json = nlohmann::json;

StockDataStorage::StockDataStorage() : m_market_shares(std::vector<Share>()) {
    m_data_dir = FileTool::CurrentPath() + "data";
    m_path_share_brief = m_data_dir + DIRECTORY_SEPARATOR + "stock_brief.json";
}

StockDataStorage::~StockDataStorage() {}

void StockDataStorage::Init() {
    LoadStockAllShares();
}

void StockDataStorage::LoadStockAllShares() {
    // 检查本地的股票代号文件是否存在,如果存在，检查文件时间是否超过24小时，如果是，同步信息
    if (FileTool::IsFileExists(m_path_share_brief)) {
        LoadLocalJsonFile(m_path_share_brief, m_market_shares);
    } else {
        SpiderShareListHexun* spiderHexun = new SpiderShareListHexun(this);
        spiderHexun->Crawl();
        std::string json_shares = ToJson(m_market_shares);
        FileTool::SaveFile(m_path_share_brief, json_shares);
    }
    // 查询股票的曾用名
    SpiderBasicInfoEastMoney* spiderEastMoney = new SpiderBasicInfoEastMoney(this, true);
    spiderEastMoney->SetCrawlRange(0, 30);
    spiderEastMoney->Crawl();
    // PrintAllShares(m_market_shares);

    // Set console code page to UTF-8 so console known how to interpret string data
    // SetConsoleOutputCP(936);
}
std::string StockDataStorage::ToJson(std::vector<Share>& shares) {
    json result = json::array();
    for (Share share : shares) {
        json o = json::object();
        o["code"] = share.code;
        o["name"] = share.name;
        o["market"] = static_cast<int>(share.market);
        result.push_back(o);
    }
    std::string data = result.dump(4);
    return data;
}

bool StockDataStorage::LoadLocalJsonFile(std::string& path, std::vector<Share>& shares) {
    try {
        std::string json_data = FileTool::LoadFile(path);
        json arr = json::parse(json_data);
        for (auto& item : arr) {
            Share share;
            share.code = item["code"].template get<std::string>();
            share.name = item["name"].template get<std::string>();
            share.market = static_cast<Market>(item["market"].template get<int>());
            shares.push_back(share);
        }
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

bool StockDataStorage::CrawlStockHistoryName() {
    return true;
}

bool StockDataStorage::CrawlStockKlinesHistoryData() {
    return true;
}

std::vector<Share> StockDataStorage::GetMarketAllShares() {
    return m_market_shares;
}

void StockDataStorage::PrintAllShares(std::vector<Share>& all_shares) {
    std::string data = ToJson(all_shares);
    gLogger->log("%s", data.c_str());
}
