///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareKline.cpp
// Purpose:     crawl stock history
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareKline.h"
#include "net/ConcurrentRequest.h"
#include "spider/SpiderShareKlineProvider.h"
#include "spider/SpiderShareKlineProviderBaidu.h"
#include "spider/SpiderShareKlineProviderEastMoney.h"
#include "spider/SpiderShareKlineUrl.h"
#include "stock/Stock.h"
#include "stock/StockDataStorage.h"
#include "util/EasyLogger.h"
#include "util/Timer.h"

using json = nlohmann::json;

SpiderShareKline::SpiderShareKline(StockDataStorage* storage) : Spider(storage) {
}

SpiderShareKline::SpiderShareKline(StockDataStorage* storage, bool concurrent) : Spider(storage, concurrent) {
}

SpiderShareKline::~SpiderShareKline() {
}

void SpiderShareKline::Crawl(KlineType kline_type) {
    m_timeStart = std::chrono::high_resolution_clock::now();
    DoCrawl(kline_type);
}

SpiderShareKlineProvider* SpiderShareKline::GetKlineProvider(DataProvider provider) {
    SpiderShareKlineProvider* pProvider = nullptr;
    if (provider == DataProvider::FinanceBaidu) {
        pProvider = new SpiderShareKlineProviderBaidu();
    } else if (provider == DataProvider::EastMoney) {
        pProvider = new SpiderShareKlineProviderEastMoney();
    }
    return pProvider;
}

bool SpiderShareKline::CrawlMinuteKlineSync(Share* pShare, std::vector<minuteKline>& minute_klines) {
    std::string url = KLINE_URL_FINANCE_BAIDU_MINUTE(pShare->code);
    std::string response = HttpGet(url);
    SpiderShareKlineProvider* pProvider = GetKlineProvider(DataProvider::FinanceBaidu);
    if (pProvider == nullptr) {
        return false;
    }
    pProvider->ParseMinuteKline(response, minute_klines);
    delete pProvider;
    return true;
}

bool SpiderShareKline::CrawlFiveDayMinuteKlineSync(Share* pShare, std::vector<minuteKline>& five_day_minute_klines) {
    std::string url = KLINE_URL_FINANCE_BAIDU_FIVE_DAY(pShare->code, pShare->name);
    std::string response = HttpGet(url);
    SpiderShareKlineProvider* pProvider = GetKlineProvider(DataProvider::FinanceBaidu);
    if (pProvider == nullptr) {
        return false;
    }
    pProvider->ParseMinuteKline(response, five_day_minute_klines);
    delete pProvider;
    return true;
}

bool SpiderShareKline::CrawlDayKlineSync(Share* pShare, std::vector<uiKline>& day_klines) {
    return CrawlSync(pShare, KlineType::Day, day_klines);
}

bool SpiderShareKline::CrawlIncrementDayKlineSync(Share* pShare,
                                                  const std::string end_day,
                                                  const int ndays,
                                                  std::vector<uiKline>& day_klines) {
    SpiderShareKlineProvider* pProvider = GetKlineProvider(DataProvider::FinanceBaidu);
    if (pProvider == nullptr) {
        return false;
    }
    std::string url =
        pProvider->GetKlineUrl(KlineType::Day, pShare->code, pShare->name, pShare->market, end_day, ndays);
    std::string response = HttpGet(url);
    if (response.length() == 0) {  // 修正代理服务器导致403获取失败的问题
        return false;
    }
    pProvider->ParseDayKline(response, day_klines);
    delete pProvider;
    return true;
}

