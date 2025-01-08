///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/indicators/RichMinuteVolumnIndicatorCtrl.cpp
// Purpose:     分时/五日分时 成交量附图指标
// Author:      songhuabiao
// Modified by:
// Created:     2025-01-08 19:35
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichMinuteVolumeIndicatorCtrl.h"
#include "ui/RichHelper.h"
#include "ui/components/RichKlineCtrl.h"
#include "util/Color.h"
#include "util/Global.h"

RichMinuteVolumeIndicatorCtrl::RichMinuteVolumeIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                                                             const wxPoint& pos,
                                                             const wxSize& size)
    : RichIndicatorCtrl(pKlineCtrl, pos, size) {
}

RichMinuteVolumeIndicatorCtrl::~RichMinuteVolumeIndicatorCtrl() {
}

void RichMinuteVolumeIndicatorCtrl::DrawTitleBar(wxDC* pDC) {
}

void RichMinuteVolumeIndicatorCtrl::Draw(wxDC* pDC) {
    if (!m_visible) {
        return;
    }
    if (m_pKlineCtrl->m_mode == KlineType::Minute) {
        DrawMinuteBar(pDC, m_pKlineCtrl->m_pMinuteKlines, KlineType::Minute);
    } else if (m_pKlineCtrl->m_mode == KlineType::FiveDay) {
        DrawMinuteBar(pDC, m_pKlineCtrl->m_pFiveDayMinuteKlines, KlineType::FiveDay);
    }
}

wxString RichMinuteVolumeIndicatorCtrl::GetName() {
    return CN("分时成交量");
}

wxString RichMinuteVolumeIndicatorCtrl::GetFormulaName() {
    return "Minute VOLUME";
}

double RichMinuteVolumeIndicatorCtrl::GetMaxVolume() {
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

double RichMinuteVolumeIndicatorCtrl::GetFiveDayMaxVolume() {
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

void RichMinuteVolumeIndicatorCtrl::DrawMinuteBar(wxDC* pDC,
                                                  std::vector<minuteKline>* pMinuteKlines,
                                                  KlineType kline_type) {
    if (pMinuteKlines == nullptr || pMinuteKlines->size() == 0) {
        return;
    }
    double max_volume = kline_type == KlineType::Minute ? GetMaxVolume() : GetFiveDayMaxVolume();
    double max_lines = kline_type == KlineType::Minute ? 240 : 1200;
    double inner_width =
        static_cast<double>((m_pKlineCtrl->m_width - m_pKlineCtrl->m_paddingLeft - m_pKlineCtrl->m_paddingRight));
    double w = inner_width / max_lines;
    double offsetX = m_pKlineCtrl->m_pos.x + m_pKlineCtrl->m_paddingLeft;
    size_t nTotalLine = 240;
    if (kline_type == KlineType::Minute) {
        nTotalLine = pMinuteKlines->size() < 240 ? pMinuteKlines->size() : 240;
    } else {
        nTotalLine = pMinuteKlines->size() < 1200 ? pMinuteKlines->size() : 1200;
    }

    wxPen redPen(RED_COLOR);
    wxPen greenPen(GREEN_COLOR);
    wxPen normalPen(wxColor(215, 215, 215));
    for (size_t i = 1; i < nTotalLine; i++) {
        minuteKline kline = pMinuteKlines->at(i);
        double x = (double)(i * w) + m_pKlineCtrl->m_paddingLeft;
        double y = m_y + (1.0 - kline.volume / max_volume) * m_height;
        double h = kline.volume / max_volume * m_height;
        double prevPrice = pMinuteKlines->at(i - 1).price;
        if (kline.price > prevPrice) {
            pDC->SetPen(redPen);
        } else if (kline.price == prevPrice) {
            pDC->SetPen(normalPen);
        } else {
            pDC->SetPen(greenPen);
        }
        pDC->DrawLine(x, y, x, y + h);
    }

    // 绘制矩形边框
    pDC->SetPen(wxPen(KLINE_PANEL_BORDER_COLOR));
    pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    wxPoint ptTopLeft(offsetX, m_y);
    wxSize rtSize(m_width, m_height);
    pDC->DrawRectangle(ptTopLeft, rtSize);

    // 计算左右两边的分时成交量或者成交额
    double hRow = m_height / 4;
    double rowPrice = max_volume / 4;

    // 绘制中间的虚横线
    wxPen dotPen(KLINE_PANEL_BORDER_COLOR, 1, wxPENSTYLE_DOT);
    wxPen solidPen(KLINE_PANEL_BORDER_COLOR, 2, wxPENSTYLE_SOLID);
    pDC->SetPen(dotPen);
    wxPoint ptLeft(offsetX, m_y + hRow);
    wxPoint ptRight(offsetX + m_width, m_y + hRow);
    for (size_t i = 0; i < 4; i++) {
        pDC->DrawLine(ptLeft, ptRight);
        ptLeft.y += hRow;
        ptRight.y += hRow;
    }
    // 绘制中间的虚竖线
    int nCols = (kline_type == KlineType::Minute) ? 8 : 20;
    double wCol = inner_width / nCols;
    wxPoint ptTop(offsetX, m_y);
    wxPoint ptBottom(offsetX, m_y + m_height);
    for (size_t i = 0; i < nCols; i++) {
        ptTop.x += wCol;
        ptBottom.x += wCol;
        if ((i + 1) % 4 == 0) {
            continue;
        }
        pDC->DrawLine(ptTop, ptBottom);
    }
    // 绘制中间的粗虚竖线
    pDC->SetPen(solidPen);
    ptTop.x = offsetX;
    ptBottom.x = m_pKlineCtrl->m_paddingLeft;
    for (size_t i = 0; i < nCols;) {
        ptTop.x += wCol * 4;
        ptBottom.x += wCol * 4;
        i += 4;
        pDC->DrawLine(ptTop, ptBottom);
    }

    // 绘制左右两边的分时成交量或者成交额
    wxRect rectLeft(2, m_y - hRow / 2, m_pKlineCtrl->m_paddingLeft - 4, hRow);
    wxRect rectRight(m_pKlineCtrl->m_width - m_pKlineCtrl->m_paddingRight + 2, m_y - hRow / 2,
                     m_pKlineCtrl->m_paddingRight, hRow);
    pDC->SetTextForeground(wxColor(180, 182, 184));
    for (size_t i = 0; i < 4; i++) {
        wxString label = CN(convert_double((max_volume - rowPrice * i) / 100, 0));
        pDC->DrawLabel(label, rectLeft, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        pDC->DrawLabel(label, rectRight, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        rectLeft.y += hRow;
        rectRight.y += hRow;
    }
    pDC->DrawLabel(wxT("0"), rectLeft, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    pDC->DrawLabel(wxT("0"), rectRight, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
}
