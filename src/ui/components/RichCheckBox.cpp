///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/commponents/RichCheckBox.cpp
// Purpose:     NO KEYBOARD AND MOUSE FOCUS checkbox control
// Author:      songhuabiao
// Modified by:
// Created:     2025-01-09 21:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichCheckBox.h"
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <iostream>
#include "ui/RichHelper.h"
#include "ui/events/RichCheckBoxEvent.h"
#include "util/Color.h"

RichCheckBox::RichCheckBox(const std::vector<std::string>& options,  // 需要显示的选项
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
        m_checkboxLabels.emplace_back(CN(options[i]));
    }
    Bind(wxEVT_LEFT_DOWN, &RichCheckBox::OnClick, this);
    Bind(wxEVT_PAINT, &RichCheckBox::OnPaint, this);
    Bind(wxEVT_MOTION, &RichCheckBox::OnMouseMove, this);
    Bind(wxEVT_ERASE_BACKGROUND, &RichCheckBox::OnEraseBackground, this);
    Layout();
}

void RichCheckBox::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    for (size_t i = 0; i < m_checkboxLabels.size(); i++) {
        if (i == m_active || i == m_focus) {
            dc.SetTextForeground(m_clrTextActive);
            dc.DrawText(m_checkboxLabels[i], m_checkboxRects[i].x, m_checkboxRects[i].y);
        } else {
            dc.SetTextForeground(m_clrTextDefault);
            dc.DrawText(m_checkboxLabels[i], m_checkboxRects[i].x, m_checkboxRects[i].y);
        }
    }
}

void RichCheckBox::OnMouseMove(wxMouseEvent& event) {
    int mouseX = event.GetX();
    int mouseY = event.GetY();
    for (size_t i = 0; i < m_checkboxRects.size(); i++) {
        if (mouseX >= m_checkboxRects[i].GetLeft() && mouseY >= m_checkboxRects[i].GetTop() &&
            mouseX <= m_checkboxRects[i].GetRight() && mouseY <= m_checkboxRects[i].GetBottom()) {
            m_focus = i;
        }
    }
    Refresh();
}

wxString RichCheckBox::GetActiveLabel() {
    return m_checkboxLabels[m_active];
}

int RichCheckBox::GetActiveIndex() {
    return m_active;
}

bool RichCheckBox::Layout() {
    if (m_checkboxLabels.size() < 1) {
        return false;
    }
    int offsetX = this->GetPosition().x;
    int offsetY = 0;
    int padding = 12;
    wxClientDC dc(this);
    for (size_t i = 0; i < m_checkboxLabels.size(); i++) {
        wxSize text_size = dc.GetTextExtent(m_checkboxLabels[i]);
        wxRect rc;
        rc.x = offsetX;
        rc.y = offsetY;
        rc.width = text_size.GetWidth();
        rc.height = 16;
        m_checkboxRects.push_back(rc);
        offsetX += rc.width + padding;
    }
    return true;
}

void RichCheckBox::SetSelection(int n) {
    if ((size_t)n > m_checkboxLabels.size()) {
        return;
    }
    if (m_active != n) {
        m_active = n;
    }
}

void RichCheckBox::OnClick(wxMouseEvent& event) {
    if (event.LeftDown()) {
        int mouseX = event.GetX();
        int mouseY = event.GetY();
        for (size_t i = 0; i < m_checkboxRects.size(); i++) {
            if (mouseX >= m_checkboxRects[i].GetLeft() && mouseY >= m_checkboxRects[i].GetTop() &&
                mouseX <= m_checkboxRects[i].GetRight() && mouseY <= m_checkboxRects[i].GetBottom()) {
                m_active = i;
                // 发送自定义事件
                RichCheckBoxEvent checkbox_event = RichCheckBoxEvent(wxEVT_RICH_CHECKBOX, GetId());
                checkbox_event.SetChecked(m_active, true);
                wxPostEvent(this->GetParent(), checkbox_event);
                break;
            }
        }
    }
}

void RichCheckBox::OnEraseBackground(wxEraseEvent& event) {
    wxDC* pDC = event.GetDC();
    pDC->SetBrush(wxBrush(KLINE_PANEL_BACKGROUND_COLOR));
    pDC->SetPen(wxPen(KLINE_PANEL_BACKGROUND_COLOR));
    wxSize size = GetSize();
    pDC->DrawRectangle(wxPoint(0, 0), size);
}