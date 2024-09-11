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

wxIMPLEMENT_DYNAMIC_CLASS(RichFrame, wxTopLevelWindow);

RichFrame::RichFrame() : wxTopLevelWindow() {
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
    m_bLeftMouseDown = false;
    m_dragging = false;
}

bool RichFrame::Create(wxWindow* parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name) {
    return wxTopLevelWindow::Create(parent, id, name, pos, size, style, name);
}

RichFrame::~RichFrame() {
    if (m_frameMenu != nullptr) {
        delete m_frameMenu;
        m_frameMenu = nullptr;
    }
}

void RichFrame::OnClose(wxCloseEvent& event) {
}

void RichFrame::OnIconize(wxIconizeEvent& event) {
}

void RichFrame::OnMaximize(wxMaximizeEvent& event) {
}

void RichFrame::OnPaint(wxPaintEvent& event) {
}

void RichFrame::DrawBorder(wxDC* pDC) {
}

void RichFrame::SetTopMenuBarBkColor(wxColor& color) {
}

void RichFrame::DrawIcon(wxDC* pDC) {
}

void RichFrame::DrawMenuBar(wxDC* pDC, wxPoint& pt, wxSize& size) {
}

void RichFrame::DrawMaximizeButton(wxDC* pDC, wxPoint& pt, wxSize& size) {
}

void RichFrame::DrawMimimizeButton(wxDC* pDC, wxPoint& pt, wxSize& size) {
}

void RichFrame::DrawCloseButton(wxDC* pDC, wxPoint& pt, wxSize& size) {
}

void RichFrame::DrawTitle(wxDC* pDC, wxPoint& pt, wxSize& size) {
}