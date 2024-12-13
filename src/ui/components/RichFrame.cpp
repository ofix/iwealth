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

RichFrame::RichFrame() {
    Init();
}

RichFrame::RichFrame(wxWindow* parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name) {
    Init();
    Create(parent, id, pos, size, style, name);
}

void RichFrame::Init() {
    m_isMaximized = false;
    m_frameMenuBar = NULL;
    m_bLeftMouseDown = false;
    m_dragging = false;
    m_topBarStyle = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX;
    m_pTopBar = NULL;
    m_pVerticalSizer = NULL;
    Bind(wxEVT_MAXIMIZE, &RichFrame::OnMaximize, this);
    Bind(wxEVT_ICONIZE, &RichFrame::OnIconize, this);
}

bool RichFrame::Create(wxWindow* parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name) {
    // style &= ~(wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxCLOSE_BOX);  // 移除三个按钮,防止外边框出现
    bool result = wxTopLevelWindow::Create(parent, id, name, pos, size, style, name);
    if (!result) {
        return false;
    }
    return true;
    // return CreateTopBar();
}

bool RichFrame::CreateTopBar() {
    m_pTopBar = new RichTopBar(this, wxID_ANY, wxPoint(0, 0), wxSize(-1, 40), 0, _("猛龙证券"));
    if (!m_pTopBar) {
        return false;
    }
    m_pTopBar->SetTopBarStyle(wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX);
    // 自动横向扩展
    m_pVerticalSizer = new wxBoxSizer(wxVERTICAL);
    m_pVerticalSizer->Add(m_pTopBar, 0, wxEXPAND | wxALL, 0);
    SetSizer(m_pVerticalSizer);
    return true;
}

RichFrame::~RichFrame() {
    DeleteTopBar();
}

void RichFrame::DeleteTopBar() {
    wxDELETE(m_pTopBar);
}

// wxPoint RichFrame::GetClientAreaOrigin() const {
//     wxPoint pt = wxTopLevelWindow::GetClientAreaOrigin();
//     pt.y += 40;
//     return pt;
// }

// void RichFrame::DoGetClientSize(int* width, int* height) const {
//     wxTopLevelWindow::DoGetClientSize(width, height);
//     if (height) {
//         (*height) -= 40;
//     }
// }

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
    Iconize(true);
}

// void RichFrame::OnRestoreWindow(wxTaskBarIconEvent& event) {
//     std::cout << "TaskBarIconEvent clicked" << std::endl;
//     if (IsIconized()) {
//         std::cout << "Active Frame" << std::endl;
//         Iconize(false);
//         Restore();
//         Maximize(false);
//         Show();
//     }
// }

void RichFrame::OnMaximize(wxMaximizeEvent& event) {
    if (IsMaximized()) {
        Maximize(false);
    } else {
        Maximize(true);
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

void RichFrame::Maximize(bool maximize) {
    int x, y, w, h;
    wxClientDisplayRect(&x, &y, &w, &h);

    if (maximize && !m_isMaximized) {
        m_isMaximized = true;

        GetPosition(&m_savedFrame.x, &m_savedFrame.y);
        GetSize(&m_savedFrame.width, &m_savedFrame.height);

        SetSize(x, y, w, h);
    } else if (!maximize && m_isMaximized) {
        m_isMaximized = false;
        SetSize(m_savedFrame.x, m_savedFrame.y, m_savedFrame.width, m_savedFrame.height);
    }
}

bool RichFrame::IsMaximized() const {
    return m_isMaximized;
}

void RichFrame::Restore() {
    if (IsMaximized()) {
        Maximize(false);
    }
}

void RichFrame::OnMouseLeftDown(wxMouseEvent& event) {
    // m_bLeftMouseDown = true;
    // if (!m_dragging) {
    //     if (CanMove()) {
    //         m_dragging = true;
    //         wxPoint clientStart = event.GetPosition();
    //         m_dragStartMouse = ClientToScreen(clientStart);
    //         m_dragStartWindow = GetParent()->GetPosition();
    //         CaptureMouse();
    //     }
    // }
}

void RichFrame::OnMouseLeftUp(wxMouseEvent&) {
    // m_bLeftMouseDown = false;
    // if (m_hitState == RichTopBarHitState::BOX_CLOSE) {
    //     wxCommandEvent event(wxEVT_CLOSE_WINDOW);
    //     wxPostEvent(GetParent(), event);
    // }
    // FinishDrag();
}

void RichFrame::OnMouseMove(wxMouseEvent& event) {
    // if (m_dragging) {
    //     wxPoint curClientPsn = event.GetPosition();
    //     wxPoint curScreenPsn = ClientToScreen(curClientPsn);
    //     wxPoint movementVector = curScreenPsn - m_dragStartMouse;
    //     GetParent()->SetPosition(m_dragStartWindow + movementVector);
    // } else {
    //     OnHitTest(event);
    //     Refresh();
    // }
}

bool RichFrame::CanMove() {
    // return m_hitState == RichTopBarHitState::DEFAULT;
}

void RichFrame::FinishDrag() {
    // if (m_dragging) {
    //     m_dragging = false;
    // }
    // if (HasCapture()) {
    //     ReleaseMouse();
    // }
}

void RichFrame::OnMouseCaptureLost(wxMouseCaptureLostEvent&) {
    // FinishDrag();
}

void RichFrame::OnHitTest(wxMouseEvent& event) {
}