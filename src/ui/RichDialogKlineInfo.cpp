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
    dc.SetPen(wxPen(wxColor(255, 0, 0)));
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(10, 40, 65, 340);
    int x = 10;
    int y = 40;
    int hFont = 21;
    std::string day = FormatDay(m_pKline->day);
    std::string week = GetWeek(m_pKline->day);
    // day
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText(CN("时间"), x + 4, y + 4);
    dc.DrawText(CN(day), x + 4, y + hFont);
    dc.DrawText(CN(week), x + 4, y + hFont * 2);
    dc.DrawText(CN("开盘价"), x + 4, y + hFont * 3);
    dc.DrawText(CN("最高价"), x + 4, y + hFont * 5);
    dc.DrawText(CN("最低价"), x + 4, y + hFont * 7);
    dc.DrawText(CN("收盘价"), x + 4, y + hFont * 9);
    dc.DrawText(CN("涨幅"), x + 4, y + hFont * 11);
    dc.DrawText(CN("振幅"), x + 4, y + hFont * 13);
    dc.DrawText(CN("成交量"), x + 4, y + hFont * 15);
    dc.DrawText(CN("成交额"), x + 4, y + hFont * 17);
    // price open
    if (m_pKline->price_open < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_open == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // text
    // std::string::Format("%.2f",m_pKline->price_open);
    dc.DrawText("1", x + 4, y + hFont * 4);
    // price max
    if (m_pKline->price_max < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_max == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // std::string::Format("%.2f",m_pKline->price_max
    dc.DrawText("1", x + 4, y + hFont * 6);
    // price min
    if (m_pKline->price_min < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_min == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // std::string::Format("%.2f",m_pKline->price_min)
    dc.DrawText("1", x + 4, y + hFont * 8);
    // price close
    if (m_pKline->price_close < m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (m_pKline->price_close == m_pPreviousKline->price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // std::string::Format("%.2f",m_pKline->price_close)
    dc.DrawText("1", x + 4, y + hFont * 10);
}

std::string RichDialogKlineInfo::FormatDay(std::string date) {
    std::string year = date.substr(0, 4);
    std::string month = date.substr(5, 2);
    std::string day = date.substr(8, 2);
    return year + "\r\n" + month + day;
}

void RichDialogKlineInfo::SetCurrentKline(uiKline* pKline) {
    m_pKline = pKline;
}

void RichDialogKlineInfo::SetPreviousKline(uiKline* pKline) {
    m_pPreviousKline = pKline;
}

std::string RichDialogKlineInfo::GetWeek(std::string /*date*/) {
    // std::string strYear = date.substr(0, 4);
    // std::string strMonth = date.substr(5, 2);
    // std::string strDay = date.substr(8, 2);
    // long year, month, day;
    // strYear.ToLong(&year);
    // strMonth.ToLong(&month);
    // strDay.ToLong(&day);
    return "";
    // return CalcWeek(year, month, day);
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
            return "星期一";
        case 1:
            return "星期二";
        case 2:
            return "星期三";
        case 3:
            return "星期四";
        case 4:
            return "星期五";
        case 5:
            return "星期六";
        case 6:
            return "星期日";
    }
    return "";
}
