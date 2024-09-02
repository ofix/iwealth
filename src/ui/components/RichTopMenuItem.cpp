///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/commponents/RichTopMenuItem.cpp
// Purpose:     customize top menu item
// Author:      songhuabiao
// Modified by:
// Created:     2024-09-02 19:40
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////
#include "RichTopMenuItem.h"

RichTopMenuItem::RichTopMenuItem(const std::string name) : m_menu_item_name(name), m_state(MenuItemState::Normal) {
}

RichTopMenuItem::~RichTopMenuItem() {
}

void RichTopMenuItem::Disable() {
    m_state = MenuItemState::Disabled;
}

void RichTopMenuItem::Hilight() {
    m_state = MenuItemState::Hilight;
}