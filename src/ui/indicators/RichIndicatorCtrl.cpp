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
    std::cout << "default: " << m_pKlineCtrl << std::endl;
}

RichIndicatorCtrl::RichIndicatorCtrl(RichKlineCtrl* pKlineCtrl, int x, int y, int w, int h)
    : m_pKlineCtrl(pKlineCtrl), m_x(x), m_y(y), m_width(w), m_height(h) {
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
    std::cout <<"custom: "<< m_pKlineCtrl << std::endl;
}

RichKlineCtrl* RichIndicatorCtrl::GetKlineCtrl() const {
    return m_pKlineCtrl;
}

RichIndicatorCtrl::RichIndicatorCtrl(RichKlineCtrl* pKlineCtrl, const wxPoint& pos, const wxSize& size) {
    RichIndicatorCtrl(pKlineCtrl, pos.x, pos.y, size.GetWidth(), size.GetHeight());
}

RichIndicatorCtrl::~RichIndicatorCtrl() {
}

void RichIndicatorCtrl::SetMode(int mode) {
    m_mode = mode;
}

void RichIndicatorCtrl::Draw(wxDC* pDC) {
}

std::string RichIndicatorCtrl::GetName() {
    return "";
}

std::string RichIndicatorCtrl::GetFormulaName() {
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