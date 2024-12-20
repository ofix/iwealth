///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/indicators/RichIndicatorCtrl.cpp
// Purpose:     base class for kline indicators (volume/amount/macd/kdj)
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-12 19:44
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////
#include "RichIndicatorCtrl.h"
#include <iostream>

RichIndicatorCtrl::RichIndicatorCtrl() {
    m_pKlineCtrl = nullptr;
    m_mode = 0;
    m_upArrowX = m_x + m_width - 64;
    m_upArrowY = m_y;
    m_upArrowWidth = 16;
    m_upArrowHeight = 16;
    m_downArrowX = m_x + m_width - 48;
    m_downArrowY = m_y;
    m_downArrowWidth = 16;
    m_downArrowHeight = 16;
    m_heightManual = 0;
    m_minHeight = 48;
    m_maxHeight = 320;
}

RichIndicatorCtrl::RichIndicatorCtrl(RichKlineCtrl* pKlineCtrl, int x, int y, int w, int h)
    : m_pKlineCtrl(pKlineCtrl),
      m_x(x),
      m_y(y),
      m_width(w),
      m_height(h),
      m_titleHeight(20),
      m_minHeight(48),
      m_maxHeight(320) {
    m_mode = 0;
    m_bodyHeight = m_height - m_titleHeight;
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

RichIndicatorCtrl::~RichIndicatorCtrl() {
}

RichKlineCtrl* RichIndicatorCtrl::GetKlineCtrl() const {
    return m_pKlineCtrl;
}

// Note: 构造函数调用另一个构造函数必须在在初始化列表中进行，如果在函数体中调用，构造会失败
RichIndicatorCtrl::RichIndicatorCtrl(RichKlineCtrl* pKlineCtrl, const wxPoint& pos, const wxSize& size)
    : RichIndicatorCtrl(pKlineCtrl, pos.x, pos.y, size.GetWidth(), size.GetHeight()) {
}

void RichIndicatorCtrl::SetMode(int mode) {
    m_mode = mode;
}

void RichIndicatorCtrl::Draw(wxDC* pDC) {
}

void RichIndicatorCtrl::DrawColorTextArr(wxDC* pDC, int x, int y, std::vector<RichColorText>& colorTexts) {
    int offsetX = x;
    for (size_t i = 0; i < colorTexts.size(); i++) {
        pDC->SetTextForeground(colorTexts[i].color);
        pDC->DrawText(colorTexts[i].text, offsetX, y);
        wxSize text_size = pDC->GetTextExtent(colorTexts[i].text);
        offsetX += text_size.GetWidth() + 12;
    }
}

wxString RichIndicatorCtrl::GetName() {
    return "";
}

wxString RichIndicatorCtrl::GetFormulaName() {
    return "";
}

// 绘制上箭头
void RichIndicatorCtrl::DrawUpArrow(wxDC* pDC) {
}

// 绘制下箭头
void RichIndicatorCtrl::DrawDownArrow(wxDC* pDC) {
}

void RichIndicatorCtrl::SetPosition(wxPoint& pos) {
    m_x = pos.x;
    m_y = pos.y;
}

void RichIndicatorCtrl::SetPosition(int x, int y) {
    m_x = x;
    m_y = y;
}

void RichIndicatorCtrl::SetWidth(int width) {
    m_width = width;
}

void RichIndicatorCtrl::SetMaxHeight(int height) {
    m_maxHeight = height;
}

void RichIndicatorCtrl::SetMinHeight(int height) {
    m_minHeight = height;
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
    m_height = height;
    m_bodyHeight = height - m_titleHeight;
}

int RichIndicatorCtrl::GetMaxHeight() const {
    return m_maxHeight;
}

int RichIndicatorCtrl::GetMinHeight() const {
    return m_minHeight;
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