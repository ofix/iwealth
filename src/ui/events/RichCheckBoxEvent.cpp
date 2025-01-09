///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/events/RichCheckBoxEvent.cpp
// Purpose:     custom radio control selection event
// Author:      songhuabiao
// Modified by:
// Created:     2025-01-09 20:22
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/events/RichCheckBoxEvent.h"

wxDEFINE_EVENT(wxEVT_RICH_CHECKBOX, RichCheckBoxEvent);

RichCheckBoxEvent::RichCheckBoxEvent(wxEventType type, int id) : wxEvent(id, type) {
}

// 自定义数据必须在这里进行设置，否则事件接收者无法正确收到
RichCheckBoxEvent::RichCheckBoxEvent(const RichCheckBoxEvent& event) : wxEvent(event) {
    int index;
    bool checked;
    event.GetChecked(index, checked);
    this->SetChecked(index, checked);
}

void RichCheckBoxEvent::GetChecked(int& index, bool& checked) const {
    index = m_index;
    checked = m_checked;
}

void RichCheckBoxEvent::SetChecked(int index, bool checked) {
    m_index = index;
    m_checked = checked;
}