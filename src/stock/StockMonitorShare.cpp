///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/stock/StockMonitorShare.cpp
// Purpose:     iwealth stock monitor shares
// Author:      songhuabiao
// Created:     2024-07-11 20:30
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////
#include "StockMonitorShare.h"
#include <iostream>
#include "StockDataStorage.h"
#include "nlohmann/json.hpp"
#include "util/FileTool.h"

using json = nlohmann::json;

StockMonitorShare::StockMonitorShare(StockDataStorage* pStorage) : m_pStorage(pStorage) {
    Load();
}

StockMonitorShare::~StockMonitorShare() {
}

bool StockMonitorShare::AddShareToMonitorList(const std::string& share_code) {
    MonitorShare monitor_share;
    Share* pShare = m_pStorage->FindShare(share_code);
    if (pShare != nullptr) {
        monitor_share.pShare = pShare;
        monitor_share.expect_change_rate = 0.0;
        monitor_share.max_change_rate = 0.0;
        monitor_share.max_decrease_used_days = 0.0;
        monitor_share.moniter_date = 0.0;
        monitor_share.monitor_days = 0.0;
        monitor_share.monitor_price = 0.0;
        monitor_share.current_change_rate = 0.0;
        // 根据股票名称查找股票，如果找到，添加数据
        m_monitor_shares.push_back(monitor_share);
        return true;
    }

    return false;
}

bool StockMonitorShare::DelShareFromMonitorList(const std::string& share_code) {
    auto it = std::find_if(m_monitor_shares.begin(), m_monitor_shares.end(),
                           [&share_code](const MonitorShare& monitor_share) {
                               return monitor_share.pShare->code == share_code;
                           });
    if (it != m_monitor_shares.end()) {
        m_monitor_shares.erase(it);
        return true;
    }
    return false;
}

bool StockMonitorShare::CalcShareChangeRate(const MonitorShare& share, std::vector<uiKline>& klines) {
    // 倒序遍历股票列表 klines，找到监控日期的K线
    // 从前向后遍历K线，计算最低价,最低价耗时天数
    return false;
}

bool StockMonitorShare::Load() {
    if (!FileTool::IsFileExists(PATH_MONITOR_SHARE)) {
        return false;
    }
    std::string data = FileTool::LoadFile(PATH_MONITOR_SHARE);
}

bool StockMonitorShare::Save() {
}