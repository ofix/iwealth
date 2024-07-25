///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/stock/StockFavoriteShare.cpp
// Purpose:     iwealth stock favorate shares
// Author:      songhuabiao
// Created:     2024-07-11 20:30
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "UserFavoriteShare.h"
#include <fstream>
#include <iostream>
#include "Stock.h"
#include "StockDataStorage.h"
#include "nlohmann/json.hpp"
#include "util/DateTime.h"
#include "util/FileTool.h"

using json = nlohmann::json;

UserFavoriteShare::UserFavoriteShare(StockDataStorage* pStorage) : m_pStorage(pStorage) {
    Load();  // 先加载
    if (!HasFavoriteGroup()) {
        CreateFavoriteShareGroup(DEFAULT_GROUP_NAME);
    }
}

UserFavoriteShare::~UserFavoriteShare() {
}

bool UserFavoriteShare::CreateFavoriteShareGroup(const std::string& favorite_group_name) {
    for (const auto& group : m_favoriteShareGroups) {
        if (group.name == favorite_group_name) {
            return false;
        }
    }
    FavoriteShareGroup new_group;
    new_group.name = favorite_group_name;
    new_group.shares = {};
    m_favoriteShareGroups.push_back(new_group);
    return true;
}

bool UserFavoriteShare::RemoveFavoriteShareGroup(const std::string& favorite_group_name) {
    auto it = std::find_if(m_favoriteShareGroups.begin(), m_favoriteShareGroups.end(),
                           [&favorite_group_name](const FavoriteShareGroup& group) {
                               return group.name == favorite_group_name;
                           });
    if (it != m_favoriteShareGroups.end()) {
        m_favoriteShareGroups.erase(it);
        return true;
    }
    return false;
}

bool UserFavoriteShare::AddShareToFavoriteGroup(const std::string& share_code,
                                                std::string& favorite_group_name,
                                                std::string favorite_date) {
    if (favorite_group_name == "") {
        favorite_group_name = DEFAULT_GROUP_NAME;
    }
    auto groupIt = std::find_if(m_favoriteShareGroups.begin(), m_favoriteShareGroups.end(),
                                [&favorite_group_name](const FavoriteShareGroup& group) {
                                    return group.name == favorite_group_name;
                                });
    if (groupIt == m_favoriteShareGroups.end()) {
        return false;
    }

    // 根据股票代码查找股票
    Share* pShare = m_pStorage->FindShare(share_code);
    if (pShare) {
        FavoriteShare favorite_share;
        favorite_share.pShare = pShare;                                                         // 初始化指针
        favorite_share.favoriate_date = favorite_date == "" ? now("%Y-%m-%d") : favorite_date;  // 初始化日期
        favorite_share.add_price = 0.0;                                                         // 初始化价格
        favorite_share.total_change_rate = 0.0;                                                 // 初始化涨跌幅
        favorite_share.recent_5_days_change_rate = 0.0;  // 初始化 5 日涨跌幅
        favorite_share.recent_month_change_rate = 0.0;   // 初始化 1 个月涨跌幅
        favorite_share.recent_year_change_rate = 0.0;    // 初始化今年以来涨跌幅
        groupIt->shares.push_back(favorite_share);
        return true;
    }
    return false;
}

bool UserFavoriteShare::DelShareFromFavoriteGroup(const std::string& share_code, std::string& favorite_group_name) {
    auto groupIt = std::find_if(m_favoriteShareGroups.begin(), m_favoriteShareGroups.end(),
                                [&favorite_group_name](const FavoriteShareGroup& group) {
                                    return group.name == favorite_group_name;
                                });
    if (groupIt == m_favoriteShareGroups.end()) {
        return false;
    }
    auto shareIt =
        std::find_if(groupIt->shares.begin(), groupIt->shares.end(), [&share_code](const FavoriteShare& share) {
            return share.pShare->code == share_code;
        });
    if (shareIt == groupIt->shares.end()) {
        return false;
    }
    groupIt->shares.erase(shareIt);
    return true;
}

// 加载用户自选股文件
bool UserFavoriteShare::Load() {
    if (!FileTool::IsFileExists(PATH_USER_FAVORITE_SHARE)) {
        return false;
    }
    std::string data = FileTool::LoadFile(PATH_USER_FAVORITE_SHARE);
    json favorite_groups = json::parse(data);

    /************************************************
    [
        {
            "FavoriteGroupName":"默认",
            "Shares":[{
                    "Code":"600517",
                    "Day":"2024-07-11",
                },{
                    "Code":"300182",
                    "Day":"2024-07-11",
                }]
        }
    ]
     ************************************************/
    for (json::iterator it = favorite_groups.begin(); it != favorite_groups.end(); ++it) {
        std::string favorite_group_name = (*it)["FavoriteGroupName"];
        json shares = (*it)["Shares"];
        CreateFavoriteShareGroup(favorite_group_name);
        for (json::iterator it_s = shares.begin(); it_s != shares.end(); ++it_s) {
            std::string share_code = (*it_s)["Code"].template get<std::string>();
            std::string favorite_day = (*it_s)["Day"].template get<std::string>();
            AddShareToFavoriteGroup(share_code, favorite_group_name, favorite_day);
        }
    }
    return true;
}

bool UserFavoriteShare::Save() {
    json result = json::array();
    for (auto& group : m_favoriteShareGroups) {
        json o = json::object();
        json favorite_shares = json::array();
        o["FavoriteGroupName"] = group.name;  // 分组名称
        for (auto& share : group.shares) {
            json favorite_share = json::object();
            favorite_share["Code"] = share.pShare->code;
            favorite_share["Day"] = share.favoriate_date;
            favorite_shares.push_back(favorite_share);
        }
        o["Shares"] = favorite_shares;
        result.push_back(o);
    }
    std::string content = result.dump(4);
    return FileTool::SaveFile(PATH_USER_FAVORITE_SHARE, content);
}

