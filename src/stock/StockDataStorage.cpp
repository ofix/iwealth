///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/core/StockDataStorage.cpp
// Purpose:     iwealth stock shares' data storage
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/StockDataStorage.h"
#include <wx/wx.h>
#include "spider/Spider.h"
#include "spider/SpiderBasicInfoEastMoney.h"
#include "spider/SpiderShareCategory.h"
#include "spider/SpiderShareKline.h"
#include "spider/SpiderShareListHexun.h"
#include "ui/RichApplication.h"
#include "util/DateTime.h"
#include "util/EasyLogger.h"
#include "util/FileTool.h"
#include "util/Timer.h"

using json = nlohmann::json;

StockDataStorage::StockDataStorage()
    : m_inited(false),
      m_fetch_quote_data_ok(false),
      m_fetch_klines_ok(false),
      m_fetch_financial_data_ok(false),
      m_fetch_business_analysis_ok(false),
      m_fetch_old_name_ok(false) {
    m_data_dir = FileTool::CurrentPath() + "data";
    m_path_share_quote = m_data_dir + DIRECTORY_SEPARATOR + "stock_quote.json";
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
    // 检查本地的股票代号文件是否存在
    if (FileTool::IsFileExists(m_path_share_quote) && !IsLocalQuoteDataExpired()) {
        LoadLocalQuoteData(m_path_share_quote, m_market_shares);  // 步骤1. 恢复 m_market_shares 数据
        HashShares();                                             // 步骤2 share_code -> Share* 映射
        RestoreShareCategoryProvince();                           // 步骤3 恢复 m_category_provinces
        RestoreShareCategoryIndustry();                           // 步骤4 恢复 m_category_industries
        m_fetch_quote_data_ok = true;                             // 立即显示行情列表
    } else {
        // 检查当前时间，如果时间是早晨09:00:00 ~ 09:29:59 之间，停止爬取，因为行情数据被重新初始化了
        if (between_time_period("09:00", "09:29")) {
            std::string current_time = now("%Y-%m-%d %H:%M:%S");
            std::string future_time = now("%Y-%m-%d ") + "09:30:00";
            int wait_seconds = diff_seconds(current_time, future_time);
            std::function<void(uint32_t, void*)> timer_cb = [=](uint32_t timer_id, void* args) {
                OnTimeout(timer_id, args);
            };
            const char* opt = "FetchQuote";
            Timer::SetTimeout(wait_seconds * 1000, timer_cb, static_cast<void*>(const_cast<char*>(opt)));
        } else {
            AsyncFetchShareQuoteData();  // 异步爬取行情数据
        }
    }
}

/// @brief 检查本地行情数据是否过期
bool StockDataStorage::IsLocalQuoteDataExpired() {
    // 获取最近交易日
    std::string nearest_trade_day = get_nearest_trade_day();
    std::string nearest_trade_close_time = nearest_trade_day + " 15:00:00";
    // 获取本地行情数据文件修改时间
    std::string local_quote_file_modified_time = FileTool::GetFileModifiedTime(m_path_share_quote);
    // 检查文件修改时间是否 > 最近交易日收盘时间
    if (compare_time(local_quote_file_modified_time, nearest_trade_close_time) > 0) {
        return false;
    }
    return true;
}

// 从本地行情数据中恢复 省份->[股票1,股票2] province -> [Share*,Share*]的映射
void StockDataStorage::RestoreShareCategoryProvince() {
    m_category_provinces.Clear();
    for (Share& share : m_market_shares) {
        m_category_provinces.Insert(share.province, &share);
    }
}

// 从本地行情数据中恢复 行业->[股票1,股票2] industry_name -> [Share*,Share*]的映射
void StockDataStorage::RestoreShareCategoryIndustry() {
    m_category_industries.Clear();
    for (Share& share : m_market_shares) {
        m_category_industries.Insert(share.industry_name, &share);
    }
}

