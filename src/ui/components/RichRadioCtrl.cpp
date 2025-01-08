///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/commponents/RichRadioCtrl.cpp
// Purpose:     NO KEYBOARD AND MOUSE FOCUS radio control
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichRadioCtrl.h"
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <iostream>
#include "ui/RichHelper.h"
#include "ui/events/RichRadioEvent.h"
#include "util/Color.h"

RichRadioCtrl::RichRadioCtrl(const std::vector<std::string>& options,  // 需要显示的选项
                             int current,                              // 初始化第几项选中
                             wxWindow* parent,                         // 父窗口
                             wxWindowID id,                            // 窗口ID
                             const wxPoint& pos,                       // 窗口位置
                             const wxSize& size,                       // 窗口大小
                             long style                                // 窗口样式
                             )
    : wxControl(parent, id, pos, size, style | wxNO_BORDER), m_active(current), m_focus(current) {
    m_clrBackground = KLINE_PANEL_BACKGROUND_COLOR;
    m_clrTextDefault = wxColor(180, 180, 180);
    m_clrTextActive = wxColor(233, 223, 0);
    SetBackgroundColour(m_clrBackground);
    for (size_t i = 0; i < options.size(); i++) {
        m_radioLabels.emplace_back(CN(options[i]));
    }
    Bind(wxEVT_LEFT_DOWN, &RichRadioCtrl::OnClick, this);
    Bind(wxEVT_PAINT, &RichRadioCtrl::OnPaint, this);
    Bind(wxEVT_MOTION, &RichRadioCtrl::OnMouseMove, this);
    Bind(wxEVT_ERASE_BACKGROUND, &RichRadioCtrl::OnEraseBackground, this);
    Layout();
}

void RichRadioCtrl::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    for (size_t i = 0; i < m_radioLabels.size(); i++) {
        if (i == m_active || i == m_focus) {
            dc.SetTextForeground(m_clrTextActive);
            dc.DrawText(m_radioLabels[i], m_radioRects[i].x, m_radioRects[i].y);
        } else {
            dc.SetTextForeground(m_clrTextDefault);
            dc.DrawText(m_radioLabels[i], m_radioRects[i].x, m_radioRects[i].y);
        }
    }
}

void RichRadioCtrl::OnMouseMove(wxMouseEvent& event) {
    int mouseX = event.GetX();
    int mouseY = event.GetY();
    for (size_t i = 0; i < m_radioRects.size(); i++) {
        if (mouseX >= m_radioRects[i].GetLeft() && mouseY >= m_radioRects[i].GetTop() &&
            mouseX <= m_radioRects[i].GetRight() && mouseY <= m_radioRects[i].GetBottom()) {
            m_focus = i;
        }
    }
    Refresh();
}

wxString RichRadioCtrl::GetActiveLabel() {
    return m_radioLabels[m_active];
}

int RichRadioCtrl::GetActiveIndex() {
    return m_active;
}

bool RichRadioCtrl::Layout() {
    if (m_radioLabels.size() < 1) {
        return false;
    }
    int offsetX = 0;
    int offsetY = 0;
    int padding = 12;
    wxClientDC dc(this);
    for (size_t i = 0; i < m_radioLabels.size(); i++) {
        wxSize text_size = dc.GetTextExtent(m_radioLabels[i]);
        wxRect rc;
        rc.x = offsetX;
        rc.y = offsetY;
        rc.width = text_size.GetWidth();
        rc.height = 16;
        m_radioRects.push_back(rc);
        offsetX += rc.width + padding;
    }
    return true;
}

void RichRadioCtrl::SetSelection(int n) {
    if ((size_t)n > m_radioLabels.size()) {
        return;
    }
    if (m_active != n) {
        m_active = n;
    }
}

void RichRadioCtrl::OnClick(wxMouseEvent& event) {
    if (event.LeftDown()) {
        int mouseX = event.GetX();
        int mouseY = event.GetY();
        for (size_t i = 0; i < m_radioRects.size(); i++) {
            if (mouseX >= m_radioRects[i].GetLeft() && mouseY >= m_radioRects[i].GetTop() &&
                mouseX <= m_radioRects[i].GetRight() && mouseY <= m_radioRects[i].GetBottom()) {
                m_active = i;
                // 发送自定义事件
                RichRadioEvent radio_event = RichRadioEvent(wxEVT_RICH_RADIO, GetId());
                radio_event.SetSelection(m_active);
                wxPostEvent(this->GetParent(), radio_event);
                break;
            }
        }
    }
}

void RichRadioCtrl::OnEraseBackground(wxEraseEvent& event) {
    wxDC* pDC = event.GetDC();
    pDC->SetBrush(wxBrush(KLINE_PANEL_BACKGROUND_COLOR));
    pDC->SetPen(wxPen(KLINE_PANEL_BACKGROUND_COLOR));
    wxSize size = GetSize();
    pDC->DrawRectangle(wxPoint(0, 0), size);
}