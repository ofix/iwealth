///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/RichDialogKlineInfo.cpp
// Purpose:     dialog for display day kline information
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-14 16:59
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/RichDialogKlineInfo.h"
#include <wx/dcbuffer.h>
#include "ui/RichHelper.h"
#include "util/Global.h"

BEGIN_EVENT_TABLE(RichDialogKlineInfo, wxDialog)
EVT_PAINT(RichDialogKlineInfo::OnPaint)
END_EVENT_TABLE()

RichDialogKlineInfo::RichDialogKlineInfo(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxDialog(parent, id, wxT(""), pos, size) {
    // 必须调用此函数，否则无法重绘wxDialog及其子类
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

RichDialogKlineInfo::~RichDialogKlineInfo() {
    // dtor
}

void RichDialogKlineInfo::OnPaint(wxPaintEvent& event) {
    if (m_pKline == nullptr) {
        return;
    }
    wxAutoBufferedPaintDC dc(this);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
    int x = 8;
    int y = 8;
    int hFont = 20;
    wxRect rect(x + 20, y, 110, hFont);
    int alignment = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL;

    std::string week = GetWeek(m_pKline->day);
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("时间"), x, y);
    dc.DrawLabel(CN(m_pKline->day + "/" + week), rect, alignment);
    ////////////////////////////////
    dc.DrawText(CN("开盘价"), x, y + hFont * 1);
    if (m_pKline->price_open < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_open == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    rect.Offset(0, hFont);
    dc.DrawLabel(CN(convert_double(m_pKline->price_open)), rect, alignment);
    ////////////////////////////////
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("收盘价"), x, y + hFont * 2);
    if (m_pKline->price_close < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_close == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    rect.Offset(0, hFont);
    dc.DrawLabel(CN(convert_double(m_pKline->price_close)), rect, alignment);

    ////////////////////////////////
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("最高价"), x, y + hFont * 3);
    if (m_pKline->price_max < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_max == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    rect.Offset(0, hFont);
    dc.DrawLabel(CN(convert_double(m_pKline->price_max)), rect, alignment);
    ////////////////////////////////
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("最低价"), x, y + hFont * 4);
    if (m_pKline->price_min < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_min == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    rect.Offset(0, hFont);
    dc.DrawLabel(CN(convert_double(m_pKline->price_min)), rect, alignment);
    ////////////////////////////////
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("当前价"), x, y + hFont * 5);
    if (m_pKline->price_now < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_now == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    rect.Offset(0, hFont);
    dc.DrawLabel(CN(convert_double(m_pKline->price_now)), rect, alignment);
    ////////////////////////////////
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("涨幅"), x, y + hFont * 6);
    if (m_pKline->change_rate < 0) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->change_rate == 0) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    rect.Offset(0, hFont);
    dc.DrawLabel(CN(convert_double(m_pKline->change_rate) + "%"), rect, alignment);
    ////////////////////////////////
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("换手率"), x, y + hFont * 7);
    rect.Offset(0, hFont);
    dc.DrawLabel(CN(convert_double(m_pKline->turnover_rate) + "%"), rect, alignment);
    ////////////////////////////////
    dc.DrawText(CN("成交量"), x, y + hFont * 8);
    rect.Offset(0, hFont);
    dc.DrawLabel(RichUnit(m_pKline->volume), rect, alignment);
    ////////////////////////////////
    dc.DrawText(CN("成交额"), x, y + hFont * 9);
    rect.Offset(0, hFont);
    dc.DrawLabel(RichUnit(m_pKline->amount), rect, alignment);
}

void RichDialogKlineInfo::SetCurrentKline(uiKline* pKline) {
    m_pKline = pKline;
}

void RichDialogKlineInfo::SetPreviousKline(uiKline* pKline) {
    m_pPreviousKline = pKline;
}

std::string RichDialogKlineInfo::GetWeek(std::string date) {
    std::string strYear = date.substr(0, 4);
    std::string strMonth = date.substr(5, 2);
    std::string strDay = date.substr(8, 2);
    int year = std::stoi(strYear);
    int month = std::stoi(strMonth);
    int day = std::stoi(strDay);
    return CalcWeek(year, month, day);
}

// 通过日期判断是星期几可以通过基姆拉尔森计算公式算出
std::string RichDialogKlineInfo::CalcWeek(int year, int month, int day) {
    if (month == 1 || month == 2) {  // 把一月和二月换算成上一年的十三月和是四月
        month += 12;
        year--;
    }
    int week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    switch (week) {
        case 0:
            return "一";
        case 1:
            return "二";
        case 2:
            return "三";
        case 3:
            return "四";
        case 4:
            return "五";
        case 5:
            return "六";
        case 6:
            return "日";
    }
    return "";
}
