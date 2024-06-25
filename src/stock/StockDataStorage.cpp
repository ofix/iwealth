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
#include "search/ChinesePinYin.h"
#include "spider/Spider.h"
#include "spider/SpiderShareBasicInfo.h"
#include "spider/SpiderShareCategory.h"
#include "spider/SpiderShareKline.h"
#include "spider/SpiderShareQuote.h"
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
    m_data_dir = FileTool::CurrentPath() + "data" + DIRECTORY_SEPARATOR;
    m_path_share_quote = m_data_dir + "stock_quote.json";
    m_path_category_province = m_data_dir + "stock_province.csv";
    m_path_category_industry = m_data_dir + "stock_industry.csv";
    m_path_category_concept = m_data_dir + "stock_concept.csv";
    m_path_share_names = m_data_dir + "stock_share_names.rch";
    Init();
}

StockDataStorage::~StockDataStorage() {
}

void StockDataStorage::Init() {
    if (!m_inited) {
        m_inited = true;
        LoadLocalFileShare();
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

void StockDataStorage::LoadLocalFileShare() {
    // 检查本地的股票代号文件是否存在
    if (FileTool::IsFileExists(m_path_share_quote)) {
        if (IsLocalQuoteDataExpired()) {
            FetchQuoteSync();  // 如果本地行情数据过时了，同步更新行情数据
        }
        LoadLocalFileQuote(m_path_share_quote, m_market_shares);  // 步骤1. 恢复 m_market_shares 数据
        HashShares();                                             // 步骤2 share_code -> Share* 映射
        LoadLocalFileCategory(ShareCategoryType::Province, m_path_category_province,
                              m_category_provinces);  // 步骤3 恢复 m_category_provinces
        LoadLocalFileCategory(ShareCategoryType::Industry, m_path_category_industry,
                              m_category_industries);  // 步骤4 恢复 m_category_industries
        m_fetch_quote_data_ok = true;                  // 立即显示行情列表标记
    } else {
        // 检查当前时间，如果时间是早晨09:00:00 ~ 09:29:59 之间，停止爬取，因为行情数据被重新初始化了，
        // 这个时候抓取的数据有可能是错误的, 可以显示其他的信息提示用户
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
            FetchQuoteIndustryProvince();  // 异步爬取当前行情/行业板块/地域板块信息
        }
    }
}

/// @brief 加载本地板块->股票映射文件
/// @param type 板块类别，行业板块/概念板块/地域板块
/// @param file_path 本地板块股票映射文件
/// @param share_categories ShareCategory 内存映射表
void StockDataStorage::LoadLocalFileCategory(ShareCategoryType type,
                                             const std::string& file_path,
                                             ShareCategory& share_categories) {
    std::string lines = FileTool::LoadFile(file_path);
    std::vector<std::string> categories = split(lines, "\n");
    for (auto& category : categories) {
        if (category.length() < 10) {
            break;
        }
        std::vector<std::string> map = split(category, ",");
        std::string category_name = map[0];
        std::vector<std::string> share_codes = split(map[1], "|");
        for (auto& share_code : share_codes) {
            Share* pShare = FindShare(share_code);
            if (pShare) {
                share_categories.Insert(category_name, pShare);
                if (type == ShareCategoryType::Province) {
                    pShare->province = category_name;
                } else if (type == ShareCategoryType::Industry) {
                    pShare->industry_name = category_name;
                }
            }
        }
    }
}

Spider* StockDataStorage::GetSpider(SpiderType type) {
    switch (type) {
        case SpiderType::Quote: {
            return new SpiderShareQuote(this);
        }
        case SpiderType::Province: {
            return new SpiderShareCategory(this, ShareCategoryType::Province);
        }
        case SpiderType::Industry: {
            return new SpiderShareCategory(this, ShareCategoryType::Industry);
        }
        case SpiderType::Concept: {
            return new SpiderShareCategory(this, ShareCategoryType::Concept);
        }
        case SpiderType::IndustryProvince: {
            return new SpiderShareCategory(this, ShareCategoryType::Industry | ShareCategoryType::Province);
        }
        default:
            return nullptr;
    }
}

void StockDataStorage::FetchQuoteIndustryProvince() {
    Spider* spiderQuote = GetSpider(SpiderType::Quote);
    spiderQuote->Crawl();  // 异步行情数据爬虫
    Spider* spiderCategory = GetSpider(SpiderType::IndustryProvince);
    spiderCategory->Crawl();  // 异步板块数据爬虫

    std::function<void(uint32_t, void*)> timer_cb = [=](uint32_t timer_id, void* args) {
        OnTimerFetchShareQuoteData(timer_id, args);
    };
    std::vector<Spider*>* pSpiders = new std::vector<Spider*>();
    pSpiders->emplace_back(spiderQuote);  // 和push_back相比，可以减少复杂对象的拷贝或移动构造。
    pSpiders->emplace_back(spiderCategory);

    Timer::SetInterval(1000, timer_cb, 0, static_cast<void*>(pSpiders));
}

void StockDataStorage::FetchQuoteSync() {
    SpiderShareQuote* spiderQuote = static_cast<SpiderShareQuote*>(GetSpider(SpiderType::Quote));
    spiderQuote->CrawlSync();                   // 同步爬取行情数据
    spiderQuote->SaveShareListToDataStorage();  // 保存股票列表
    HashShares();                               // share_code->Share* 映射
    SaveQuote();                                // 保存行情信息
}

void StockDataStorage::FetchBasicInfo() {
    SpiderShareBasicInfo* pSpiderBasicInfo = new SpiderShareBasicInfo(this, true);
    pSpiderBasicInfo->Crawl();
}

void StockDataStorage::FetchKline() {
}

void StockDataStorage::FetchFinancial() {
}

void StockDataStorage::FetchBusinessAnalysis() {
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
        result.push_back(o);
    }
    std::string data = result.dump(4);
    return data;
}