bool SpiderShareKline::CrawlSync(Share* pShare, KlineType kline_type, std::vector<uiKline>& period_klines) {
    std::vector<DataProvider> providers = {
        DataProvider::FinanceBaidu,
        DataProvider::EastMoney,
    };
    std::vector<DataProvider> data_providers;
    int iProvider = rand_int(0, providers.size() - 1);
    DataProvider provider = providers[1];  // 强制使用 EastMoney
    if (kline_type == KlineType::Minute || kline_type == KlineType::FiveDay) {
        provider = DataProvider::FinanceBaidu;
    }
    std::vector<CrawlRequest> requests = {};
    CrawlRequest request;
    SpiderShareKlineProvider* pProvider = GetKlineProvider(provider);
    request.url = pProvider->GetKlineUrl(kline_type, pShare->code, pShare->name, pShare->market);

    KlineCrawlExtra* pExtra = new KlineCrawlExtra();
    pExtra->pProvider = pProvider;
    pExtra->type = kline_type;
    pExtra->market = pShare->market;
    pExtra->share = pShare;
    request.pExtra = pExtra;
    requests.push_back(request);
    bool result = Start(requests, "sync_kline", 1);
    if (result) {
        MergeShareKlines(m_concurrent_day_klines_adjust, period_klines);
    }
    return result;
}

void SpiderShareKline::DoCrawl(KlineType kline_type) {
    std::vector<KlineCrawlTask> tasks = {
        {DataProvider::FinanceBaidu, 0.5},
        {DataProvider::EastMoney, 0.5},
    };
    if (this->IsConcurrentMode()) {
        ConurrentCrawl(tasks, kline_type);
    }
}

void SpiderShareKline::ConurrentCrawl(std::vector<KlineCrawlTask>& tasks, KlineType kline_type) {
    std::vector<Share> shares = m_pStockStorage->m_market_shares;
    size_t pos_start = 0, pos_end = 0;
    double priority = 0;
    for (size_t i = 0; i < tasks.size(); i++) {
        priority += tasks[i].priority;
        pos_end = std::round((shares.size() - 1) * priority);
        std::string provider_name = GetProviderName(tasks[i].provider);
        SpiderShareKlineProvider* pProvider = GetKlineProvider(tasks[i].provider);
        std::vector<CrawlRequest> requests = {};
        for (size_t j = pos_start; j <= pos_end; j++) {
            CrawlRequest request;
            request.url = pProvider->GetKlineUrl(kline_type, shares[j].code, shares[j].name, shares[j].market);
            KlineCrawlExtra* pExtra = new KlineCrawlExtra;
            pExtra->pProvider = pProvider;
            pExtra->type = kline_type;
            pExtra->market = shares[j].market;
            pExtra->share = &shares[j];
            request.pExtra = pExtra;
            requests.push_back(request);
        }
        pos_start = pos_end + 1;
        StartDetachThread(requests, GetProviderName(tasks[i].provider));  // 启动分离线程
    }
    // 启动定时器更新打印速度，并打印爬取进度,以下两种方法绑定成员函数都是OK的
    // std::function<void(uint32_t, void*)> timerCallback =
    //     std::bind(&SpiderShareKline::OnRequestTimer, this, std::placeholders::_1, std::placeholders::_2);
    std::function<void(uint32_t, void*)> timerCallback = [=](uint32_t timer_id, void* args) {
        OnRequestTimer(timer_id, args);
    };

    uint32_t timer_id = Timer::SetInterval(1000, timerCallback);
    if (timer_id == TIMER_CREATE_FAILED) {
        std::cout << "timer created failed!" << std::endl;
    }
}

void SpiderShareKline::OnRequestTimer(uint32_t timer_id, void* /*args*/) {
    this->UpdateRequestStatistics();
    std::cout << "kline sync progress: " << GetProgress() << std::endl;
    // 取消周期性定时任务
    if (this->IsConcurrentMode() && this->HasFinish()) {
        Timer::CancelTimer(timer_id);
    }
}

// 此成员函数通常在分离的线程中运行
void SpiderShareKline::ConcurrentResponseCallback(conn_t* conn) {
    KlineCrawlExtra* pExtra = static_cast<KlineCrawlExtra*>(conn->extra);
    std::string share_code = pExtra->share->code;
    SpiderShareKlineProvider* pProvider = pExtra->pProvider;

    std::vector<uiKline> multi_kline = {};
    pProvider->ParseDayKline(conn->response, multi_kline);
    if (multi_kline.size() > 0) {
        if (pProvider->GetType() == DataProvider::FinanceBaidu) {
            std::string end_date = multi_kline[0].day;
            conn->url = pProvider->GetKlineUrl(pExtra->type, share_code, pExtra->share->name, pExtra->market, end_date);
            conn->reuse = true;  // 需要复用
        } else {
            conn->reuse = false;
            DELETE_RESOURCE(conn->extra);  // (跨) 线程删除
        }
        this->m_concurrent_day_klines_adjust[share_code].push_back(multi_kline);
    } else {
        conn->reuse = false;  // 不需要复用
    }
}

