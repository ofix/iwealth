///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/ui/commponents/RichPanel.cpp
// Purpose:     iwealth common panel ancestoer, add panel type support
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichPanel.h"

RichPanel::RichPanel(PanelType type,
                     wxWindow* parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
    : wxWindow(parent, id, pos, size, style, name), m_type(type) {
}

PanelType RichPanel::GetType() {
    return m_type;
}