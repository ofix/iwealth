///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/ui/commponents/RichStaticText.cpp
// Purpose:     NO KEYBOARD AND MOUSE FOCUS static text control
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichStaticText.h"

RichStaticText::RichStaticText(wxWindow* parent,
                               wxWindowID id,
                               const wxString& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
    : wxStaticText(parent, id, label, pos, size, style, name) {
}