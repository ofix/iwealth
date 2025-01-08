///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/components/RichDialog.cpp
// Purpose:     the common customize title bar dialog
// Author:      songhuabiao
// Modified by:
// Created:     2024-08-18 19:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichDialog.h"

///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/RichDialog.cpp
// Purpose:     dialog for display day kline information
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-14 16:59
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include <wx/dcbuffer.h>
#include "ui/RichHelper.h"
#include "ui/components/RichDialog.h"
#include "util/Color.h"
#include "util/Global.h"


BEGIN_EVENT_TABLE(RichDialog, wxDialog)
EVT_PAINT(RichDialog::OnPaint)
EVT_LEFT_DOWN(RichDialog::OnLeftMouseDown)
EVT_MOTION(RichDialog::OnLeftMouseMove)
EVT_LEFT_UP(RichDialog::OnLeftMouseUp)
END_EVENT_TABLE()

RichDialog::RichDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxDialog(parent, id, title, pos, size, 0) {
    // 必须调用此函数，否则无法重绘wxDialog及其子类
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_inMove = false;
    m_rectCloseBtn = wxRect(size.GetWidth() - 20, 0, 20, 20);
    m_titleBackgroundColor = wxColor(255, 50, 50);
    m_titleForegroundColor = wxColor(232, 232, 232);
}

RichDialog::~RichDialog() {
    // dtor
}

void RichDialog::SetTitleBackgroundColor(wxColor color) {
    m_titleBackgroundColor = color;
}

void RichDialog::SetTitleForegroundColor(wxColor color) {
    m_titleForegroundColor = color;
}

void RichDialog::DrawTitle(wxDC* pDC) {
    wxString title = GetTitle();
    int width = GetSize().GetWidth();
    int height = 20;
    wxRect rcTitle = wxRect(4, 0, width, height);
    wxBrush bkBrush(m_titleBackgroundColor);
    pDC->SetBrush(bkBrush);
    pDC->DrawRectangle(0, 0, width, height);
    pDC->SetTextForeground(m_titleForegroundColor);
    pDC->DrawLabel(title, rcTitle, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    DrawCloseButton(pDC);
}

void RichDialog::DrawCloseButton(wxDC* pDC) {
    wxPen close_pen(wxColor(250, 250, 250), 1, wxPENSTYLE_SOLID);
    pDC->SetPen(close_pen);
    wxRect innerRect(m_rectCloseBtn.GetX() + 4, m_rectCloseBtn.GetY() + 4, m_rectCloseBtn.GetWidth() - 8,
                     m_rectCloseBtn.GetHeight() - 8);
    pDC->DrawLine(innerRect.GetTopLeft(), innerRect.GetBottomRight());
    pDC->DrawLine(innerRect.GetBottomLeft(), innerRect.GetTopRight());
    pDC->DrawPoint(innerRect.GetBottomRight());
    pDC->DrawPoint(innerRect.GetTopRight());
}

void RichDialog::OnLeftMouseDown(wxMouseEvent& event) {
    wxPoint pt = event.GetPosition();
    if (!IsInCloseButton(pt) && IsInTitleBar(pt)) {
        m_inMove = true;
        m_ptPrev = pt;
    }
}

void RichDialog::OnLeftMouseMove(wxMouseEvent& event) {
    if (m_inMove) {
        wxPoint pt = event.GetPosition();
        int offsetX = pt.x - m_ptPrev.x;
        int offsetY = pt.y - m_ptPrev.y;
        this->Move(this->GetScreenPosition() + wxPoint(offsetX, offsetY));  // 移动窗口
    }
}

void RichDialog::OnLeftMouseUp(wxMouseEvent& event) {
    wxPoint pt = event.GetPosition();
    if (m_inMove) {
        m_inMove = false;
    }
    if (IsInCloseButton(pt)) {
        this->Close();
    }
}

bool RichDialog::IsInTitleBar(wxPoint& pt) {
    wxRect rectTitleBar = wxRect(GetSize());
    return rectTitleBar.Contains(pt);
}

bool RichDialog::IsInCloseButton(wxPoint& pt) {
    return m_rectCloseBtn.Contains(pt);
}

void RichDialog::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(KLINE_PANEL_BACKGROUND_COLOR));
    dc.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
    DrawTitle(&dc);
    OnDrawClient(&dc);
}

void RichDialog::OnDrawClient(wxDC* pDC) {
}