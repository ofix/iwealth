///////////////////////////////////////////////////////////////////////////////
// Name:           iwealth/core/StockDataStorage.cpp
// Purpose:     AsyncTaskManger
// Author:      songhuabiao
// Created:     2024-05-15-10.58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/StockDataStorage.h"
#include "spider/SpiderShareListHexun.h"
#include "util/EasyLogger.h"
#include "util/FileTool.h"

// #include <windows.h>

using json = nlohmann::json;

StockDataStorage::StockDataStorage() {
    m_data_dir = FileTool::CurrentPath() + "/data";
    std::cout << "data_dir: " << m_data_dir << std::endl;
}

StockDataStorage::~StockDataStorage() {}

void StockDataStorage::Init() {
    LoadStockAllShares();
}

void StockDataStorage::LoadStockAllShares() {
    // 检查本地的股票代号文件是否存在,如果存在，检查文件时间是否超过24小时，如果是，同步信息
    SpiderShareListHexun* spiderHexun = new SpiderShareListHexun(this);
    spiderHexun->Crawl();
    PrintAllShares(m_market_shares);

    // Set console code page to UTF-8 so console known how to interpret string data
    // SetConsoleOutputCP(936);
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
    json result = json::array();
    for (auto item : all_shares) {
        json o = json::object();
        o["code"] = item.code;
        o["name"] = item.name;
        result.push_back(o);
    }
    std::string data = result.dump(4);
    gLogger->log("%s", data.c_str());
}
