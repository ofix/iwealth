///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/spider/SpiderShareHistory.cpp
// Purpose:     crawl stock history
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "spider/SpiderShareHistory.h"
#include "stock/StockDataStorage.h"

SpiderShareHistory::SpiderShareHistory(StockDataStorage* storage) : Spider(storage) {}

SpiderShareHistory::~SpiderShareHistory() {}
