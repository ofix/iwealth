///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/stock/SpiderCompositeIndex.cpp
// Purpose:     iwealth market composite indexes
// Author:      songhuabiao
// Created:     2024-07-26 20:30
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderCompositeIndex.h"

SpiderCompositeIndex::SpiderCompositeIndex(StockDataStorage* pStorage) : Spider(pStorage) {
}

SpiderCompositeIndex::~SpiderCompositeIndex() {
}

void SpiderCompositeIndex::Crawl() {
}

bool SpiderCompositeIndex::CrawlSync() {
    return false;
}

std::vector<std::string> SpiderCompositeIndex::Parse(const std::string& response) {
    std::vector<std::string> result;
    return result;
}
