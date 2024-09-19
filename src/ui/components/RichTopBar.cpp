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

#define FRAME_BOX_SIZE 32
#define FRAME_BOX_MARGIN 4
#define FRAME_BOX_OUTER_SIZE 36

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
    Bind(wxEVT_LEAVE_WINDOW, &RichTopBar::OnLeaveWindow, this);
}

RichTopBar::~RichTopBar() {
}

void RichTopBar::SetTopBarStyle(long style) {
    m_topBarStyle = style;
}

long RichTopBar::GetTopBarStyle() const {
    return m_topBarStyle;
}

void RichTopBar::OnLeaveWindow(wxMouseEvent& event) {
    m_hitState = TopBarHitState::DEFAULT;
    Refresh();
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

void RichTopBar::OnMouseLeftUp(wxMouseEvent& event) {
    m_bLeftMouseDown = false;
    if (m_hitState == TopBarHitState::BOX_CLOSE) {
        wxCommandEvent event(wxEVT_CLOSE_WINDOW);
        wxPostEvent(GetParent(), event);
    } else if (m_hitState == TopBarHitState::BOX_MAXIMIZE) {
        wxCommandEvent event(wxEVT_MAXIMIZE);
        wxPostEvent(GetParent(), event);
        std::cout << "Send Maximize Event" << std::endl;
    } else if (m_hitState == TopBarHitState::BOX_MINIMIZE) {
        wxCommandEvent event(wxEVT_ICONIZE);
        wxPostEvent(GetParent(), event);
        std::cout << "Send Mimize Event" << std::endl;
    }
    FinishDrag();
    event.Skip();
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
    wxRect rect(GetSize().GetWidth() - FRAME_BOX_OUTER_SIZE, FRAME_BOX_MARGIN, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
    m_hitState = TopBarHitState::DEFAULT;
    if (rect.Contains(pt)) {
        if (m_topBarStyle & wxCLOSE_BOX) {  // 关闭按钮
            m_hitState = TopBarHitState::BOX_CLOSE;
            return;
        } else if (m_topBarStyle & wxMAXIMIZE_BOX) {
            m_hitState = TopBarHitState::BOX_MAXIMIZE;
            return;
        } else if (m_topBarStyle & wxMINIMIZE_BOX) {
            m_hitState = TopBarHitState::BOX_MINIMIZE;
            return;
        }
    }

    rect.Offset(-FRAME_BOX_OUTER_SIZE, 0);  // 最大化按钮
    if (rect.Contains(pt)) {
        if (m_topBarStyle & wxMAXIMIZE_BOX) {
            m_hitState = TopBarHitState::BOX_MAXIMIZE;
            return;
        } else if (m_topBarStyle & wxMINIMIZE_BOX) {
            m_hitState = TopBarHitState::BOX_MINIMIZE;
            return;
        }
    }

    rect.Offset(-FRAME_BOX_OUTER_SIZE, 0);  // 最小化按钮
    if (rect.Contains(pt)) {
        if (m_topBarStyle & wxMINIMIZE_BOX) {
            m_hitState = TopBarHitState::BOX_MINIMIZE;
            return;
        }
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
    if (m_topBarStyle & wxMINIMIZE_BOX) {
        DrawMinimizeBox(&dc);
    }
    if (m_topBarStyle & wxMAXIMIZE_BOX) {
        DrawMaximizeBox(&dc);
    }
    if (m_topBarStyle & wxCLOSE_BOX) {
        DrawCloseBox(&dc);
    }
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

void RichTopBar::DrawMaximizeBox(wxDC* pDC) {
    int offsetX = GetSize().GetWidth();
    if (m_topBarStyle & wxCLOSE_BOX) {
        offsetX -= FRAME_BOX_OUTER_SIZE;
    }
    offsetX -= FRAME_BOX_OUTER_SIZE;
    if (m_hitState == TopBarHitState::BOX_MAXIMIZE) {
        pDC->SetBrush(wxBrush(wxColor(242, 57, 64)));
        pDC->SetPen(*wxTRANSPARENT_PEN);
        wxRect rectBk(offsetX, FRAME_BOX_MARGIN, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
        pDC->DrawRectangle(rectBk);
    }
    wxPen pen = wxPen(wxColor(229, 229, 229), 1, wxPENSTYLE_SOLID);
    pDC->SetPen(pen);
    int offsetY = FRAME_BOX_MARGIN;
    wxRect rect(offsetX + 4, offsetY + 8, FRAME_BOX_SIZE / 3, FRAME_BOX_SIZE / 3);
    pDC->DrawRectangle(rect);
    rect.Offset(4, -4);
    pDC->DrawRectangle(rect);
}

void RichTopBar::DrawMinimizeBox(wxDC* pDC) {
    long style = GetWindowStyleFlag();
    int offsetX = GetSize().GetWidth();
    if (m_topBarStyle & wxCLOSE_BOX) {
        offsetX -= FRAME_BOX_OUTER_SIZE;
    }
    if (m_topBarStyle & wxMAXIMIZE_BOX) {
        offsetX -= FRAME_BOX_OUTER_SIZE;
    }

    offsetX -= FRAME_BOX_OUTER_SIZE;
    if (m_hitState == TopBarHitState::BOX_MINIMIZE) {
        pDC->SetBrush(wxBrush(wxColor(242, 57, 64)));
        pDC->SetPen(*wxTRANSPARENT_PEN);
        wxRect rectBk(offsetX, 4, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
        pDC->DrawRectangle(rectBk);
    }
    wxPen pen = wxPen(wxColor(229, 229, 229), 1, wxPENSTYLE_SOLID);
    pDC->SetPen(pen);

    pDC->DrawLine(offsetX + FRAME_BOX_SIZE / 4, 16, offsetX + 24, 16);
}

void RichTopBar::DrawCloseBox(wxDC* pDC) {
    if (m_hitState == TopBarHitState::BOX_CLOSE) {
        pDC->SetBrush(wxBrush(wxColor(242, 57, 64)));
        pDC->SetPen(*wxTRANSPARENT_PEN);
        wxRect rectBk(GetSize().GetWidth() - FRAME_BOX_OUTER_SIZE, FRAME_BOX_MARGIN, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
        pDC->DrawRectangle(rectBk);
    }

    wxRect rect(GetSize().GetWidth() - (FRAME_BOX_MARGIN + 10 + 12), FRAME_BOX_MARGIN + 10, 12, 12);
    wxPen pen = wxPen(wxColor(229, 229, 229), 1, wxPENSTYLE_SOLID);
    pDC->SetPen(pen);
    // 绘制十字线
    pDC->DrawLine(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    pDC->DrawLine(rect.x, rect.y + rect.height, rect.x + rect.width, rect.y);
    pDC->DrawPoint(rect.x + rect.width, rect.y + rect.height);
    pDC->DrawPoint(rect.x + rect.width, rect.y);
}