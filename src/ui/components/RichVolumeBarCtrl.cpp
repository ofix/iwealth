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
    if (m_pKlineCtrl->m_mode == KlineType::Day || m_pKlineCtrl->m_mode == KlineType::Week ||
        m_pKlineCtrl->m_mode == KlineType::Month || m_pKlineCtrl->m_mode == KlineType::Quarter ||
        m_pKlineCtrl->m_mode == KlineType::Year) {
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
        for (it = klines.begin() + klineRng.begin; it <= klines.begin() + klineRng.end; ++it, ++i) {
            // make sure i must be double or result would be error!
            double x = (double)i * m_pKlineCtrl->m_klineWidth;
            double y = yVolumeBar + (1.0 - it->volume / max_volume) * hVolumeBar;
            double h = it->volume / max_volume * hVolumeBar;

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
        if (m_pKlineCtrl->m_crossLine != NO_CROSS_LINE) {
            DrawCrossLine(pDC, m_pKlineCtrl->m_crossLinePt.x,
                          m_pKlineCtrl->m_crossLinePt.y + m_pKlineCtrl->m_height * 0.7, m_pKlineCtrl->m_width,
                          m_pKlineCtrl->m_height * 0.3);
        }
    } else if (m_pKlineCtrl->m_mode == KlineType::Minute || m_pKlineCtrl->m_mode == KlineType::FiveDay) {
        if (m_pKlineCtrl->m_pMinuteKlines == nullptr || m_pKlineCtrl->m_pMinuteKlines->size() == 0) {
            return;
        }
        std::vector<minuteKline>& klines = *(m_pKlineCtrl->m_pMinuteKlines);
        double hVolumeBar = m_pKlineCtrl->m_height * 0.3;
        double yVolumeBar = m_pKlineCtrl->m_height * 0.7;
        double max_volume = GetMaxVolume();
        double w =
            static_cast<double>((m_pKlineCtrl->m_width - m_pKlineCtrl->m_paddingLeft - m_pKlineCtrl->m_paddingRight)) /
            klines.size();
        wxPen greenPen(wxColor(84, 255, 255));
        wxPen normalPen(wxColor(215, 215, 215));
        for (size_t i = 1; i < klines.size(); i++) {
            minuteKline kline = klines.at(i);
            double x = (double)(i * w) + m_pKlineCtrl->m_paddingLeft;
            double y = yVolumeBar + (1.0 - kline.volume / max_volume) * hVolumeBar;
            double h = kline.volume / max_volume * hVolumeBar;
            double prevPrice = klines.at(i - 1).price;
            if (kline.price > prevPrice) {
                pDC->SetPen(*wxRED_PEN);
            } else if (kline.price == prevPrice) {
                pDC->SetPen(normalPen);
            } else {
                pDC->SetPen(greenPen);
            }
            pDC->DrawLine(x, y, x, y + h);
        }
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

double RichVolumeBarCtrl::GetMaxVolume() {
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
