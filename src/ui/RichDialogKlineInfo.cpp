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
#include "ui/components/RichKlineCtrl.h"

RichDialogKlineInfo::RichDialogKlineInfo(RichKlineCtrl* pKlineCtrl,
                                         wxWindow* parent,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size)
    : wxDialog(parent, id, wxT(""), pos, size), m_pKlineCtrl(pKlineCtrl) {
    // ctor
}

RichDialogKlineInfo::~RichDialogKlineInfo() {
    // dtor
}

void RichDialogKlineInfo::OnDraw(wxDC& dc) {
    uiKline cur = GetCurrentKlineInfo();
    uiKline old = GetPrevKlineInfo();
    dc.SetPen(wxPen(wxColor(255, 0, 0)));
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(10, 40, 65, 340);
    int x = 10;
    int y = 40;
    int hFont = 21;
    std::string day = FormatDay(cur.day);
    std::string week = GetWeek(cur.day);
    // day
    dc.SetTextForeground(wxColor(210, 210, 210));
    dc.DrawText("时间", x + 4, y + 4);
    dc.DrawText(day, x + 4, y + hFont);
    dc.DrawText(week, x + 4, y + hFont * 2);
    dc.DrawText("开盘价", x + 4, y + hFont * 3);
    dc.DrawText("最高价", x + 4, y + hFont * 5);
    dc.DrawText("最低价", x + 4, y + hFont * 7);
    dc.DrawText("收盘价", x + 4, y + hFont * 9);
    dc.DrawText("涨幅", x + 4, y + hFont * 11);
    dc.DrawText("振幅", x + 4, y + hFont * 13);
    dc.DrawText("成交量", x + 4, y + hFont * 15);
    dc.DrawText("成交额", x + 4, y + hFont * 17);
    // price open
    if (cur.price_open < old.price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (cur.price_open == old.price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // text
    // std::string::Format("%.2f",cur.price_open);
    dc.DrawText("1", x + 4, y + hFont * 4);
    // price max
    if (cur.price_max < old.price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (cur.price_max == old.price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // std::string::Format("%.2f",cur.price_max
    dc.DrawText("1", x + 4, y + hFont * 6);
    // price min
    if (cur.price_min < old.price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (cur.price_min == old.price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // std::string::Format("%.2f",cur.price_min)
    dc.DrawText("1", x + 4, y + hFont * 8);
    // price close
    if (cur.price_close < old.price_close) {
        dc.SetTextForeground(wxColor(0, 255, 0));
    } else if (cur.price_close == old.price_close) {
        dc.SetTextForeground(wxColor(255, 255, 255));
    } else {
        dc.SetTextForeground(wxColor(255, 0, 0));
    }
    // std::string::Format("%.2f",cur.price_close)
    dc.DrawText("1", x + 4, y + hFont * 10);
}

std::string RichDialogKlineInfo::FormatDay(std::string date) {
    std::string year = date.substr(0, 4);
    std::string month = date.substr(5, 2);
    std::string day = date.substr(8, 2);
    return year + "\r\n" + month + day;
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

uiKline RichDialogKlineInfo::GetCurrentKlineInfo() {
    int n;
    if (m_pKlineCtrl->m_crossLine == NO_CROSS_LINE) {
        n = m_pKlineCtrl->m_pKlines->size() - 1;
    } else {
        n = m_pKlineCtrl->m_crossLine;
    }
    return m_pKlineCtrl->m_pKlines->at(n);
}

uiKline RichDialogKlineInfo::GetPrevKlineInfo() {
    int n;
    if (m_pKlineCtrl->m_crossLine == NO_CROSS_LINE) {
        n = m_pKlineCtrl->m_pKlines->size() - 1;
    } else {
        n = m_pKlineCtrl->m_crossLine;
    }
    return m_pKlineCtrl->m_pKlines->at((n - 1) >= 0 ? (n - 1) : 0);
}
