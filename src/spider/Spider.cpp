///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/Spider.cpp
// Purpose:     Spider base class
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/Spider.h"
#include <mutex>
#include "curl/curl.h"
#include "stock/StockDataStorage.h"
#include "util/EasyLogger.h"

Spider::Spider(StockDataStorage* storage)
    : m_pStockStorage(storage),
      m_posStart(0),
      m_posEnd(0),
      m_concurrentMode(false),
      m_debug(false),
      m_timeStart(std::chrono::milliseconds(0)),
      m_timeEnd(std::chrono::milliseconds(0)),
      m_timeConsume(0) {
}

Spider::Spider(StockDataStorage* storage, bool concurrent)
    : m_pStockStorage(storage),
      m_posStart(0),
      m_posEnd(0),
      m_concurrentMode(concurrent),
      m_timeStart(std::chrono::milliseconds(0)),
      m_timeEnd(std::chrono::milliseconds(0)),
      m_timeConsume(0) {
}

Spider::~Spider() {
}

void Spider::SetCrawlRange(size_t start_pos, size_t end_pos) {
    m_posStart = start_pos;
    m_posEnd = end_pos;
}

std::string Spider::Fetch(const std::string& url, int http_version) {
    return HttpGet(url, http_version);
}

void Spider::Crawl() {
    m_timeStart = std::chrono::high_resolution_clock::now();
    DoCrawl();
    m_timeEnd = std::chrono::high_resolution_clock::now();
    m_timeConsume = std::chrono::duration_cast<std::chrono::milliseconds>(m_timeEnd - m_timeStart);
}

void Spider::DoCrawl() {
}

void Spider::ConcurrentResponseCallback(conn_t* /*conn*/) {
}

void Spider::Pause() {
}

void Spider::Stop() {
}

bool Spider::IsConcurrentMode() const {
    return this->m_concurrentMode;
}

std::string Spider::GetTimeConsumed() const {
    std::chrono::seconds::rep total_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_timeConsume).count();
    std::chrono::milliseconds::rep remaining_ms = m_timeConsume.count() % 1000;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << total_seconds << "." << remaining_ms << " s";
    return oss.str();
}

bool Spider::HasFinish() {
    return GetProgress() >= 1.0;
}

double Spider::GetProgress() {
    if (m_statisticsList.size() == 0) {
        return 0;
    }
    std::atomic<uint32_t> finished{0};
    std::atomic<uint32_t> total{0};
    for (RequestStatistics* pStatistics : m_statisticsList) {
        finished += pStatistics->success_requests + pStatistics->failed_requests;
        total += pStatistics->request_count;
    }
    double progress = static_cast<double>(finished) / total;
    return progress;
}

// 定时器每秒钟更新一次
void Spider::UpdateRequestStatistics() {
    for (RequestStatistics* pStatistics : m_statisticsList) {
        // 计算单位时间内接收的字节数
        pStatistics->real_time_speed = pStatistics->recv_bytes_cur - pStatistics->recv_bytes_last;
        pStatistics->recv_bytes_last.store(pStatistics->recv_bytes_cur.load());
        std::lock_guard<std::mutex> lock(pStatistics->mutex);
        pStatistics->speed_list.push_back(pStatistics->real_time_speed);  // 将每秒的下载字节数压入数组
    }
}

// 一个线程对应一个请求统计
RequestStatistics* Spider::NewRequestStatistics(uint32_t request_count, DataProvider provider) {
    RequestStatistics* pStatistics = new RequestStatistics();
    if (!pStatistics) {
        gLogger->log("[ConcurrentCrawl] allocate memory failed");
        return nullptr;
    }
    pStatistics->provider = provider;
    pStatistics->request_count += request_count;
    m_statisticsList.push_back(pStatistics);
    return pStatistics;
}

std::string Spider::UrlEncode(const std::string& decoded) {
    const auto encoded_value = curl_easy_escape(nullptr, decoded.c_str(), static_cast<int>(decoded.length()));
    std::string result(encoded_value);
    curl_free(encoded_value);
    return result;
}

std::string Spider::UrlDecode(const std::string& encoded) {
    int output_length;
    const auto decoded_value =
        curl_easy_unescape(nullptr, encoded.c_str(), static_cast<int>(encoded.length()), &output_length);
    std::string result(decoded_value, output_length);
    curl_free(decoded_value);
    return result;
}