// 从本地行情数据中恢复 概念->[股票1,股票2] concept -> [Share*,Share*]的映射
void StockDataStorage::RestoreShareCategoryConcepts() {
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

void StockDataStorage::OnTimeout(uint32_t /*timer_id*/, void* args) {
    char* opt = static_cast<char*>(args);
    if (strcmp(opt, "FetchQuote") == 0) {
        AsyncFetchShareQuoteData();  // 异步爬取行情数据
    }
}

std::string StockDataStorage::DumpQuoteData(std::vector<Share>& shares) {
    json result = json::array();
    for (Share share : shares) {
        json o = json::object();
        o["code"] = share.code;                                    // 股票代码
        o["name"] = share.name;                                    // 股票名称
        o["market"] = static_cast<int>(share.market);              // 股票市场
        o["price_yesterday_close"] = share.price_yesterday_close;  // 昨天收盘价
        o["price_now"] = share.price_now;                          // 当前价
        o["price_min"] = share.price_min;                          // 最低价
        o["price_max"] = share.price_max;                          // 最高价
        o["price_open"] = share.price_open;                        // 开盘价
        o["price_close"] = share.price_close;                      // 收盘价
        o["price_amplitude"] = share.price_amplitude;              // 股价振幅
        o["change_amount"] = share.change_amount;                  // 涨跌额
        o["change_rate"] = share.change_rate;                      // 涨跌幅度
        o["volume"] = share.volume;                                // 成交量
        o["amount"] = share.amount;                                // 成交额
        o["turnover_rate"] = share.turnover_rate;                  // 换手率
        o["qrr"] = share.qrr;                                      // 量比
        o["industry_name"] = share.industry_name;                  // 行业名称
        o["province"] = share.province;                            // 所属省份
        result.push_back(o);
    }
    std::string data = result.dump(4);
    return data;
}

bool StockDataStorage::LoadLocalQuoteData(std::string& path, std::vector<Share>& shares) {
    try {
        std::string json_data = FileTool::LoadFile(path);
        json arr = json::parse(json_data);
        for (auto& item : arr) {
            Share share;
            share.code = item["code"].template get<std::string>();                               // 股票代码
            share.name = item["name"].template get<std::string>();                               // 股票名称
            share.market = static_cast<Market>(item["market"].template get<int>());              // 股票市场
            share.price_yesterday_close = item["price_yesterday_close"].template get<double>();  // 昨天收盘价
            share.price_now = item["price_now"].template get<double>();                          // 当前价
            share.price_min = item["price_min"].template get<double>();                          // 最低价
            share.price_max = item["price_max"].template get<double>();                          // 最高价
            share.price_open = item["price_open"].template get<double>();                        // 开盘价
            share.price_close = item["price_close"].template get<double>();                      // 收盘价
            share.price_amplitude = item["price_amplitude"].template get<double>();              // 股价振幅
            share.change_amount = item["change_amount"].template get<double>();                  // 涨跌额
            share.change_rate = item["change_rate"].template get<double>();                      // 涨跌幅度
            share.volume = item["volume"].template get<uint64_t>();                              // 成交量
            share.amount = item["amount"].template get<uint64_t>();                              // 成交额
            share.turnover_rate = item["turnover_rate"].template get<double>();                  // 换手率
            share.qrr = item["qrr"].template get<double>();                                      // 量比
            share.industry_name = item["industry_name"].template get<std::string>();             // 行业名称
            share.province = item["province"].template get<std::string>();                       // 所属省份
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
    return false;
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

void StockDataStorage::SetFetchResultOk(FetchResult result) {
    if (result == FetchResult::QuoteData) {
        m_fetch_quote_data_ok = true;
#ifdef IWEALTH
        // 发送消息给UI主线程显示行情数据]
        wxThreadEvent event(wxEVT_COMMAND_THREAD, ID_QUOTE_DATA_READY);
        event.SetString("Quote");
        static_cast<RichApplication*>(wxTheApp)->GetMainFrame()->GetEventHandler()->AddPendingEvent(event);
#endif
        // 如果需要，行情数据爬完以后再进行其他数据的爬取
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
    spiderCategory->Crawl(ShareCategoryType::Industry | ShareCategoryType::Province);

    std::function<void(uint32_t, void*)> timer_cb = [=](uint32_t timer_id, void* args) {
        OnTimerFetchShareQuoteData(timer_id, args);
    };
    std::vector<Spider*>* pSpiders = new std::vector<Spider*>();
    pSpiders->push_back(spiderShareList);
    pSpiders->push_back(spiderCategory);
    Timer::SetInterval(1000, timer_cb, 0, static_cast<void*>(pSpiders));
}

// std::vector可以保证数据持续可访问，std::queue容器pop一次就无法工作了
void StockDataStorage::OnTimerFetchShareQuoteData(uint32_t timer_id, void* args) {
    std::vector<Spider*>* pSpiders = static_cast<std::vector<Spider*>*>(args);
    SpiderShareListHexun* spiderQuote = static_cast<SpiderShareListHexun*>((*pSpiders)[0]);
    SpiderShareCategory* spiderCategory = static_cast<SpiderShareCategory*>((*pSpiders)[1]);
    if (spiderQuote->HasFinish() && spiderCategory->HasFinish()) {
        spiderQuote->SaveShareListToDataStorage();                 // 保存股票列表
        HashShares();                                              // share_code->Share* 映射
        spiderCategory->BuildShareCategoryProvinces();             // 填充股票省份信息
        spiderCategory->BuildShareCategoryIndustries();            // 填充股票行业信息
        std::string json_shares = DumpQuoteData(m_market_shares);  // 序列化股票
        FileTool::SaveFile(m_path_share_quote, json_shares);       // 保存股票列表到文件
        SetFetchResultOk(FetchResult::QuoteData);                  // 通知ui线程刷新页面
        delete spiderQuote;                                        // 删除爬虫指针
        delete spiderCategory;                                     // 删除爬虫指针
        delete pSpiders;                                           // 删除容器指针
        pSpiders = nullptr;
        Timer::CancelTimer(timer_id);  // 取消定时器
        // 所有股票下载完后，开始爬取股票基本信息，包括(员工数/曾用名/主营业务/分红融资额/分红送转/总股本//增减持/官方网站/等等)
    } else {
        std::cout << "spiderShareList::progress: " << spiderQuote->GetProgress()
                  << ",spiderShareCategory::progress: " << spiderCategory->GetProgress() << std::endl;
    }
}

void StockDataStorage::AsyncFetchShareBasicInfo() {
    SpiderBasicInfoEastMoney* pSpiderBasicInfo = new SpiderBasicInfoEastMoney(this, true);
    pSpiderBasicInfo->Crawl();
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
    std::string data = DumpQuoteData(all_shares);
    gLogger->log("%s", data.c_str());
}
