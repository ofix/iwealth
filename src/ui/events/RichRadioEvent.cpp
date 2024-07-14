///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/events/RichRadioEvent.cpp
// Purpose:     custom radio control selection event
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/events/RichRadioEvent.h"

wxDEFINE_EVENT(wxEVT_RICH_RADIO, RichRadioEvent);

RichRadioEvent::RichRadioEvent(wxEventType type, int id) : wxEvent(id, type) {
}

// 自定义数据必须在这里进行设置，否则事件接收者无法正确收到
RichRadioEvent::RichRadioEvent(const RichRadioEvent& event) : wxEvent(event) {
    this->SetSelection(event.GetSelection());
}

int RichRadioEvent::GetSelection() const {
    return m_selection;
}
void RichRadioEvent::SetSelection(int n) {
    m_selection = n;
}