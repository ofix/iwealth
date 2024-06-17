///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/core/StockDataStorage.cpp
// Purpose:     iwealth stock shares' data storage
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/StockDataStorage.h"
#include "spider/SpiderBasicInfoEastMoney.h"
#include "spider/SpiderShareKline.h"
#include "spider/SpiderShareListHexun.h"
#include "util/EasyLogger.h"
#include "util/FileTool.h"

// #include <windows.h>

using json = nlohmann::json;

StockDataStorage::StockDataStorage() : m_market_shares(std::vector<Share>()) {
    m_data_dir = FileTool::CurrentPath() + "data";
    m_path_share_brief = m_data_dir + DIRECTORY_SEPARATOR + "stock_brief.json";
}

StockDataStorage::~StockDataStorage() {
}

void StockDataStorage::Init() {
    LoadStockAllShares();
}

Share* StockDataStorage::FindShare(std::string& share_code) {
    if (m_code_share_map.find(share_code) != m_code_share_map.end()) {
        return m_code_share_map[share_code];
    }
    return nullptr;
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
    // SpiderBasicInfoEastMoney* spiderEastMoney = new SpiderBasicInfoEastMoney(this,
    // true); spiderEastMoney->SetCrawlRange(0, 30); spiderEastMoney->Crawl();
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

bool StockDataStorage::SaveShareKLines(const KlineType kline_type) {
    std::string dir_path = FileTool::CurrentPath() + "data/";
    if (kline_type == KlineType::Day) {
        dir_path += "day";
        return SaveShareKlines(dir_path, m_day_klines_adjust);
    } else if (kline_type == KlineType::Week) {
        dir_path += "week";
        return SaveShareKlines(dir_path, m_week_klines_adjust);
    } else if (kline_type == KlineType::Month) {
        dir_path += "month";
        return SaveShareKlines(dir_path, m_month_klines_adjust);
    } else if (kline_type == KlineType::Year) {
        dir_path += "year";
        return SaveShareKlines(dir_path, m_year_klines_adjust);
    }
}

bool StockDataStorage::SaveShareKlines(const std::string& dir_path,
                                       const std::unordered_map<std::string, std::vector<uiKline>>& klines) {
    for (const auto& pair : klines) {
        std::string file_path = dir_path + pair.first + ".csv";
        std::vector<uiKline> vec = pair.second;
        std::string lines;
        for (const auto& kline : vec) {
            std::string line = "";
            line += kline.day + ",";
            line += std::to_string(kline.price_open) + ",";
            line += std::to_string(kline.price_close) + ",";
            line += std::to_string(kline.price_max) + ",";
            line += std::to_string(kline.price_min) + ",";
            line += std::to_string(kline.trade_volume) + ",";
            line += std::to_string(kline.trade_amount) + ",";
            line += std::to_string(kline.change_amount) + ",";
            line += std::to_string(kline.change_rate) + ",";
            line += std::to_string(kline.turnover_rate) + "\r\n";
            lines += line;
        }
        FileTool::SaveFile(file_path, lines);
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
