///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/stock/ShareCategory.cpp
// Purpose:     common share category class
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, songhuabiao, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/ShareCategory.h"

bool ShareCategory::Insert(std::string& category, Share* share) {
    if (share == nullptr) {
        return false;
    }
    if (m_map.find(category) != m_map.end()) {
        m_map[category].push_back(share);
    } else {
        std::vector<Share*> shares;
        shares.push_back(share);
        m_map[category] = shares;
    }
    return true;
}

void ShareCategory::Clear() {
    m_map.clear();
}

std::vector<Share*> ShareCategory::Query(const std::string& category) {
    if (m_map.find(category) != m_map.end()) {
        return m_map[category];
    }
    return {};
}