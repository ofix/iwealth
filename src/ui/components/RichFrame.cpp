///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/components/RichFrame.cpp
// Purpose:     the customize main frame with top menu bar like vscode
// Author:      songhuabiao
// Modified by:
// Created:     2024-09-07 16:36
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichFrame.h"
#include <wx/dcbuffer.h>
#include <wx/event.h>
#include <wx/pen.h>
#include <iostream>

wxIMPLEMENT_DYNAMIC_CLASS(RichFrame, wxTopLevelWindow);

#define FRAME_BOX_SIZE 32
#define FRAME_BOX_MARGIN 4
#define FRAME_BOX_OUTER_SIZE 36

RichFrame::RichFrame() : wxTopLevelWindow() {
    m_frameMenuBar = NULL;
}

RichFrame::RichFrame(wxWindow* parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
    : wxTopLevelWindow(parent, id, name, pos, size, style, name) {
    Create(parent, id, pos, size, style, name);
    Bind(wxEVT_CLOSE_WINDOW, &RichFrame::OnClose, this);
    Bind(wxEVT_ICONIZE, &RichFrame::OnIconize, this);
    Bind(wxEVT_MAXIMIZE, &RichFrame::OnMaximize, this);

    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(wxColor(150, 3, 6));
    Bind(wxEVT_LEFT_DOWN, &RichFrame::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &RichFrame::OnMouseLeftUp, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &RichFrame::OnMouseCaptureLost, this);
    Bind(wxEVT_MOTION, &RichFrame::OnMouseMove, this);
    Bind(wxEVT_PAINT, &RichFrame::OnPaint, this);

    m_bLeftMouseDown = false;
    m_dragging = false;
    m_topBarStyle = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX;
}

bool RichFrame::Create(wxWindow* parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name) {
    SetTopBarStyle(style);
    style &= ~(wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER); // 移除三个按钮,防止外边框出现
    return wxTopLevelWindow::Create(parent, id, name, pos, size, style, name);
}

RichFrame::~RichFrame() {
    DeleteMenuBar();
}

void RichFrame::SetTopBarStyle(long style) {
    m_topBarStyle = style;
}

long RichFrame::GetTopBarStyle() const {
    return m_topBarStyle;
}

void RichFrame::DeleteMenuBar() {
    wxDELETE(m_frameMenuBar);
}

wxPoint RichFrame::GetClientAreaOrigin() const {
    wxPoint pt = wxTopLevelWindow::GetClientAreaOrigin();
    pt.y += 40;
    return pt;
}

void RichFrame::SetMenuBar(wxMenuBar* menuBar) {
    m_frameMenuBar = menuBar;
}

wxMenuBar* RichFrame::GetMenuBar() const {
    return m_frameMenuBar;
}

bool RichFrame::ProcessCommand(int id) {
}

void RichFrame::OnClose(wxCloseEvent& event) {
}

void RichFrame::OnIconize(wxIconizeEvent& event) {
}

void RichFrame::OnMaximize(wxMaximizeEvent& event) {
}

void RichFrame::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    DrawBorder(&dc);
    if (m_topBarStyle & wxCLOSE_BOX) {
        DrawCloseBox(&dc);
    }
    if (m_topBarStyle & wxMAXIMIZE_BOX) {
        DrawMaximizeBox(&dc);
    }
    if (m_topBarStyle & wxMINIMIZE_BOX) {
        DrawMinimizeBox(&dc);
    }
}

void RichFrame::DrawBorder(wxDC* pDC) {
    wxSize size = GetSize();
    size.IncBy(1, 1);
    wxPoint pt = GetPosition();
    wxPen solidPen(wxColor(255, 0, 0), 1, wxPENSTYLE_SOLID);
    pDC->DrawRectangle(pt, size);
}

void RichFrame::SetTopMenuBarBkColor(wxColor& color) {
}

void RichFrame::DrawIcon(wxDC* pDC) {
}

