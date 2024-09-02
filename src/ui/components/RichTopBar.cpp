///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/commponents/RichTopBar.cpp
// Purpose:     customize title bar
// Author:      songhuabiao
// Modified by:
// Created:     2024-09-02 19:40
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichTopBar.h"
#include <wx/dcbuffer.h>
#include <wx/event.h>
#include "ui/RichHelper.h"

#define TOP_MENU_PADDING 16

RichTopBar::RichTopBar(wxWindow* parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
    : wxWindow(parent, id, pos, size, style, name),
      m_dragging(false),
      m_hitState(TopBarHitState::DEFAULT),
      m_bLeftMouseDown(false) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(wxColor(150, 3, 6));
    Bind(wxEVT_LEFT_DOWN, &RichTopBar::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &RichTopBar::OnMouseLeftUp, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &RichTopBar::OnMouseCaptureLost, this);
    Bind(wxEVT_MOTION, &RichTopBar::OnMouseMove, this);
    Bind(wxEVT_PAINT, &RichTopBar::OnPaint, this);
}

RichTopBar::~RichTopBar() {
}

void RichTopBar::OnMouseLeftDown(wxMouseEvent& event) {
    m_bLeftMouseDown = true;
    if (!m_dragging) {
        if (CanMove()) {
            m_dragging = true;
            wxPoint clientStart = event.GetPosition();
            m_dragStartMouse = ClientToScreen(clientStart);
            m_dragStartWindow = GetParent()->GetPosition();
            CaptureMouse();
        }
    }
}

void RichTopBar::OnMouseLeftUp(wxMouseEvent&) {
    m_bLeftMouseDown = false;
    if (m_hitState == TopBarHitState::BTN_CLOSE) {
        wxCommandEvent event(wxEVT_CLOSE_WINDOW);
        wxPostEvent(GetParent(), event);
    }
    FinishDrag();
}

void RichTopBar::OnMouseMove(wxMouseEvent& event) {
    if (m_dragging) {
        wxPoint curClientPsn = event.GetPosition();
        wxPoint curScreenPsn = ClientToScreen(curClientPsn);
        wxPoint movementVector = curScreenPsn - m_dragStartMouse;
        GetParent()->SetPosition(m_dragStartWindow + movementVector);
    } else {
        OnHitTest(event);
        Refresh();
    }
}

void RichTopBar::OnMouseCaptureLost(wxMouseCaptureLostEvent&) {
    FinishDrag();
}

void RichTopBar::AddMenu(RichTopMenu* menu) {
    m_menus.push_back(menu);
}

void RichTopBar::OnHitTest(wxMouseEvent& event) {
    wxPoint pt = event.GetPosition();
    wxRect rectCloseBtn(GetSize().GetWidth() - 22, 10, 12, 12);
    if (rectCloseBtn.Contains(pt)) {
        m_hitState = TopBarHitState::BTN_CLOSE;
    } else {
        m_hitState = TopBarHitState::DEFAULT;
    }
}

bool RichTopBar::CanMove() {
    return m_hitState == TopBarHitState::DEFAULT;
}

void RichTopBar::FinishDrag() {
    if (m_dragging) {
        m_dragging = false;
    }
    if (HasCapture()) {
        ReleaseMouse();
    }
}

wxSize RichTopBar::DoGetBestClientSize() const {
    return wxSize(-1, 32);
    // wxSize frame_size = this->GetParent()->GetSize();
    // std::cout << "frame_size = " << frame_size.GetWidth() << std::endl;
    // return wxSize(frame_size.GetWidth(), 32);
}

void RichTopBar::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.SetBrush(wxBrush(wxColor(150, 3, 6)));
    dc.DrawRectangle(0, 0, 2048, 36);
    DrawIcon(&dc);
    DrawTitle(&dc);
    DrawTopMenu(&dc);
    DrawMimizeBox(&dc);
    DrawMaximizeBox(&dc);
    DrawCloseBox(&dc);
}
// logo
void RichTopBar::DrawIcon(wxDC* pDC) {
}
// 窗口标题
void RichTopBar::DrawTitle(wxDC* pDC) {
}
// 系统菜单
void RichTopBar::DrawTopMenu(wxDC* pDC) {
    size_t offsetX = 28;
    int height = GetSize().GetHeight();
    wxRect rect_menu(offsetX, 0, 0, 0);
    pDC->SetFont(RichHelper::GetDefaultFont(12));
    pDC->SetTextForeground(wxColor(229, 229, 229));
    for (auto& menu : m_menus) {
        wxSize menu_size = pDC->GetTextExtent(menu->GetName());
        pDC->DrawLabel(menu->GetName(), wxRect(offsetX, 0, menu_size.GetWidth() + TOP_MENU_PADDING * 2, height),
                       wxALIGN_CENTER | wxALIGN_CENTRE_VERTICAL);
        offsetX += menu_size.GetWidth() + TOP_MENU_PADDING * 2;
    }
}
// 最小化按钮
void RichTopBar::DrawMimizeBox(wxDC* pDC) {
}
// 最大化按钮
void RichTopBar::DrawMaximizeBox(wxDC* pDC) {
}
// 关闭按钮
void RichTopBar::DrawCloseBox(wxDC* pDC) {
    wxRect rect(GetSize().GetWidth() - 22, 10, 12, 12);

    if (m_hitState == TopBarHitState::BTN_CLOSE) {
        pDC->SetBrush(wxBrush(wxColor(242, 57, 64)));
        pDC->SetPen(*wxTRANSPARENT_PEN);
        wxRect rectBk(GetSize().GetWidth() - 32, 0, 32, 32);
        pDC->DrawRectangle(rectBk);
    }
    wxPen pen = wxPen(wxColor(229, 229, 229), 1, wxPENSTYLE_SOLID);
    pDC->SetPen(pen);
    pDC->DrawLine(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    pDC->DrawLine(rect.x, rect.y + rect.height, rect.x + rect.width, rect.y);
    pDC->DrawPoint(rect.x + rect.width, rect.y + rect.height);
    pDC->DrawPoint(rect.x + rect.width, rect.y);
}