///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/events/RichShareSearchEvent.cpp
// Purpose:     custom radio control selection event
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/events/RichShareSearchEvent.h"
#include <iostream>

wxDEFINE_EVENT(wxEVT_RICH_SHARE_SEARCH, RichShareSearchEvent);

RichShareSearchEvent::RichShareSearchEvent(wxEventType type, int id) : wxEvent(id, type) {
}

// 自定义数据必须在这里进行设置，否则事件接收者无法正确收到
RichShareSearchEvent::RichShareSearchEvent(const RichShareSearchEvent& event) : wxEvent(event) {
    this->SetShareCode(event.GetShareCode());
}

std::string RichShareSearchEvent::GetShareCode() const {
    return m_share_code;
}
void RichShareSearchEvent::SetShareCode(const std::string& share_code) {
    m_share_code = share_code;
}