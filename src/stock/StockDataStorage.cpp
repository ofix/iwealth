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
#include "spider/SpiderShareCategory.h"
#include "spider/SpiderShareKline.h"
#include "spider/SpiderShareListHexun.h"
#include "util/EasyLogger.h"
#include "util/FileTool.h"
#include "util/Timer.h"

// #include <windows.h>

using json = nlohmann::json;

StockDataStorage::StockDataStorage()
    : m_inited(false),
      m_fetch_quote_data_ok(false),
      m_fetch_klines_ok(false),
      m_fetch_financial_data_ok(false),
      m_fetch_business_analysis_ok(false),
      m_fetch_old_name_ok(false) {
    m_data_dir = FileTool::CurrentPath() + "data";
    m_path_share_brief = m_data_dir + DIRECTORY_SEPARATOR + "stock_brief.json";
    Init();
}

StockDataStorage::~StockDataStorage() {
}

void StockDataStorage::Init() {
    if (!m_inited) {
        m_inited = true;
        LoadStockAllShares();
    }
}

void StockDataStorage::LoadStockAllShares() {
    // 检查本地的股票代号文件是否存在,如果存在，检查文件时间是否超过24小时，如果是，同步信息
    if (FileTool::IsFileExists(m_path_share_brief)) {
        LoadLocalJsonFile(m_path_share_brief, m_market_shares);
    } else {
        AsyncFetchShareQuoteData();  // 异步爬取行情数据
    }
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

Share* StockDataStorage::FindShare(std::string& share_code) {
    if (m_code_share_hash.find(share_code) != m_code_share_hash.end()) {
        return m_code_share_hash[share_code];
    }
    return nullptr;
}

/// @brief 将股票代码映射到Share*指针，方便程序快速查找
void StockDataStorage::HashShares() {
    for (Share& share : m_market_shares) {
        m_code_share_hash[share.code] = &share;
    }
}

void StockDataStorage::SetFetchResultOk(FetchResult result) {
    if (result == FetchResult::QuoteData) {
        m_fetch_quote_data_ok = true;
        std::string json_shares = ToJson(m_market_shares);
        FileTool::SaveFile(m_path_share_brief, json_shares);
        if (m_market_shares.size() > 0) {
            HashShares();
        }
        // 行情数据爬完以后再进行其他数据的爬取
    } else if (result == FetchResult::Klines) {
        m_fetch_klines_ok = true;
    } else if (result == FetchResult::FinancialData) {
        m_fetch_financial_data_ok = true;
    } else if (result == FetchResult::BusinessAnalysis) {
        m_fetch_business_analysis_ok = true;
    } else if (result == FetchResult::OldNames) {
        m_fetch_old_name_ok = true;
    }
}

void StockDataStorage::AsyncFetchShareQuoteData() {
    SpiderShareListHexun* spiderShareList = new SpiderShareListHexun(this);
    spiderShareList->Crawl();  // 当前线程同步爬取市场行情数据
    SpiderShareCategory* spiderCategory = new SpiderShareCategory(this);
    spiderCategory->Crawl();

    uint32_t timer_id = Timer::SetInterval(1000, [=](uint32_t timer_id, void* args) {
        if (spiderShareList->HasFinish() && spiderCategory->HasFinish()) {
            this->SetFetchResultOk(FetchResult::QuoteData);
            Timer::CancelTimer(timer_id);  // 取消定时器
        } else {
            std::cout << "spiderShareList::progress: " << spiderShareList->GetProgress()
                      << ",spiderShareCategory::progress: " << spiderCategory->GetProgress() << std::endl;
        }
    });
}

void StockDataStorage::AsyncFetchShareOldNames() {
}

void StockDataStorage::AsyncFetchShareKlines() {
}

void StockDataStorage::AsyncFetchShareFinancialData() {
}

void StockDataStorage::AsyncFetchShareBusinessAnalysis() {
}

std::vector<Share> StockDataStorage::GetMarketAllShares() {
    return m_market_shares;
}

void StockDataStorage::PrintAllShares(std::vector<Share>& all_shares) {
    std::string data = ToJson(all_shares);
    gLogger->log("%s", data.c_str());
}
