///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/indicators/RichVolumnIndicatorCtrl.cpp
// Purpose:     日K线成交量附图指标
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-18 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichVolumeIndicatorCtrl.h"
#include "ui/RichHelper.h"
#include "ui/components/RichKlineCtrl.h"
#include "util/Global.h"

RichVolumeIndicatorCtrl::RichVolumeIndicatorCtrl(RichKlineCtrl* pKlineCtrl, const wxPoint& pos, const wxSize& size)
    : RichIndicatorCtrl(pKlineCtrl, pos, size) {
}

RichVolumeIndicatorCtrl::~RichVolumeIndicatorCtrl() {
}

void RichVolumeIndicatorCtrl::Draw(wxDC* pDC) {
    DrawVolumeBar(pDC);
    // if (m_pKlineCtrl->m_mode == KlineType::Day || m_pKlineCtrl->m_mode == KlineType::Week ||
    //     m_pKlineCtrl->m_mode == KlineType::Month || m_pKlineCtrl->m_mode == KlineType::Quarter ||
    //     m_pKlineCtrl->m_mode == KlineType::Year) {
    //     if (m_mode == 0) {
    //         DrawVolumeBar(pDC);
    //     } else if (m_mode == 1) {
    //         DrawAmountBar(pDC);
    //     }
    //     if (m_pKlineCtrl->m_crossLine != NO_CROSS_LINE) {
    //         DrawCrossLine(pDC, m_pKlineCtrl->m_crossLinePt.x,
    //                       m_pKlineCtrl->m_crossLinePt.y + m_pKlineCtrl->m_height * 0.7, m_pKlineCtrl->m_width,
    //                       m_pKlineCtrl->m_height * 0.3);
    //     }
    // } else if (m_pKlineCtrl->m_mode == KlineType::Minute) {
    //     DrawMinuteBar(pDC, m_pKlineCtrl->m_pMinuteKlines, KlineType::Minute);
    // } else if (m_pKlineCtrl->m_mode == KlineType::FiveDay) {
    //     DrawMinuteBar(pDC, m_pKlineCtrl->m_pFiveDayMinuteKlines, KlineType::FiveDay);
    // }
}

std::string RichVolumeIndicatorCtrl::GetName() {
    return "成交量";
}

std::string RichVolumeIndicatorCtrl::GetFormulaName() {
    return "VOLUME";
}

void RichVolumeIndicatorCtrl::DrawVolumeBar(wxDC* pDC) {
    std::cout << m_pKlineCtrl << std::endl;
    if (m_pKlineCtrl->m_pKlines == nullptr || m_pKlineCtrl->m_pKlines->size() == 0) {
        return;
    }
    std::vector<uiKline>& klines = *(m_pKlineCtrl->m_pKlines);
    uiKlineRange& klineRng = m_pKlineCtrl->m_klineRng;
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
        double y = m_y + (1.0 - it->volume / max_volume) * m_height;
        double h = it->volume / max_volume * m_height;

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

void RichVolumeIndicatorCtrl::DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h) {
    wxPen dash_pen(wxColor(200, 200, 200), 1, wxPENSTYLE_LONG_DASH);
    pDC->SetPen(dash_pen);
    pDC->DrawLine(centerX, centerY, centerX, h);  // 竖线
}

