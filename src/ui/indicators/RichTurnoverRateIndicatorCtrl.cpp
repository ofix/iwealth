///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/indicators/RichVolumnBarCtrl.cpp
// Purpose:     日K线换手率附图指标
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-18 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichTurnoverRateIndicatorCtrl.h"
#include "ui/RichHelper.h"
#include "ui/components/RichKlineCtrl.h"
#include "util/Global.h"

RichTurnoverRateIndicatorCtrl::RichTurnoverRateIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                                                             const wxPoint& pos,
                                                             const wxSize& size)
    : RichIndicatorCtrl(pKlineCtrl, pos, size) {
}

void RichTurnoverRateIndicatorCtrl::Draw(wxDC* pDC) {
}

std::string RichTurnoverRateIndicatorCtrl::GetName() {
    return "换手率";
}

std::string RichTurnoverRateIndicatorCtrl::GetFormulaName() {
    return "TURNOVER_RATE";
}

double RichTurnoverRateIndicatorCtrl::GetMaxTurnoverRateInRange() {
    double max = 0;
    std::vector<uiKline>::const_iterator it;
    std::vector<uiKline>& klines = *(m_pKlineCtrl->m_pKlines);
    uiKlineRange& klineRng = m_pKlineCtrl->m_klineRng;
    for (it = klines.begin() + klineRng.begin; it != klines.begin() + klineRng.end; ++it) {
        if (it->volume >= max) {
            max = it->volume;
        }
    }
    return max;
}

double RichTurnoverRateIndicatorCtrl::GetMaxTurnoverRate() {
    double max = 0;
    std::vector<minuteKline>::const_iterator it;
    std::vector<minuteKline>& klines = *(m_pKlineCtrl->m_pMinuteKlines);
    for (auto& kline : klines) {
        if (kline.volume >= max) {
            max = kline.volume;
        }
    }
    return max;
}

double RichTurnoverRateIndicatorCtrl::GetFiveDayMaxTurnoverRate() {
    double max = 0;
    std::vector<minuteKline>::const_iterator it;
    std::vector<minuteKline>& klines = *(m_pKlineCtrl->m_pFiveDayMinuteKlines);
    for (auto& kline : klines) {
        if (kline.volume >= max) {
            max = kline.volume;
        }
    }
    return max;
}