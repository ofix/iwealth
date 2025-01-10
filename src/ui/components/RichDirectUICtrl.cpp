///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/commponents/RichDirectUICtrl.cpp
// Purpose:     base Direct UI control
// Author:      songhuabiao
// Modified by:
// Created:     2025-01-10 21:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichDirectUICtrl.h"

RichDirectUICtrl::RichDirectUICtrl(RichDirectUICtrl* parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   const wxString& name)
    : m_parent(parent), m_id(id) {
    m_x = pos.x;
    m_y = pos.y;
    m_width = size.GetWidth();
    m_height = size.GetHeight();
    m_name = name;
}
RichDirectUICtrl::~RichDirectUICtrl() {
    for (auto child : m_children) {
        delete child;
    }
}
void RichDirectUICtrl::AddChild(RichDirectUICtrl* child) {
    m_children.push_back(child);
    child->m_parent = this;
}

void RichDirectUICtrl::SetParent(RichDirectUICtrl* parent) {
    this->m_parent = parent;
}

void RichDirectUICtrl::SetPosition(int x, int y) {
    m_x = x;
    m_y = y;
}

void RichDirectUICtrl::SetSize(int width, int height) {
    m_width = width;
    m_height = height;
}

void RichDirectUICtrl::Layout() {
    for (auto child : m_children) {
        child->Layout();
    }
}

void RichDirectUICtrl::Draw(wxDC* pDC) {
    pDC->SetBrush(*wxLIGHT_GREY_BRUSH);
    pDC->DrawRectangle(m_x, m_y, m_width, m_height);

    for (auto child : m_children) {
        child->Draw(pDC);
    }
}

bool RichDirectUICtrl::onMouseEvent(wxMouseEvent& event) {
}