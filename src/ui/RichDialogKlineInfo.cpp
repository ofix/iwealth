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

RichDialogKlineInfo::RichDialogKlineInfo(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : RichDialog(parent, id, wxT(""), pos, size) {
}

RichDialogKlineInfo::~RichDialogKlineInfo() {
    // dtor
}

void RichDialogKlineInfo::OnDrawClient(wxDC* pDC) {
    if (m_pKline == nullptr) {
        return;
    }
#define CLR_RED wxColor(255, 0, 0)
#define CLR_WHITE wxColor(255, 255, 255)
#define CLR_GREEN wxColor(0, 255, 0)
#define CLR_TEXT wxColor(210, 210, 210)
    ////////////////////////////////
    int x = 8;
    int y = 28;
    int hFont = 20;
    wxRect rect(x + 20, y, 110, hFont);
    int alignment = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL;
    std::string week = GetWeek(m_pKline->day);
    pDC->SetTextForeground(CLR_TEXT);
    pDC->DrawText(CN("时间"), x, y);
    pDC->DrawLabel(CN(m_pKline->day + "/" + week), rect, alignment);
    ////////////////////////////////
    pDC->DrawText(CN("开盘价"), x, y + hFont * 1);
    if (m_pKline->price_open < m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_GREEN);
    } else if (m_pKline->price_open == m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_WHITE);
    } else {
        pDC->SetTextForeground(CLR_RED);
    }
    rect.Offset(0, hFont);
    pDC->DrawLabel(CN(convert_double(m_pKline->price_open)), rect, alignment);
    ////////////////////////////////
    pDC->SetTextForeground(CLR_TEXT);
    pDC->DrawText(CN("收盘价"), x, y + hFont * 2);
    if (m_pKline->price_close < m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_GREEN);
    } else if (m_pKline->price_close == m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_WHITE);
    } else {
        pDC->SetTextForeground(CLR_RED);
    }
    rect.Offset(0, hFont);
    pDC->DrawLabel(CN(convert_double(m_pKline->price_close)), rect, alignment);

    ////////////////////////////////
    pDC->SetTextForeground(CLR_TEXT);
    pDC->DrawText(CN("最高价"), x, y + hFont * 3);
    if (m_pKline->price_max < m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_GREEN);
    } else if (m_pKline->price_max == m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_WHITE);
    } else {
        pDC->SetTextForeground(CLR_RED);
    }
    rect.Offset(0, hFont);
    pDC->DrawLabel(CN(convert_double(m_pKline->price_max)), rect, alignment);
    ////////////////////////////////
    pDC->SetTextForeground(CLR_TEXT);
    pDC->DrawText(CN("最低价"), x, y + hFont * 4);
    if (m_pKline->price_min < m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_GREEN);
    } else if (m_pKline->price_min == m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_WHITE);
    } else {
        pDC->SetTextForeground(CLR_RED);
    }
    rect.Offset(0, hFont);
    pDC->DrawLabel(CN(convert_double(m_pKline->price_min)), rect, alignment);
    ////////////////////////////////
    pDC->SetTextForeground(CLR_TEXT);
    pDC->DrawText(CN("当前价"), x, y + hFont * 5);
    if (m_pKline->price_now < m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_GREEN);
    } else if (m_pKline->price_now == m_pPreviousKline->price_close) {
        pDC->SetTextForeground(CLR_WHITE);
    } else {
        pDC->SetTextForeground(CLR_RED);
    }
    rect.Offset(0, hFont);
    pDC->DrawLabel(CN(convert_double(m_pKline->price_now)), rect, alignment);
    ////////////////////////////////
    pDC->SetTextForeground(CLR_TEXT);
    pDC->DrawText(CN("涨幅"), x, y + hFont * 6);
    if (m_pKline->change_rate < 0) {
        pDC->SetTextForeground(CLR_GREEN);
    } else if (m_pKline->change_rate == 0) {
        pDC->SetTextForeground(CLR_WHITE);
    } else {
        pDC->SetTextForeground(CLR_RED);
    }
    rect.Offset(0, hFont);
    pDC->DrawLabel(CN(convert_double(m_pKline->change_rate) + "%"), rect, alignment);
    ////////////////////////////////
    pDC->SetTextForeground(CLR_TEXT);
    pDC->DrawText(CN("换手率"), x, y + hFont * 7);
    rect.Offset(0, hFont);
    pDC->DrawLabel(CN(convert_double(m_pKline->turnover_rate) + "%"), rect, alignment);
    ////////////////////////////////
    pDC->DrawText(CN("成交量"), x, y + hFont * 8);
    rect.Offset(0, hFont);
    pDC->DrawLabel(RichUnit(m_pKline->volume), rect, alignment);
    ////////////////////////////////
    pDC->DrawText(CN("成交额"), x, y + hFont * 9);
    rect.Offset(0, hFont);
    pDC->DrawLabel(RichUnit(m_pKline->amount), rect, alignment);
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