// 合并K线，可以考虑移动语义
// void SpiderShareKline::MergeShareKlines(const KlineType kline_type) {
//     if (kline_type == KlineType::Day) {
//         MergeShareKlines(m_concurrent_day_klines_adjust, m_pStockStorage->m_day_klines_adjust);
//     } else if (kline_type == KlineType::Week) {
//         MergeShareKlines(m_concurrent_week_klines_adjust, m_pStockStorage->m_week_klines_adjust);
//     } else if (kline_type == KlineType::Month) {
//         MergeShareKlines(m_concurrent_month_klines_adjust, m_pStockStorage->m_month_klines_adjust);
//     } else if (kline_type == KlineType::Year) {
//         MergeShareKlines(m_concurrent_year_klines_adjust, m_pStockStorage->m_year_klines_adjust);
//     }
// }

void SpiderShareKline::MergeShareKlines(
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>>& concurrent_klines,
    std::unordered_map<std::string, std::vector<uiKline>>& target_klines) {
    for (std::unordered_map<std::string, std::vector<std::vector<uiKline>>>::const_iterator it =
             concurrent_klines.begin();
         it != concurrent_klines.end(); ++it) {
        const std::string& share_code = it->first;
        const std::vector<std::vector<uiKline>>& multi_klines = it->second;
        size_t kline_count = GetKlineCount(multi_klines);
        std::vector<uiKline> history_kline;
        history_kline.reserve(kline_count);  // 提升std::vector插入性能，避免频繁申请内存
        std::vector<std::vector<uiKline>>::const_reverse_iterator it_klines;
        for (it_klines = multi_klines.crbegin(); it_klines != multi_klines.crend(); ++it_klines) {
            const std::vector<uiKline>& kline = *it_klines;
            history_kline.insert(history_kline.end(), kline.begin(), kline.end());
        }
        // 对键和值进行相应操作
        target_klines.insert({share_code, history_kline});
    }
}

void SpiderShareKline::MergeShareKlines(
    std::unordered_map<std::string, std::vector<std::vector<uiKline>>>& concurrent_klines,
    std::vector<uiKline>& target_klines) {
    for (std::unordered_map<std::string, std::vector<std::vector<uiKline>>>::const_iterator it =
             concurrent_klines.begin();
         it != concurrent_klines.end(); ++it) {
        const std::string& share_code = it->first;
        const std::vector<std::vector<uiKline>>& multi_klines = it->second;
        size_t kline_count = GetKlineCount(multi_klines);
        target_klines.clear();
        target_klines.reserve(kline_count);  // 提升std::vector插入性能，避免频繁申请内存
        std::vector<std::vector<uiKline>>::const_reverse_iterator it_klines;
        for (it_klines = multi_klines.crbegin(); it_klines != multi_klines.crend(); ++it_klines) {
            const std::vector<uiKline>& kline = *it_klines;
            target_klines.insert(target_klines.end(), kline.begin(), kline.end());
        }
    }
}

// 获取所有历史向量K线的数量，避免std::vector容器反复分配内存带来的性能损失
size_t SpiderShareKline::GetKlineCount(const std::vector<std::vector<uiKline>>& multi_klines) {
    std::vector<std::vector<uiKline>>::const_iterator it;
    std::size_t count = 0;
    for (it = multi_klines.cbegin(); it != multi_klines.cend(); ++it) {
        count += (*it).size();
    }
    return count;
}

void SpiderShareKline::DumpKline(uiKline& kline) {
    std::cout << kline.day << "," << std::fixed << std::setprecision(2) << kline.price_now << "," << kline.price_max
              << "," << kline.price_min << "," << kline.price_open << "," << kline.price_close << ","
              << kline.change_rate << "," << kline.change_amount << "," << kline.volume << "," << kline.amount << ","
              << kline.turnover_rate;
    std::cout << std::endl;
}