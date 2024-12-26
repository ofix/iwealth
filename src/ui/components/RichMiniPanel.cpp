///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/commponents/RichMiniPanel.cpp
// Purpose:     base class for mini panels
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-26 20:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichMiniPanel.h"

RichMiniPanel::RichMiniPanel(wxWindow* parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
    : wxPanel(parent, id, pos, size, style, name) {
}

RichMiniPanel::~RichMiniPanel() {
}

void RichMiniPanel::SetTitle(wxString& title) {
    m_title = title;
}

wxString RichMiniPanel::GetTitle() {
    return m_title;
}