///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/stock/StockCompositeIndex.cpp
// Purpose:     iwealth market composite indexes
// Author:      songhuabiao
// Created:     2024-07-26 20:30
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////
#include "stock/StockCompositeIndex.h"
#include "stock/StockDataStorage.h"

StockCompositeIndex::StockCompositeIndex(StockDataStorage* pStorage) : m_pStorage(pStorage) {
}

StockCompositeIndex::~StockCompositeIndex() {
}
// 查询所有股指的名称
std::vector<std::string> StockCompositeIndex::QueryAllCompositeIndexNames() {
    std::vector<std::string> m_names;
    return m_names;
}
// 查询股指成分股信息
StockIndex* StockCompositeIndex::QueryCompositeIndexInfo(const std::string& index_name) {
    return nullptr;
}
// 查询股指最近一个交易日的分时图数据
RichResult StockCompositeIndex::QueryCompositeIndexMinuteKlines(const std::string& index_name,
                                                                std::vector<minuteKline>& minute_klines) {
    return Success();
}
// 查询股指的日K线/周K线/月K线/季K线/年K线数据
RichResult StockCompositeIndex::QueryCompositeIndexKlines(const KlineType kline_type,
                                                          const std::string& index_name,
                                                          std::vector<uiKline>& day_klines) {
    return Success();
}

// 添加股指信息到缓存
void StockCompositeIndex::AddCompositeIndexInfo(const std::string& index_name,
                                                const std::vector<std::string>& share_list) {
}
// 异步下载单个股指成分股数据
RichResult StockCompositeIndex::FetchCompositeIndexInfo(const std::string& index_name) {
}
// 异步下载所有股指成分股数据
RichResult StockCompositeIndex::FetchAllCompositeIndexInfo(const std::vector<std::string>& urls) {
}
// 异步下载单个股指行情数据
RichResult StockCompositeIndex::FetchCompositeIndexQuote(const std::string& index_name) {
}
// 异步下载所有股指行情数据
RichResult StockCompositeIndex::FetchAllCompositeIndexQuote(const std::vector<std::string>& urls) {
}
// 保存股指成分股数据
bool StockCompositeIndex::SaveCompositeIndexInfo(const std::string& index_name) {
}
// 加载股指成分股数据
bool StockCompositeIndex::LoadCompositeIndexInfo(const std::string& index_name, std::vector<StockIndex>& stock_index) {
}
// 保存股指日K线数据
bool StockCompositeIndex::SaveCompositeIndexDayKlines(const std::string& index_name) {
}
// 加载股指日K线数据
bool StockCompositeIndex::LoadCompositeIndexDayKlines(const std::string& index_name, std::vector<uiKline>& day_klines) {
}

std::vector<Share*> StockCompositeIndex::GetShareList(const std::string index_name) {
}

std::vector<Share*> StockCompositeIndex::GetShareList(const std::vector<std::string>& share_code_list) {
    std::vector<Share*> share_list;
    for (auto& share_code : share_code_list) {
        Share* pShare = m_pStorage->FindShare(share_code);
        if (pShare != nullptr) {
            share_list.emplace_back(pShare);
        }
    }
    return share_list;
}