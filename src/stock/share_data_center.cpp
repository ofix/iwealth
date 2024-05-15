///////////////////////////////////////////////////////////////////////////////
// Name:           iwealth/core/ShareDataCenter.cpp
// Purpose:     AsyncTaskManger
// Author:      songhuabiao
// Created:     2024-05-15-10.58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/share_data_center.h"
#include "spider/share_list_spider_hexun.h"

using json = nlohmann::json;

ShareDataCenter::ShareDataCenter() {}

ShareDataCenter::~ShareDataCenter() {}

void ShareDataCenter::Init() {
    LoadStockAllShares();
}

void ShareDataCenter::LoadStockAllShares() {
    // 检查本地的股票代号文件是否存在,如果存在，检查文件时间是否超过24小时，如果是，同步信息
    ShareListSpiderHexun* spiderHexun = new ShareListSpiderHexun(this);
    spiderHexun->Run();
}

std::vector<Share> ShareDataCenter::GetMarketAllShares() {
    return m_market_shares;
}

void ShareDataCenter::PrintAllShares(std::vector<Share>& all_shares) {
    json result = json::array();
    for (auto item : all_shares) {
        json o = json::object();
        o["code"] = item.code;
        o["name"] = item.name;
        result.push_back(o);
    }
    std::string data = result.dump(4);
    std::cout << data << std::endl;
}
