///////////////////////////////////////////////////////////////////////////////
// Name:        src/formula/FormulaEma.cpp
// Purpose:     formula EMA for klines
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-14 12:39
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////
#include "formula/FormulaEma.h"

FormulaEma::FormulaEma(FormulaType type) : FormulaBase(type) {
}

FormulaEma::~FormulaEma() {
}

/**
 * @brief 计算指数移动平均线
 * @param klines k线数组
 * @param ema
 * @param n
 */
void FormulaEma::GetEmaPrice(std::vector<uiKline>& klines, std::vector<double>& ema_prices, int n) {
    double ema_price = klines[0].price_close;  // 初始化 EMA 为第一个收盘价
    double multiplier = 2.0 / (n + 1);         // 计算平滑因子
    ema_prices.push_back(ema_price);

    for (size_t i = 1; i < klines.size(); ++i) {
        ema_price = (klines[i].price_close - ema_price) * multiplier + ema_price;  // 计算 EMA
        ema_prices.push_back(ema_price);
    }
}