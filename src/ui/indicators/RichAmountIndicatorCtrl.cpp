///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/indicators/RichAmountIndicatorCtrl.cpp
// Purpose:     日K线成交额附图指标
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-18 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichAmountIndicatorCtrl.h"
#include "ui/RichHelper.h"
#include "ui/components/RichKlineCtrl.h"
#include "util/Global.h"

RichAmountIndicatorCtrl::RichAmountIndicatorCtrl(RichKlineCtrl* pKlineCtrl, const wxPoint& pos, const wxSize& size)
    : RichIndicatorCtrl(pKlineCtrl, pos, size) {
}

RichAmountIndicatorCtrl::~RichAmountIndicatorCtrl() {
}

void RichAmountIndicatorCtrl::DrawTitleBar(wxDC* pDC) {
    m_colorTextArr = {{GetName(), INDICATOR_COLOR_1},
                      {GetYesterdayAmount(), INDICATOR_COLOR_2},
                      {GetTodayAmount(), INDICATOR_COLOR_3}};
    DrawColorTextArr(pDC, 2, m_y + 2, m_colorTextArr);
}

wxString RichAmountIndicatorCtrl::GetYesterdayAmount() {
    uiKline* pKline = m_pKlineCtrl->GetPreviousKline();
    return CN("昨: ") + RichUnit(pKline->amount);
}

wxString RichAmountIndicatorCtrl::GetTodayAmount() {
    uiKline* pKline = m_pKlineCtrl->GetCurrentKline();
    return CN("今: ") + RichUnit(pKline->amount);
}

wxString RichAmountIndicatorCtrl::GetWeekAmount() {
    return CN("周: ---");
}

void RichAmountIndicatorCtrl::Draw(wxDC* pDC) {
    if (m_visible) {
        DrawTitleBar(pDC);
        DrawAmountBar(pDC);
    }
}

wxString RichAmountIndicatorCtrl::GetName() {
    return CN("成交额");
}

wxString RichAmountIndicatorCtrl::GetFormulaName() {
    return "AMOUNT";
}

void RichAmountIndicatorCtrl::DrawAmountBar(wxDC* pDC) {
    if (m_pKlineCtrl->m_pKlines == nullptr || m_pKlineCtrl->m_pKlines->size() == 0) {
        return;
    }
    std::vector<uiKline>& klines = *(m_pKlineCtrl->m_pKlines);
    uiKlineRange& klineRng = m_pKlineCtrl->m_klineRng;
    double max_amount = GetMaxAmountInRange();

    long w = m_pKlineCtrl->m_klineInnerWidth;
    if (w < 1) {
        w = 1;
    }
    int body_y = m_y + m_titleHeight;
    std::vector<uiKline>::const_iterator it;
    int i = 0;
    for (it = klines.begin() + klineRng.begin; it <= klines.begin() + klineRng.end; ++it, ++i) {
        double x = (double)i * m_pKlineCtrl->m_klineWidth;
        double y = body_y + m_bodyHeight - it->amount * m_bodyHeight / max_amount;
        double h = it->amount / max_amount * m_bodyHeight;

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

void RichAmountIndicatorCtrl::DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h) {
    wxPen dash_pen(wxColor(200, 200, 200), 1, wxPENSTYLE_LONG_DASH);
    pDC->SetPen(dash_pen);
    pDC->DrawLine(centerX, centerY, centerX, h);  // 竖线
}

double RichAmountIndicatorCtrl::GetMaxAmountInRange() {
    double max = 0;
    std::vector<uiKline>::const_iterator it;
    std::vector<uiKline>& klines = *(m_pKlineCtrl->m_pKlines);
    uiKlineRange& klineRng = m_pKlineCtrl->m_klineRng;
    for (it = klines.begin() + klineRng.begin; it != klines.begin() + klineRng.end; ++it) {
        if (it->amount >= max) {
            max = it->amount;
        }
    }
    return max;
}