bool UserFavoriteShare::HasFavoriteGroup() {
    return !m_favoriteShareGroups.empty();
}

bool UserFavoriteShare::CalcShareChangeRate(FavoriteShare& favorite_share, std::vector<uiKline>* klines) {
    double start_price = 0.0;
    double end_price = 0.0;
    int start_idx = -1, end_idx = -1;

    for (int i = klines->size() - 1; i >= 0; i--) {
        const auto& kline = (*klines)[i];
        if (kline.day >= favorite_share.favoriate_date && is_trade_day(kline.day)) {
            if (start_idx == -1) {
                start_idx = i;
                start_price = kline.price_close;
            }
            end_idx = i;
            end_price = kline.price_close;
        }
    }

    if (start_idx == -1 || end_idx == -1) {
        return false;
    }
    favorite_share.total_change_rate = (end_price - start_price) / start_price * 100.0;
    return true;
}

// 计算用户所有自选股分组的涨跌幅
bool UserFavoriteShare::CalcAllFavoriteSharesChangeRate() {
    // 获取当前日期
    // time_t now = time(0);
    // struct tm* localNow = localtime(&now);
    // std::string currentDate = std::to_string(localNow->tm_year + 1900) + "-" + std::to_string(localNow->tm_mon + 1) +
    //                           "-" + std::to_string(localNow->tm_mday);

    // for (auto& group : m_favoriteShareGroups) {
    //     for (auto& share : group.shares) {
    //         std::vector<uiKline>* klines = m_pStorage->GetShareKlines(share.pShare->code, KlineType::Day);
    //         if (klines != nullptr) {
    //             if (!CalcShareChangeRate(share, klines)) {
    //                 std::cerr << "计算涨跌幅失败" << std::endl;
    //             }
    //         }

    //         // 计算最近 5 日的涨跌幅
    //         std::string fiveDaysAgo = currentDate;
    //         struct tm timeInfo;
    //         if (_strptime(fiveDaysAgo.c_str(), "%Y-%m-%d", &timeInfo)) {
    //             timeInfo.tm_mday -= 5;
    //             mktime(&timeInfo);
    //             fiveDaysAgo = std::to_string(timeInfo.tm_year + 1900) + "-" + std::to_string(timeInfo.tm_mon + 1) +
    //                           "-" + std::to_string(timeInfo.tm_mday);
    //         }

    //         double recent5DaysChangeRate = 0.0;
    //         int start_idx = -1, end_idx = -1;
    //         for (int i = klines.size() - 1; i >= 0; i--) {
    //             const auto& kline = klines[i];
    //             if (kline.day >= fiveDaysAgo && kline.day <= currentDate && is_trade_day(kline.day)) {
    //                 if (start_idx == -1) {
    //                     start_idx = i;
    //                     start_price = kline.price_close;
    //                 }
    //                 end_idx = i;
    //                 end_price = kline.price_close;
    //             }
    //         }

    //         if (start_idx != -1 && end_idx != -1) {
    //             share.recent_5_days_change_rate = (end_price - start_price) / start_price * 100.0;
    //         }

    //         // 计算最近 1 个月的涨跌幅
    //         std::string oneMonthAgo = currentDate;
    //         struct tm timeInfo2;
    //         if (_strptime(oneMonthAgo.c_str(), "%Y-%m-%d", &timeInfo2)) {
    //             timeInfo2.tm_mon -= 1;
    //             mktime(&timeInfo2);
    //             oneMonthAgo = std::to_string(timeInfo2.tm_year + 1900) + "-" + std::to_string(timeInfo2.tm_mon + 1) +
    //                           "-" + std::to_string(timeInfo2.tm_mday);
    //         }

    //         double recentMonthChangeRate = 0.0;
    //         start_idx = -1;
    //         end_idx = -1;
    //         for (int i = klines.size() - 1; i >= 0; i--) {
    //             const auto& kline = klines[i];
    //             if (kline.day >= oneMonthAgo && kline.day <= currentDate && is_trade_day(kline.day)) {
    //                 if (start_idx == -1) {
    //                     start_idx = i;
    //                     start_price = kline.price_close;
    //                 }
    //                 end_idx = i;
    //                 end_price = kline.price_close;
    //             }
    //         }

    //         if (start_idx != -1 && end_idx != -1) {
    //             share.recent_month_change_rate = (end_price - start_price) / start_price * 100.0;
    //         }

    //         // 计算今年以来的涨跌幅
    //         std::string startOfYear = std::to_string(localNow->tm_year + 1900) + "-01-01";

    //         double recentYearChangeRate = 0.0;
    //         start_idx = -1;
    //         end_idx = -1;
    //         for (int i = klines.size() - 1; i >= 0; i--) {
    //             const auto& kline = klines[i];
    //             if (kline.day >= startOfYear && kline.day <= currentDate && is_trade_day(kline.day)) {
    //                 if (start_idx == -1) {
    //                     start_idx = i;
    //                     start_price = kline.price_close;
    //                 }
    //                 end_idx = i;
    //                 end_price = kline.price_close;
    //             }
    //         }

    //         if (start_idx != -1 && end_idx != -1) {
    //             share.recent_year_change_rate = (end_price - start_price) / start_price * 100.0;
    //         }
    //     }
    // }
}