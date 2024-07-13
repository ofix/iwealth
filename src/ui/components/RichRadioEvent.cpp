///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/ui/commponents/RichRadioEvent.cpp
// Purpose:     custom radio control selection event
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichRadioEvent.h"

wxDEFINE_EVENT(wxEVT_RICH_RADIO, RichRadioEvent);

RichRadioEvent::RichRadioEvent(wxEventType type, int id) : wxEvent(id, type) {
}

RichRadioEvent::RichRadioEvent(const RichRadioEvent& event) : wxEvent(event) {
}

int RichRadioEvent::GetSelection() const {
    return m_selection;
}
void RichRadioEvent::SetSelection(int n) {
    m_selection = n;
}