double RichVolumeIndicatorCtrl::GetMaxVolumeInRange() {
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

double RichVolumeIndicatorCtrl::GetMaxVolume() {
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

double RichVolumeIndicatorCtrl::GetFiveDayMaxVolume() {
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

// void RichVolumeIndicatorCtrl::DrawMinuteBar(wxDC* pDC, std::vector<minuteKline>* pMinuteKlines, KlineType kline_type)
// {
//     if (pMinuteKlines == nullptr || pMinuteKlines->size() == 0) {
//         return;
//     }
//     double max_volume = kline_type == KlineType::Minute ? GetMaxVolume() : GetFiveDayMaxVolume();
//     double max_lines = kline_type == KlineType::Minute ? 240 : 1200;
//     double inner_width =
//         static_cast<double>((m_pKlineCtrl->m_width - m_pKlineCtrl->m_paddingLeft - m_pKlineCtrl->m_paddingRight));
//     double w = inner_width / max_lines;
//     size_t nTotalLine = 240;
//     if (kline_type == KlineType::Minute) {
//         nTotalLine = pMinuteKlines->size() < 240 ? pMinuteKlines->size() : 240;
//     } else {
//         nTotalLine = pMinuteKlines->size() < 1200 ? pMinuteKlines->size() : 1200;
//     }
//     wxPen greenPen(wxColor(84, 255, 255));
//     wxPen normalPen(wxColor(215, 215, 215));
//     for (size_t i = 1; i < nTotalLine; i++) {
//         minuteKline kline = pMinuteKlines->at(i);
//         double x = (double)(i * w) + m_pKlineCtrl->m_paddingLeft;
//         double y = m_y + (1.0 - kline.volume / max_volume) * m_height;
//         double h = kline.volume / max_volume * m_height;
//         double prevPrice = pMinuteKlines->at(i - 1).price;
//         if (kline.price > prevPrice) {
//             pDC->SetPen(*wxRED_PEN);
//         } else if (kline.price == prevPrice) {
//             pDC->SetPen(normalPen);
//         } else {
//             pDC->SetPen(greenPen);
//         }
//         pDC->DrawLine(x, y, x, y + h);
//     }
//     // 绘制矩形边框
//     pDC->SetPen(wxPen(wxColor(45, 45, 45)));
//     pDC->SetBrush(*wxTRANSPARENT_BRUSH);
//     wxPoint ptTopLeft(m_x, m_y);
//     wxSize rtSize(m_width,m_height);
//     pDC->DrawRectangle(ptTopLeft, rtSize);

//     // 计算左右两边的分时成交量或者成交额
//     double hRow = m_height / 4;
//     double rowPrice = max_volume / 4;

//     // 绘制中间的虚横线
//     wxPen dotPen(wxColor(45, 45, 45), 1, wxPENSTYLE_SHORT_DASH);
//     wxPen dotPen2(wxColor(45, 45, 45), 2, wxPENSTYLE_SOLID);
//     pDC->SetPen(dotPen);
//     wxPoint ptLeft(m_x,m_y);
//     wxPoint ptRight(m_x+m_width,m_y+m_height);
//     for (size_t i = 0; i < 4; i++) {
//         pDC->DrawLine(ptLeft, ptRight);
//         ptLeft.y += hRow;
//         ptRight.y += hRow;
//     }
//     // 绘制中间的虚竖线
//     int nCols = (kline_type == KlineType::Day) ? 8 : 20;
//     double wCol = inner_width / nCols;
//     wxPoint ptTop(m_x,m_y);
//     wxPoint ptBottom(m_x+m_width,m_y+m_height);
//     for (size_t i = 0; i < nCols; i++) {
//         ptTop.x += wCol;
//         ptBottom.x += wCol;
//         if ((i + 1) % 4 == 0) {
//             continue;
//         }
//         pDC->DrawLine(ptTop, ptBottom);
//     }
//     // 绘制中间的粗虚竖线
//     pDC->SetPen(dotPen2);
//     ptTop.x = m_pKlineCtrl->m_paddingLeft;
//     ptBottom.x = m_pKlineCtrl->m_paddingLeft;
//     for (size_t i = 0; i < nCols;) {
//         ptTop.x += wCol * 4;
//         ptBottom.x += wCol * 4;
//         i += 4;
//         pDC->DrawLine(ptTop, ptBottom);
//     }
//     // 绘制左右两边的分时成交量或者成交额
//     wxRect rectLeft(2, m_y - hRow / 2, m_pKlineCtrl->m_paddingLeft - 4, hRow);
//     wxRect rectRight(m_pKlineCtrl->m_width - m_pKlineCtrl->m_paddingRight + 2, m_y - hRow / 2,
//                      m_pKlineCtrl->m_paddingRight, hRow);
//     pDC->SetTextForeground(wxColor(180, 182, 184));
//     for (size_t i = 0; i < 4; i++) {
//         wxString label = CN(convert_double((max_volume - rowPrice * i) / 100, 0));
//         pDC->DrawLabel(label, rectLeft, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//         pDC->DrawLabel(label, rectRight, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
//         rectLeft.y += hRow;
//         rectRight.y += hRow;
//     }
// }
