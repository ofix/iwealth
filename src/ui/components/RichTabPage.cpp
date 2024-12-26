///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/commponents/RichTabPage.cpp
// Purpose:     custom tab page control like wxNoteBook
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-26 20:16
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichTabPage.h"

RichTabPage::RichTabPage(wxWindow* parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
    : wxPanel(parent, id, pos, size, style, name) {
}

RichTabPage::~RichTabPage() {
}

int RichTabPage::GetRowCount() const {
    return 0;
}

int RichTabPage::GetSelection() const {
    return m_selected;
}

int RichTabPage::SetSelection(size_t page) {
    m_selected = page;
    return m_selected;
}

int RichTabPage::ChangeSelection(size_t page) {
    m_selected = page;
    return m_selected;
}

bool RichTabPage::InsertPage(size_t index, wxWindow* page, const wxString& text, bool select) {
    return false;
}

wxString RichTabPage::GetSelectPageTitle() {
    return m_tabTitles[m_selected];
}

void RichTabPage::SetActivePageTitle(wxString& title) {
    m_tabTitles[m_selected] = title;
}