void RichFrame::OnMouseLeftDown(wxMouseEvent& event) {
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

void RichFrame::OnMouseLeftUp(wxMouseEvent&) {
    m_bLeftMouseDown = false;
    if (m_hitState == RichTopBarHitState::BOX_CLOSE) {
        wxCommandEvent event(wxEVT_CLOSE_WINDOW);
        wxPostEvent(GetParent(), event);
    }
    FinishDrag();
}

void RichFrame::OnMouseMove(wxMouseEvent& event) {
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

bool RichFrame::CanMove() {
    return m_hitState == RichTopBarHitState::DEFAULT;
}

void RichFrame::FinishDrag() {
    if (m_dragging) {
        m_dragging = false;
    }
    if (HasCapture()) {
        ReleaseMouse();
    }
}

void RichFrame::OnMouseCaptureLost(wxMouseCaptureLostEvent&) {
    FinishDrag();
}

void RichFrame::OnHitTest(wxMouseEvent& event) {
    wxPoint pt = event.GetPosition();

    wxRect rect(GetSize().GetWidth() - FRAME_BOX_OUTER_SIZE, FRAME_BOX_MARGIN, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
    m_hitState = RichTopBarHitState::DEFAULT;
    if (rect.Contains(pt)) {
        if (m_topBarStyle & wxCLOSE_BOX) {  // 关闭按钮
            m_hitState = RichTopBarHitState::BOX_CLOSE;
            return;
        } else if (m_topBarStyle & wxMAXIMIZE_BOX) {
            m_hitState = RichTopBarHitState::BOX_MAXIMIZE;
            return;
        } else if (m_topBarStyle & wxMINIMIZE_BOX) {
            m_hitState = RichTopBarHitState::BOX_MINIMIZE;
            return;
        }
    }

    rect.Offset(-FRAME_BOX_OUTER_SIZE, 0);  // 最大化按钮
    if (rect.Contains(pt)) {
        if (m_topBarStyle & wxMAXIMIZE_BOX) {
            m_hitState = RichTopBarHitState::BOX_MAXIMIZE;
            return;
        } else if (m_topBarStyle & wxMINIMIZE_BOX) {
            m_hitState = RichTopBarHitState::BOX_MINIMIZE;
            return;
        }
    }

    rect.Offset(-FRAME_BOX_OUTER_SIZE, 0);  // 最小化按钮
    if (rect.Contains(pt)) {
        if (m_topBarStyle & wxMINIMIZE_BOX) {
            m_hitState = RichTopBarHitState::BOX_MINIMIZE;
            return;
        }
    }
}

// 绘制顶部的按钮
void RichFrame::DrawMenuBar(wxDC* pDC, wxPoint& pt, wxSize& size) {
}

void RichFrame::DrawMaximizeBox(wxDC* pDC) {
    int offsetX = GetSize().GetWidth();
    if (m_topBarStyle & wxCLOSE_BOX) {
        offsetX -= FRAME_BOX_OUTER_SIZE;
    }
    offsetX -= FRAME_BOX_OUTER_SIZE;
    wxRect rect(offsetX, FRAME_BOX_MARGIN, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
    if (m_hitState == RichTopBarHitState::BOX_MAXIMIZE) {
        pDC->SetBrush(wxBrush(wxColor(242, 57, 64)));
        pDC->SetPen(*wxTRANSPARENT_PEN);
        wxRect rectBk(offsetX, FRAME_BOX_MARGIN, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
        pDC->DrawRectangle(rectBk);
    }
    wxPen pen = wxPen(wxColor(229, 229, 229), 1, wxPENSTYLE_SOLID);
    pDC->SetPen(pen);
    int offsetY = FRAME_BOX_SIZE / 2 + FRAME_BOX_MARGIN;
    pDC->DrawLine(offsetX + FRAME_BOX_SIZE >> 2, offsetY, offsetX + FRAME_BOX_SIZE >> 1, offsetY);
}

void RichFrame::DrawMinimizeBox(wxDC* pDC) {
    long style = GetWindowStyleFlag();
    int offsetX = GetSize().GetWidth();
    if (m_topBarStyle & wxCLOSE_BOX) {
        offsetX -= FRAME_BOX_OUTER_SIZE;
    }
    if (m_topBarStyle & wxMAXIMIZE_BOX) {
        offsetX -= FRAME_BOX_OUTER_SIZE;
    }

    offsetX -= FRAME_BOX_OUTER_SIZE;
    if (m_hitState == RichTopBarHitState::BOX_MINIMIZE) {
        pDC->SetBrush(wxBrush(wxColor(242, 57, 64)));
        pDC->SetPen(*wxTRANSPARENT_PEN);
        wxRect rectBk(offsetX, 4, FRAME_BOX_SIZE, FRAME_BOX_SIZE);
        pDC->DrawRectangle(rectBk);
    }
    wxPen pen = wxPen(wxColor(229, 229, 229), 1, wxPENSTYLE_SOLID);
    pDC->SetPen(pen);

    pDC->DrawLine(offsetX + FRAME_BOX_SIZE >> 2, 16, offsetX + 24, 16);
}

void RichFrame::DrawCloseBox(wxDC* pDC) {
    if (m_hitState == RichTopBarHitState::BOX_CLOSE) {
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

void RichFrame::DrawTitle(wxDC* pDC, wxPoint& pt, wxSize& size) {
}