void StockDataStorage::SaveQuote() {
    std::string json_quote = DumpQuoteData(m_market_shares);  // 序列化股票
    FileTool::SaveFile(m_path_share_quote, json_quote);       // 保存股票列表到文件
}

/// @brief 保存股票板块信息(概念板块，地域板块，行业板块)
/// @param type
/// @param data
void StockDataStorage::SaveCategory(ShareCategoryType type,
                                    std::unordered_map<std::string, std::vector<std::string>>* categories) {
    std::string path = "";
    if (type == ShareCategoryType::Province) {
        path = m_path_category_province;
    } else if (type == ShareCategoryType::Industry) {
        path = m_path_category_industry;
    } else if (type == ShareCategoryType::Concept) {
        path = m_path_category_concept;
    }
    std::string lines = "";
    for (auto& category : *categories) {
        std::string line = category.first + ",";
        for (auto& share_code : category.second) {
            line += share_code + "|";
        }
        // 去掉最后多余的 "|"
        if (!line.empty()) {
            line.pop_back();
        }
        line += "\n";
        lines += line;
    }
    FileTool::SaveFile(path, lines);
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

/// @brief 根据用户输入的前缀字符返回对应的股票指针列表
std::vector<Share*> StockDataStorage::SearchShares(const std::string& prefix) {
    std::vector<std::string> share_codes = m_trie.listPrefixWith(prefix);
    std::vector<Share*> result;
    for (auto& share_code : share_codes) {
        Share* pShare = FindShare(share_code);
        if (pShare) {
            result.emplace_back(pShare);
        }
    }
    return result;
}

/// @brief 将股票名称插入Trie树，方便快速搜索股票
/// @param share_name 股票名称（曾用名)
/// @param share_code 股票代码
void StockDataStorage::InsertShareNameToTrie(const std::string& share_name, const std::string& share_code) {
    // 插入汉字
    m_trie.insert(share_name, share_code);
    // 插入拼音
    std::vector<std::string> share_name_pinyin = ChinesePinYin::GetFirstLetters(share_name);
    for (auto& pinyin : share_name_pinyin) {
        to_lower_case(pinyin);  // 全部统一转为小写再插入Trie
        m_trie.insert(pinyin, share_code);
    }
}

/// @brief 保存股票代号=>股票名称，股票曾用名，股票拼音到文件
/// @return
bool StockDataStorage::SaveShareNames() {
    std::unordered_map<std::string, std::vector<std::string>> names = m_trie.list();
    std::string lines = "";
    for (auto& share_names : names) {
        std::string line = share_names.first + ",";    // 股票代号
        for (auto& share_name : share_names.second) {  // 股票曾用名|拼音
            line += share_name + "|";
        }
        // 去掉最后多余的 "|"
        if (!line.empty()) {
            line.pop_back();
        }
        line += "\n";
        lines += line;
    }
    return FileTool::SaveFile(m_path_share_names, lines);
}

// 加载本地股票曾用名和名称
void StockDataStorage::LoadLocalFileShareNames() {
    std::string lines = FileTool::LoadFile(m_path_share_names);
    std::vector<std::string> shares = split(lines, "\n");
    for (auto& share : shares) {
        if (share.length() < 10) {
            break;
        }
        std::vector<std::string> map = split(share, ",");
        std::string share_code = map[0];
        std::vector<std::string> share_names = split(map[1], "|");
        for (auto& share_name : share_names) {
            InsertShareNameToTrie(share_name, share_code);
        }
    }
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

std::vector<Share> StockDataStorage::GetMarketAllShares() {
    return m_market_shares;
}

bool StockDataStorage::LoadLocalFileQuote(std::string& path, std::vector<Share>& shares) {
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
            shares.push_back(share);
        }
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
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

void StockDataStorage::OnTimeout(uint32_t /*timer_id*/, void* args) {
    char* opt = static_cast<char*>(args);
    if (strcmp(opt, "FetchQuote") == 0) {
        FetchQuoteIndustryProvince();  // 异步爬取行情数据
    }
}

// std::vector可以保证数据持续可访问，std::queue容器pop一次就无法工作了
void StockDataStorage::OnTimerFetchShareQuoteData(uint32_t timer_id, void* args) {
    std::vector<Spider*>* pSpiders = static_cast<std::vector<Spider*>*>(args);
    SpiderShareQuote* spiderQuote = dynamic_cast<SpiderShareQuote*>((*pSpiders)[0]);
    SpiderShareCategory* spiderCategory = dynamic_cast<SpiderShareCategory*>((*pSpiders)[1]);
    if (spiderQuote->HasFinish() && spiderCategory->HasFinish()) {
        spiderQuote->SaveShareListToDataStorage();       // 保存股票列表
        HashShares();                                    // share_code->Share* 映射
        spiderCategory->BuildShareCategoryProvinces();   // 填充股票省份信息
        spiderCategory->BuildShareCategoryIndustries();  // 填充股票行业信息
        // 保存行情数据/行业板块/地域板块数据到本地文件
        SaveQuote();
        SaveCategory(ShareCategoryType::Industry, spiderCategory->GetCategory(ShareCategoryType::Industry));
        SaveCategory(ShareCategoryType::Province, spiderCategory->GetCategory(ShareCategoryType::Province));
        SetFetchResultOk(FetchResult::QuoteData);  // 通知ui线程刷新页面
        delete spiderQuote;                        // 删除爬虫指针
        delete spiderCategory;                     // 删除爬虫指针
        delete pSpiders;                           // 删除容器指针
        pSpiders = nullptr;
        Timer::CancelTimer(timer_id);  // 取消定时器
        // 所有股票下载完后，开始爬取股票基本信息，包括(员工数/曾用名/主营业务/分红融资额/分红送转/总股本//增减持/官方网站/等等)
    } else {
        std::cout << "spiderShareList::progress: " << spiderQuote->GetProgress()
                  << ",spiderShareCategory::progress: " << spiderCategory->GetProgress() << std::endl;
    }
}

void StockDataStorage::PrintAllShares(std::vector<Share>& all_shares) {
    std::string data = DumpQuoteData(all_shares);
    gLogger->log("%s", data.c_str());
}
