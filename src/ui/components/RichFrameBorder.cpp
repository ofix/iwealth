///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/components/RichFrameBorder.cpp
// Purpose:     the main frame border
// Author:      songhuabiao
// Modified by:
// Created:     2024-09-05 19:44
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichFrameBorder.h"
#include <wx/dcbuffer.h>

RichFrameBorder::RichFrameBorder(wxWindow* parent,
                                 wxWindowID id,
                                 const wxPoint& pos const wxSize& size,
                                 wxColor& bg_clr)
    : m_bgClr(bg_clr) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Create(parent, id, pos, size);
    Bind(wxEVT_PAINT, &RichFrameBorder::OnPaint, this);
}

void RichFrameBorder::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc;
    wxBrush brush(m_bgClr);
    dc.SetBrush(brush);
    dc.DrawRectangle(GetPosition(), GetSize());
}
