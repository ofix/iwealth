///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/commponents/RichVolumnBarCtrl.cpp
// Purpose:     kline volume bar
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichVolumeBarCtrl.h"
#include "ui/components/RichKlineCtrl.h"

RichVolumeBarCtrl::RichVolumeBarCtrl(RichKlineCtrl* pKlineCtrl, const wxPoint& pos, const wxSize& size)
    : m_pKlineCtrl(pKlineCtrl) {
}

RichVolumeBarCtrl::~RichVolumeBarCtrl() {
}

void RichVolumeBarCtrl::OnPaint(wxDC* pDC) {
    if (m_pKlineCtrl->m_pKlines == nullptr || m_pKlineCtrl->m_pKlines->size() == 0) {
        return;
    }
    std::vector<uiKline>& klines = *(m_pKlineCtrl->m_pKlines);
    uiKlineRange& klineRng = m_pKlineCtrl->m_klineRng;
    double hVolumeBar = m_pKlineCtrl->m_height * 0.3;
    double yVolumeBar = m_pKlineCtrl->m_height * 0.7;
    // get canvas height
    double max_volume = GetMaxVolumeInRange();
    // calc single volume bar width
    long w = m_pKlineCtrl->m_klineInnerWidth;
    if (w < 1) {
        w = 1;
    }
    std::vector<uiKline>::const_iterator it;
    int i = 0;
    for (it = klines.begin() + klineRng.begin; it != klines.begin() + klineRng.end; ++it, ++i) {
        // make sure i must be double or result would be error!
        double x = (double)i * m_pKlineCtrl->m_klineWidth;
        double y = yVolumeBar + (1.0 - it->volume / max_volume) * hVolumeBar;
        double h = it->volume / max_volume * hVolumeBar;

        if (it->price_close >= it->price_open) {  // red bar
            pDC->SetPen(*wxRED_PEN);
            pDC->SetBrush(*wxBLACK_BRUSH);
        } else {  // green bar
            pDC->SetPen(*wxTRANSPARENT_PEN);
            pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
        }
        pDC->DrawRectangle(x, y, w, h);
    }
    if (m_pKlineCtrl->m_crossLine != NO_CROSS_LINE) {
        DrawCrossLine(pDC, m_pKlineCtrl->m_crossLinePt.x, m_pKlineCtrl->m_crossLinePt.y + m_pKlineCtrl->m_height * 0.7,
                      m_pKlineCtrl->m_width, m_pKlineCtrl->m_height * 0.3);
    }
}

void RichVolumeBarCtrl::DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h) {
    wxPen dash_pen(wxColor(200, 200, 200), 1, wxPENSTYLE_LONG_DASH);
    pDC->SetPen(dash_pen);
    pDC->DrawLine(centerX, centerY, centerX, h);  // 竖线
}

double RichVolumeBarCtrl::GetMaxVolumeInRange() {
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
