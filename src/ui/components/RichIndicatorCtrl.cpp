///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/components/RichIndicatorCtrl.cpp
// Purpose:     base class for kline indicators (volume/amount/macd/kdj)
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-12 19:44
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

RichIndicatorCtrl::RichIndicatorCtrl() {
}

RichIndicatorCtrl::RichIndicatorCtrl(int x, int y, int w, int h) : m_x(x), m_y(y), m_width(w), m_height(h) {
    m_upArrowX = m_x + m_width - 64;
    m_upArrowY = m_y;
    m_upArrowWidth = 16;
    m_upArrowHeight = 16;
    m_downArrowX = m_x + m_width - 48;
    m_downArrowY = m_y;
    m_downArrowWidth = 16;
    m_downArrowHeight = 16;
    m_heightManual = 0;
}

virtual RichIndicatorCtrl::~RichIndicatorCtrl() {
}

void RichIndicatorCtrl::Draw(wxDC* pDC) {
}

// 绘制上箭头
void RichIndicatorCtrl::DrawUpArrow(wxDC* pDC) {
}

// 绘制下箭头
void RichIndicatorCtrl::DrawDownArrow(wxDC* pDC) {
}

// 用户手动调整附图指标的高度后，后续附图指标高度不再自适应
void RichIndicatorCtrl::SetManualHeight(int height) {
    // 检查高度是否小于最小高度，如果小于最小高度，取最小高度
    if (height < m_minHeight) {
        height = m_minHeight;
    }
    // 检查高度是否大于最大高度，如果大于最大高度，取最大高度
    if (height > m_maxHeight) {
        height = m_maxHeight;
    }
    m_heightManual = height;
}

int RichIndicatorCtrl::GetMaxHeight() const {
    return m_maxHeight;
}

int RichIndicatorCtrl::GetMinHeight() const {
    return m_minHeight;
}

void RichIndicatorCtrl::SetMaxHeight(int height) {
    m_maxHeight = height;
}

void RichIndicatorCtrl::SetMinHeight(int height) {
    m_minHeight = height;
}

int RichIndicatorCtrl::GetX() const {
    return m_x;
}

int RichIndicatorCtrl::GetY() const {
    return m_y;
}

int RichIndicatorCtrl::GetWidth() const {
    return m_width;
}

int RichIndicatorCtrl::GetHeight() const {
    return m_height;
}

bool RichIndicatorCtrl::IsVisible() const {
    return m_visible;
}

void RichIndicatorCtrl::Hide() {
    m_visible = false;
}

void RichIndicatorCtrl::Show() {
    m_visible = true;
}