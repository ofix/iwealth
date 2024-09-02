///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/commponents/RichTopMenu.cpp
// Purpose:     customize simple menu layout on title bar
// Author:      songhuabiao
// Modified by:
// Created:     2024-09-02 19:40
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////
#include "ui/components/RichTopMenu.h"
#include "ui/components/RichTopMenuItem.h"

RichTopMenu::RichTopMenu(const wxString& name, wxWindow* parent) : m_menuName(name), m_parent(parent) {
}

RichTopMenu::~RichTopMenu() {
}

void RichTopMenu::Append(const long menu_id, const std::string menu_name) {
}

wxSize RichTopMenu::DoGetBestClientSize() const {
    int width = 64;
    int height = m_menuItems.size() * 16;
    return wxSize(width, height);
}

void RichTopMenu::DisableMenuItem(const std::string& menu_item_name) {
}

void RichTopMenu::EnableMenuItem(const std::string& menu_item_name) {
}

int RichTopMenu::FindMenuItem(const std::string& menu_item_name) {
    return -1;
}

void RichTopMenu::OnPaint(wxPaintEvent& event) {
}
