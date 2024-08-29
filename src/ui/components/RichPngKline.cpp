///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/commponents/RichPngKline.cpp
// Purpose:     support save share's all day klines to png file
// Author:      songhuabiao
// Modified by:
// Created:     2024-08-29 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichPngKline.h"
#include <wx/dcbuffer.h>
#include <wx/filename.h>
#include <iostream>

RichPngKline::RichPngKline(StockDataStorage* pStorage,
                           const std::string& png_file_path,
                           int width,
                           int height,
                           int margin)
    : m_pStorage(pStorage), m_pngFilePath(png_file_path), m_width(width), m_height(height), m_margin(margin) {
    m_shareList = {};
}

RichPngKline::~RichPngKline() {
}

void RichPngKline::AddShare(const std::string& share_code) {
    m_shareList.push_back(share_code);
}

void RichPngKline::AddShares(const std::vector<std::string>& share_list) {
    m_shareList = share_list;
}

bool RichPngKline::Save() {
    int widthPng = m_width;
    int heightPng = (m_height + m_margin) * m_shareList.size() + m_margin;
    wxBitmap bitmap(widthPng, heightPng);
    wxMemoryDC dc(bitmap);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    for (size_t i = 0; i < m_shareList.size(); i++) {
        std::string share_code = m_shareList[i];
        std::vector<uiKline>* pKlines;
        RichResult status = m_pStorage->QueryKlines(share_code, KlineType::Day, &pKlines);
        if (!status.Ok()) {
            std::cout << status.What() << std::endl;
            return false;
        }
        DrawPng(&dc, i, pKlines);
    }
    // 输出到PNG路径
    wxImage image = bitmap.ConvertToImage();
    wxFileName file_name(m_pngFilePath);
    std::cout << "save png to " << file_name.GetFullPath() << std::endl;
    image.SaveFile(file_name.GetFullPath(), wxBITMAP_TYPE_PNG);
    return true;
}

//
void RichPngKline::DrawPng(wxDC* pDC, int n, std::vector<uiKline>* pKlines) {
    double w = static_cast<double>(m_width - m_margin * 2) / pKlines->size();
    double min_price = GetMinPrice(pKlines);
    double max_price = GetMaxPrice(pKlines);
    double hPrice = max_price - min_price;
    double hZoomRatio = m_height / hPrice;
    double offsetY = n * m_height + m_margin;  //
    for (size_t i = 0; i < pKlines->size(); i++) {
        uiKline currentKline = pKlines->at(i);
        uiKline nextKline = pKlines->at(i + 1);
        double x1 = m_margin + w * i;
        double y1 = (max_price - currentKline.price_close) * hZoomRatio + offsetY;
        double x2 = w * (i + 1);
        double y2 = (max_price - nextKline.price_close) * hZoomRatio + offsetY;
        pDC->DrawLine(x1, y1, x2, y2);
    }
}

double RichPngKline::GetMinPrice(std::vector<uiKline>* pKlines) {
    double min = 100000000;
    for (size_t i = 0; i < pKlines->size(); i++) {
        uiKline kline = pKlines->at(i);
        if (kline.price_min < min) {
            min = kline.price_min;
        }
    }
    return min;
}

double RichPngKline::GetMaxPrice(std::vector<uiKline>* pKlines) {
    double max = 0;
    for (size_t i = 0; i < pKlines->size(); i++) {
        uiKline kline = pKlines->at(i);
        if (kline.price_max > max) {
            max = kline.price_max;
        }
    }
    return max;
}