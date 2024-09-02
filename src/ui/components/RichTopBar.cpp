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

RichTopBar::RichTopBar(wxWindow* parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
    : wxWindow(parent, id, pos, size, style, name), m_dragging(false) {
    SetBackgroundColour(wxColor(9, 69, 162));
    Bind(wxEVT_LEFT_DOWN, &RichTopBar::OnMouseLeftDown, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &RichTopBar::OnMouseCaptureLost, this);
}

RichTopBar::~RichTopBar() {
}

void RichTopBar::OnMouseLeftDown(wxMouseEvent& event) {
    if (!m_dragging) {
        Bind(wxEVT_LEFT_UP, &RichTopBar::OnMouseLeftUp, this);
        Bind(wxEVT_MOTION, &RichTopBar::OnMouseMove, this);
        m_dragging = true;

        wxPoint clientStart = event.GetPosition();
        m_dragStartMouse = ClientToScreen(clientStart);
        m_dragStartWindow = GetParent()->GetPosition();
        CaptureMouse();
    }
}

void RichTopBar::OnMouseLeftUp(wxMouseEvent&) {
    FinishDrag();
}

void RichTopBar::OnMouseMove(wxMouseEvent& event) {
    wxPoint curClientPsn = event.GetPosition();
    wxPoint curScreenPsn = ClientToScreen(curClientPsn);
    wxPoint movementVector = curScreenPsn - m_dragStartMouse;
    GetParent()->SetPosition(m_dragStartWindow + movementVector);
}

void RichTopBar::OnMouseCaptureLost(wxMouseCaptureLostEvent&) {
    FinishDrag();
}

void RichTopBar::FinishDrag() {
    if (m_dragging) {
        Unbind(wxEVT_LEFT_UP, &RichTopBar::OnMouseLeftUp, this);
        Unbind(wxEVT_MOTION, &RichTopBar::OnMouseMove, this);
        m_dragging = false;
    }
    if (HasCapture()) {
        ReleaseMouse();
    }
}

wxSize RichTopBar::DoGetBestClientSize() const {
    return wxSize(-1, 20);
}

void RichTopBar::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
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
}
// 最小化按钮
void RichTopBar::DrawMimizeBox(wxDC* pDC) {
}
// 最大化按钮
void RichTopBar::DrawMaximizeBox(wxDC* pDC) {
}
// 关闭按钮
void RichTopBar::DrawCloseBox(wxDC* pDC) {
}