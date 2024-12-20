///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/indicators/RichTurnoverRateIndicatorCtrl.cpp
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

RichTurnoverRateIndicatorCtrl::~RichTurnoverRateIndicatorCtrl() {
}

void RichTurnoverRateIndicatorCtrl::Draw(wxDC* pDC) {
    DrawTurnoverRateBar(pDC);
}

void RichTurnoverRateIndicatorCtrl::DrawTurnoverRateBar(wxDC* pDC) {
    if (m_pKlineCtrl->m_pKlines == nullptr || m_pKlineCtrl->m_pKlines->size() == 0) {
        return;
    }
    std::vector<uiKline>& klines = *(m_pKlineCtrl->m_pKlines);
    uiKlineRange& klineRng = m_pKlineCtrl->m_klineRng;
    double max_turnover_rate = GetMaxTurnoverRateInRange();

    long w = m_pKlineCtrl->m_klineInnerWidth;
    if (w < 1) {
        w = 1;
    }
    std::vector<uiKline>::const_iterator it;
    int i = 0;
    for (it = klines.begin() + klineRng.begin; it <= klines.begin() + klineRng.end; ++it, ++i) {
        double x = (double)i * m_pKlineCtrl->m_klineWidth;
        double y = m_y + m_height - it->turnover_rate * m_height / max_turnover_rate;
        double h = it->turnover_rate / max_turnover_rate * m_height;

        if (it->price_close >= it->price_open) {  // red bar
            pDC->SetPen(*wxRED_PEN);
            pDC->SetBrush(*wxBLACK_BRUSH);
        } else {  // green bar
            pDC->SetPen(wxPen(wxColor(84, 255, 255)));
            pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
        }
        if (h < 2) {
            h = 2;
        }
        pDC->DrawRectangle(x, y, w, h);
    }
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
        if (it->turnover_rate >= max) {
            max = it->turnover_rate;
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