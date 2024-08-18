///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/components/RichRichKlineCtrl.cpp
// Purpose:     the main kline control
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-14 19:44
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichKlineCtrl.h"
#include <wx/geometry.h>
#include <wx/graphics.h>
#include "formula/FormulaEma.h"
#include "stock/StockDataStorage.h"

RichKlineCtrl::~RichKlineCtrl() {
}

RichKlineCtrl::RichKlineCtrl(StockDataStorage* pStorage, const wxPoint& pos, const wxSize& size)
    : m_pos(pos), m_width(size.GetWidth()), m_height(size.GetHeight()), m_pStorage(pStorage), m_pKlines(nullptr) {
    Init();
}

void RichKlineCtrl::Init() {
    m_mode = KlineType::Day;
    m_klineWidth = 7;
    m_visibleKlineCount = 120;
    m_shareCode = "";
    m_pShare = nullptr;
    m_klineRng.begin = 0;
    m_klineRng.end = 0;
    m_curKline = -1;
    m_maxRectPrice = 0;
    m_minRectPrice = 0;
    m_paddingBottom = 16;
    m_paddingRight = 48;
    m_crossLine = NO_CROSS_LINE;
}

uiKline* RichKlineCtrl::GetCurrentKline() {
    if (m_pKlines == nullptr) {
        return nullptr;
    }
    if (m_crossLine == NO_CROSS_LINE) {
        return &m_pKlines->at(m_pKlines->size() - 1);
    }
    return &m_pKlines->at(m_crossLine);
}

uiKline* RichKlineCtrl::GetPreviousKline() {
    if (m_pKlines == nullptr) {
        return nullptr;
    }
    if (m_crossLine == NO_CROSS_LINE) {
        return &m_pKlines->at(m_pKlines->size() - 2);
    }
    return &m_pKlines->at(m_crossLine - 1);
}

/**
 * @brief 1. 日K线需要保存到磁盘上，周K线|月K线|季K线|年K线通过日K线计算得来
 * 2. 分时K线实时请求，并缓存到内存中，采用LRU算法进行淘汰，保留最多200个股票分时图
 * 分时图占用内存为 4*60*200 = 48000/1000 = 1MB左右
 *
 * @param share_code
 * @param kline_type
 */
bool RichKlineCtrl::LoadKlines(const std::string& share_code, const KlineType& kline_type) {
    if (share_code == "") {
        return false;
    }

    if (m_shareCode != share_code) {  // 加载不同股票的分时和K线图前，需清空缓存数据
        m_shareCode = share_code;
        m_crossLine = NO_CROSS_LINE;
    }

    m_pShare = m_pStorage->FindShare(share_code);
    RichResult status;
    if (kline_type == KlineType::Minute || kline_type == KlineType::FiveDay) {
        status = m_pStorage->QueryMinuteKlines(share_code, kline_type, &m_pMinuteKlines);
    } else {
        status = m_pStorage->QueryKlines(share_code, kline_type, &m_pKlines);
    }
    if (!status.Ok()) {
        std::cout << status.What() << std::endl;
        return false;
    }

    m_emaCurves.clear();
    SetMode(kline_type);

    m_klineRng.end = m_pKlines->size() - 1;
    m_klineRng.begin = (m_klineRng.end >= 119) ? m_klineRng.end - 119 : 0;
    m_visibleKlineCount = m_klineRng.end - m_klineRng.begin + 1;
    CalcVisibleKlineWidth();

    // 添加 EMA 曲线的时候会自动计算相关数据
    AddEmaCurve(99, wxColor(255, 0, 255));
    AddEmaCurve(255, wxColor(255, 255, 0));
    AddEmaCurve(905, wxColor(0, 255, 0));
    return true;
}
// 上一个股票K线
bool RichKlineCtrl::LoadPrevKlines() {
    m_pShare = m_pStorage->FindPrevShare(m_pShare);
    if (m_pShare == nullptr) {
        return false;
    }
    return LoadKlines(m_pShare->code, m_mode);
}

// 下一个股票K线
bool RichKlineCtrl::LoadNextKlines() {
    m_pShare = m_pStorage->FindNextShare(m_pShare);
    if (m_pShare == nullptr) {
        return false;
    }
    return LoadKlines(m_pShare->code, m_mode);
}

// 股票代码保持不变，切换K线类型
bool RichKlineCtrl::LoadKlines(const KlineType& kline_type) {
    return LoadKlines(m_shareCode, kline_type);
}

std::string RichKlineCtrl::GetShareCode() const {
    return m_shareCode;
}

Share* RichKlineCtrl::GetCurrentShare() const {
    return m_pShare;
}

/**
 *@param mode KlineType the Kline draw mode
 */
void RichKlineCtrl::SetMode(KlineType mode) {
    m_mode = mode;
}

/**
 *@return int the Kline draw mode
 */
KlineType RichKlineCtrl::GetMode() const {
    return m_mode;
}

/**
 * @todo  打印调试信息
 * @param string prefix 前缀
 */
void RichKlineCtrl::PrintDebugInfo(std::string prefix) {
    std::cout << "[" << prefix << "] Range: " << m_klineRng.begin << "," << m_klineRng.end;
    std::cout << ",Total:" << m_klineRng.end - m_klineRng.begin;
    std::cout << ",KlineWidth: " << m_klineWidth;
    std::cout << ",klineInnerWidth: " << m_klineInnerWidth;
    std::cout << ",CrossLine: " << m_crossLine << std::endl;
}

/**
 *@todo 放大日/周/月/季/年K线,K线数量变少
 *@return KlineRange
 */
void RichKlineCtrl::ZoomIn() {
    if (m_crossLine == NO_CROSS_LINE) {
        m_crossLine = m_pKlines->size() - 1;  // 放大中心为最右边K线
    }
    // 可见K线数量少于8，不再放大
    if (m_klineRng.end <= m_klineRng.begin + 8) {
        return;
    }

    // 计算放大中心两边的K线数量
    double leftKlineCount = m_crossLine - m_klineRng.begin;
    double rightKlineCount = m_klineRng.end - m_crossLine;
    // 取中心点左右两侧K线较多的一边进行延展，保住中心的地位
    double size = leftKlineCount > rightKlineCount ? leftKlineCount : rightKlineCount;
    m_klineRng.begin = m_crossLine - size / 2;
    m_klineRng.end = m_crossLine + size / 2;

    if (m_klineRng.begin > m_klineRng.end) {
        m_klineRng.begin = m_klineRng.end - 8;
    }
    // 边界处理
    if (m_klineRng.begin < 0) {
        m_klineRng.begin = 0;
    }
    if (m_klineRng.end > m_pKlines->size() - 1) {
        m_klineRng.end = m_pKlines->size() - 1;
    }

    m_visibleKlineCount = m_klineRng.end - m_klineRng.begin + 1;
    CalcVisibleKlineWidth();
}

/**
 * @todo 缩小日/周/月/季/年K线，K线数量变多
 * @param int
 */
void RichKlineCtrl::ZoomOut() {
    if (m_crossLine == NO_CROSS_LINE) {
        m_crossLine = m_pKlines->size() - 1;  // 放大中心为最右边K线
    }
    // 可见K线数量已全部显示，不再缩小
    if (m_klineRng.end - m_klineRng.begin + 1 == m_pKlines->size()) {
        return;
    }

    // 计算放大中心两边的K线数量
    double leftKlineCount = m_crossLine - m_klineRng.begin;
    double rightKlineCount = m_klineRng.end - m_crossLine;
    // 取中心点左右两侧K线较多的一边进行延展，保住中心的地位
    double size = leftKlineCount > rightKlineCount ? leftKlineCount : rightKlineCount;
    m_klineRng.begin = m_crossLine - size * 2;
    m_klineRng.end = m_crossLine + size * 2;
    // 边界处理
    if (m_klineRng.begin < 0) {
        m_klineRng.begin = 0;
    }
    if (m_klineRng.end > m_pKlines->size() - 1) {
        m_klineRng.end = m_pKlines->size() - 1;
    }
    m_visibleKlineCount = m_klineRng.end - m_klineRng.begin + 1;
    CalcVisibleKlineWidth();
}

void RichKlineCtrl::OnBackground(wxEraseEvent& event) {
    wxDC* pDC = event.GetDC();
    pDC->SetBrush(wxBrush(wxColour(0, 0, 0)));
}

void RichKlineCtrl::OnSize(wxSizeEvent& event) {
    wxSize size = event.GetSize();
    m_width = size.GetWidth();
    m_height = size.GetHeight();
}

void RichKlineCtrl::OnKeyDown(wxKeyEvent& event) {
    int max = m_pKlines->size() - 1;
    int key = event.GetKeyCode();
    if (key == WXK_LEFT) {
        if (m_crossLine == NO_CROSS_LINE) {
            m_crossLine = m_pKlines->size() - 1;
            m_crossLinePt = GetCrossLinePt(m_crossLine);
            return;
        }
        if (m_crossLine == m_klineRng.begin && m_klineRng.begin > 0) {
            m_klineRng.begin -= 1;
            m_klineRng.end -= 1;
        }
        if (m_crossLine > 0) {
            m_crossLine -= 1;
            m_crossLinePt = GetCrossLinePt(m_crossLine);
        }
    } else if (key == WXK_RIGHT) {  // 右移
        if (m_crossLine == m_klineRng.end && m_klineRng.end != max) {
            m_klineRng.begin += 1;
            m_klineRng.end += 1;
        } else if (m_crossLine >= m_klineRng.begin && m_crossLine <= m_klineRng.end) {
            if (m_crossLine != max - 1) {
                m_crossLine += 1;
                m_crossLinePt = GetCrossLinePt(m_crossLine);
            }
        }
    } else if (key == WXK_UP) {  // 放大，K线数量变少
        ZoomIn();
    } else if (key == WXK_DOWN) {  // 缩小，K线数量变多
        ZoomOut();
    } else if (key == WXK_HOME) {
        m_crossLine = m_klineRng.begin;
    } else if (key == WXK_END) {
        m_crossLine = m_klineRng.end;
    } else if (key == WXK_ESCAPE) {
        if (m_crossLine != NO_CROSS_LINE) {
            m_crossLine = NO_CROSS_LINE;
        }
    }

    if (m_crossLine != NO_CROSS_LINE) {
        if (m_crossLine >= m_klineRng.begin && m_crossLine <= m_klineRng.end) {
            m_crossLinePt = GetCrossLinePt(m_crossLine);  // 修正十字线的位置
        }
    }
}

bool RichKlineCtrl::IsFocus() {
    return m_crossLine != NO_CROSS_LINE;
}

void RichKlineCtrl::OnLeftMouseDown(wxMouseEvent& event) {
    if (event.LeftIsDown()) {  // 左键按下
        long x, y;
        event.GetPosition(&x, &y);
        // 获取最靠近的K线
        int k = x / m_klineWidth;
        m_crossLine = m_klineRng.begin + k;
        if (m_crossLine > m_pKlines->size() - 1) {
            m_crossLine = m_pKlines->size() - 1;
        }
        m_crossLinePt = GetCrossLinePt(m_crossLine);
    }
}

int RichKlineCtrl::GetInnerWidth() {
    return m_width - m_paddingRight;
}

int RichKlineCtrl::GetInnerHeight() {
    return m_height * 0.7 - m_pos.y - m_paddingBottom;
}

/**
 *@todo 获取日K线最低价
 *@param vector<uiKline> uiKlines 日/周/月/季/年K线数据
 *@param int begin 开始位置
 *@param int end 结束位置
 *@author songhuabiao
 */
float RichKlineCtrl::GetRectMinPrice(std::vector<uiKline>& uiKlines, int begin, int end) {
    float min = 100000000;
    for (int i = begin; i <= end; i++) {
        if (uiKlines.at(i).price_min < min) {
            min = uiKlines.at(i).price_min;
            m_minRectPriceIndex = i - begin;
        }
    }
    m_minKlinePrice = min;
    if (HasEmaCurve()) {
        for (auto& curve : m_emaCurves) {
            if (curve.visible) {
                for (int i = begin; i <= end; i++) {
                    if (curve.ema_price.at(i) < min) {
                        min = curve.ema_price.at(i);
                    }
                }
            }
        }
    }
    m_minRectPrice = min;
    return min;
}

/**
 *@todo 获取日K线最高价
 *@param vector<uiKline> uiKlines 日/周/月/季/年K线数据
 *@param int begin 开始位置
 *@param int end 结束位置
 *@author songhuabiao
 */
float RichKlineCtrl::GetRectMaxPrice(std::vector<uiKline>& uiKlines, int begin, int end) {
    float max = -100000000;
    for (int i = begin; i <= end; i++) {
        if (uiKlines.at(i).price_max > max) {
            max = uiKlines.at(i).price_max;
            m_maxRectPriceIndex = i - begin;
        }
    }
    m_maxKlinePrice = max;
    if (HasEmaCurve()) {
        for (auto& curve : m_emaCurves) {
            if (curve.visible) {
                for (int i = begin; i <= end; i++) {
                    if (curve.ema_price.at(i) > max) {
                        max = curve.ema_price.at(i);
                    }
                }
            }
        }
    }
    m_maxRectPrice = max;
    return max;
}

// 获取显示区域最低价格
float RichKlineCtrl::GetRectMinPrice(std::vector<minuteKline>& minuteKlines, int begin, int end) {
    float min = 100000000;
    for (int i = begin; i <= end; i++) {
        if (minuteKlines.at(i).price < min) {
            min = minuteKlines.at(i).price;
        }
    }
    return min;
}
// 获取显示区域最高价格
float RichKlineCtrl::GetRectMaxPrice(std::vector<minuteKline>& minuteKlines, int begin, int end) {
    float max = -100000000;
    for (int i = begin; i <= end; i++) {
        if (minuteKlines.at(i).price > max) {
            max = minuteKlines.at(i).price;
        }
    }
    return max;
}

/**
 * @func 获取第N条日K线的十字线位置
 * @param long n 从左往右数第N条K线
 * @param return wxPoint
 * @author songhuabiao
 */
wxPoint RichKlineCtrl::GetCrossLinePt(long n) {
    double x, y;
    uiKline item = m_pKlines->at(n);
    double hPrice = m_maxRectPrice - m_minRectPrice;
    y = m_pos.y + (1 - (item.price_close - m_minRectPrice) / hPrice) * GetInnerHeight();
    x = static_cast<int>((m_klineWidth) * (n - m_klineRng.begin) + m_klineInnerWidth / 2);  // 一定可以显示完全
    return wxPoint(x, y);
}

void RichKlineCtrl::CalcVisibleKlineWidth() {
    if (m_pKlines->size() < 20) {
        m_klineWidth = 10;
        m_klineInnerWidth = 7;
    } else {
        m_klineWidth = static_cast<double>(GetInnerWidth()) / m_visibleKlineCount;
        m_klineInnerWidth = m_klineWidth * 0.8;
        if (m_klineWidth > 1 && static_cast<int>(m_klineInnerWidth) % 2 == 0) {
            m_klineInnerWidth = static_cast<int>(m_klineInnerWidth);
            m_klineInnerWidth -= 1;
            if (m_klineInnerWidth < 1) {
                m_klineInnerWidth = 1;
            }
        }
    }
    // 根据K线宽度计算起始坐标和放大坐标
    if (m_crossLine == NO_CROSS_LINE) {
        m_klineRng.begin = static_cast<int>(m_pKlines->size() - m_visibleKlineCount);
        m_klineRng.end = m_pKlines->size() - 1;
        if (m_klineRng.begin < 0) {
            m_klineRng.begin = 0;
        }
    }
}

void RichKlineCtrl::OnPaint(wxDC* pDC) {
    pDC->SetBackground(*wxBLACK_BRUSH);
    pDC->Clear();
    if (m_mode == KlineType::Minute) {
        DrawMinuteKlines(pDC);
    } else if (m_mode == KlineType::FiveDay) {
        DrawFiveDayMinuteKlines(pDC);
    } else if (m_mode == KlineType::Day || m_mode == KlineType::Week || m_mode == KlineType::Month ||
               m_mode == KlineType::Quarter || m_mode == KlineType::Year) {
        if (m_pKlines == nullptr || m_pKlines->size() == 0) {
            DrawKlineBackground(pDC, 4, 20);
            return;
        }
        DrawDayKlines(pDC);
    }
}

// 绘制日K线|周K线|月K线|季K线|年K线背景图
void RichKlineCtrl::DrawKlineBackground(wxDC* pDC, double min_price, double max_price) {
    wxPen solid_pen(wxColor(255, 0, 0), 1, wxPENSTYLE_DOT);
    pDC->SetPen(solid_pen);
    pDC->SetBrush(*wxBLACK_BRUSH);
    int minX = 0;
    int minY = m_pos.y;
    int wRect = GetInnerWidth();
    int hRect = GetInnerHeight() + m_paddingBottom;
    pDC->DrawRectangle(minX, minY, wRect, hRect);
#define NLINES 14
    // 绘制横线和价格
    double avg_price = (max_price - min_price) / NLINES;
    double avg_height = hRect / NLINES;

    double y = minY;
    wxPen dash_pen(wxColor(39, 39, 39), 1, wxPENSTYLE_SHORT_DASH);
    pDC->SetPen(dash_pen);
    for (int i = 0; i < NLINES; i++) {
        y = y + avg_height;
        pDC->DrawLine(0, y, wRect, y);
    }

    pDC->SetTextForeground(wxColor(222, 222, 222));
    pDC->DrawText(convert_double(max_price, 2), wRect + 4, minY - 8);
    y = minY;
    double price = max_price;
    for (int i = 0; i < NLINES; i++) {
        y = y + avg_height;
        price = price - avg_price;
        pDC->DrawText(convert_double(price, 2), wRect + 4, y - 8);
    }
}

// 日K线|周K线|月K线|季K线|年K线
void RichKlineCtrl::DrawDayKlines(wxDC* pDC) {
    GetRectMaxPrice(*m_pKlines, m_klineRng.begin, m_klineRng.end);
    GetRectMinPrice(*m_pKlines, m_klineRng.begin, m_klineRng.end);
    // 绘制背景
    DrawKlineBackground(pDC, m_minRectPrice, m_maxRectPrice);

    int visible_klines = m_klineRng.end - m_klineRng.begin + 1;

    double hPrice = m_maxRectPrice - m_minRectPrice;
    int minY = m_pos.y;
    int hRect = GetInnerHeight();
    int wRect = GetInnerWidth();
    double hZoomRatio = hRect / hPrice;

    uiKline kline;
    int nKline = 0;
    for (int i = m_klineRng.begin; i <= m_klineRng.end; i++) {
        kline = m_pKlines->at(i);
        double x1, y1, x2, y2;
        int xShadow, yShadowUp, yShadowDown;  // 上影线,下影线(如果有的话)
        if (m_visibleKlineCount > wRect) {
            if (kline.price_open < kline.price_close) {  // 红盘
                // 绘制K线实体
                x1 = nKline * m_klineWidth;
                y1 = (m_maxRectPrice - kline.price_close) * hZoomRatio + minY;
                x2 = x1;
                y2 = (m_maxRectPrice - kline.price_open) * hZoomRatio + minY;
                pDC->SetPen(*wxRED_PEN);
                pDC->SetBrush(*wxBLACK_BRUSH);
                pDC->DrawLine(x1, y1, x2, y2);
                // 上下影线X坐标
                xShadow = x1;
            } else if (kline.price_open > kline.price_close) {  // 绿盘
                x1 = nKline * m_klineWidth;
                y1 = (m_maxRectPrice - kline.price_open) * hZoomRatio + minY;
                x2 = x1;
                y2 = (m_maxRectPrice - kline.price_close) * hZoomRatio + minY;
                pDC->SetPen(wxPen(wxColor(84, 255, 255)));
                pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
                pDC->DrawLine(x1, y1, x2, y2);
                // 上下影线X坐标
                xShadow = x1;
            } else {  // 平盘或者一字板
                x1 = nKline * m_klineWidth;
                y1 = (m_maxRectPrice - kline.price_open) * hZoomRatio + minY;
                x2 = x1 + m_klineInnerWidth;
                y2 = y1;
                if (is_up_limit_price(kline, m_pShare)) {
                    pDC->SetPen(*wxRED_PEN);
                    pDC->SetBrush(*wxBLACK_BRUSH);
                    pDC->DrawLine(x1, y1, x2, y2);
                } else if (is_down_limit_price(kline, m_pShare)) {
                    pDC->SetPen(wxPen(wxColor(84, 255, 255)));
                    pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
                    pDC->DrawLine(x1, y1, x2, y2);
                } else {
                    pDC->SetPen(wxPen(wxColor(220, 220, 220)));
                    pDC->SetBrush(wxBrush(wxColor(220, 220, 220)));
                    pDC->DrawLine(x1, y1, x2, y2);
                }
                // 上下影线X坐标
                xShadow = (x1 + x2) / 2;
            }
            // 绘制上影线
            yShadowUp = (m_maxRectPrice - kline.price_max) * hZoomRatio + minY;
            pDC->DrawLine(xShadow, yShadowUp, xShadow, y1);
            // 绘制下影线
            yShadowDown = (m_maxRectPrice - kline.price_min) * hZoomRatio + minY;
            pDC->DrawLine(xShadow, yShadowDown, xShadow, y2 - 1);
        } else {
            if (kline.price_open < kline.price_close) {  // 红盘
                // 绘制K线实体
                x1 = static_cast<int>(nKline * m_klineWidth);
                y1 = (m_maxRectPrice - kline.price_close) * hZoomRatio + minY;
                x2 = x1 + m_klineInnerWidth;
                y2 = (m_maxRectPrice - kline.price_open) * hZoomRatio + minY;
                pDC->SetPen(*wxRED_PEN);
                pDC->SetBrush(*wxBLACK_BRUSH);
                pDC->DrawRectangle(x1, y1, x2 - x1, y2 - y1);
                // 上下影线X坐标
                xShadow = (x1 + x2) / 2;
            } else if (kline.price_open > kline.price_close) {  // 绿盘
                x1 = static_cast<int>(nKline * m_klineWidth);
                y1 = (m_maxRectPrice - kline.price_open) * hZoomRatio + minY;
                x2 = x1 + m_klineInnerWidth;
                y2 = (m_maxRectPrice - kline.price_close) * hZoomRatio + minY;
                pDC->SetPen(wxPen(wxColor(84, 255, 255)));
                pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
                pDC->DrawRectangle(x1, y1, x2 - x1, y2 - y1);
                // 上下影线X坐标
                xShadow = (x1 + x2) / 2;
            } else if (kline.price_open == kline.price_close) {
                x1 = static_cast<int>(nKline * m_klineWidth);
                y1 = (m_maxRectPrice - kline.price_open) * hZoomRatio + minY;
                x2 = x1 + m_klineInnerWidth;
                y2 = y1;
                if (is_up_limit_price(kline, m_pShare)) {
                    pDC->SetPen(*wxRED_PEN);
                    pDC->SetBrush(*wxBLACK_BRUSH);
                    pDC->DrawLine(x1, y1, x2, y2);
                } else if (is_down_limit_price(kline, m_pShare)) {
                    pDC->SetPen(wxPen(wxColor(84, 255, 255)));
                    pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
                    pDC->DrawLine(x1, y1, x2, y2);
                } else {
                    pDC->SetPen(wxPen(wxColor(220, 220, 220)));
                    pDC->SetBrush(wxBrush(wxColor(220, 220, 220)));
                    pDC->DrawLine(x1, y1, x2, y2);
                }
                // 上下影线X坐标
                xShadow = (x1 + x2) / 2;
            }
            // 绘制上影线
            yShadowUp = (m_maxRectPrice - kline.price_max) * hZoomRatio + minY;
            pDC->DrawLine(xShadow, yShadowUp, xShadow, y1);
            // 绘制下影线
            yShadowDown = (m_maxRectPrice - kline.price_min) * hZoomRatio + minY;
            pDC->DrawLine(xShadow, yShadowDown, xShadow, y2 - 1);
        }
        nKline++;
    }
    DrawEmaCurves(pDC, visible_klines, m_maxRectPrice, m_minRectPrice, 0, m_pos.y, GetInnerWidth(), GetInnerHeight(),
                  m_klineWidth);
    DrawMinMaxRectPrice(pDC);
    if (m_crossLine != NO_CROSS_LINE) {
        DrawCrossLine(pDC, m_crossLinePt.x, m_crossLinePt.y, m_width, m_height * 0.7);
    }
}

void RichKlineCtrl::DrawMinMaxRectPrice(wxDC* pDC) {
    double hPrice = m_maxRectPrice - m_minRectPrice;
    int hRect = GetInnerHeight();
    double hZoomRatio = hRect / hPrice;
    int minY = m_pos.y;

    double min_x = m_minRectPriceIndex * m_klineWidth + m_klineInnerWidth / 2 + 1;
    double min_y = (m_maxRectPrice - m_minKlinePrice) * hZoomRatio + minY;
    std::string min_text = convert_double(m_minKlinePrice, 2) + "->";
    wxString min_price_text(min_text);

    double max_x = m_maxRectPriceIndex * m_klineWidth + m_klineInnerWidth / 2 + 1;
    double max_y = (m_maxRectPrice - m_maxKlinePrice) * hZoomRatio + minY;
    std::string max_text = "<-" + convert_double(m_maxKlinePrice, 2);
    wxString max_price_text(max_text);

    pDC->SetPen(wxPen(wxColor(220, 220, 220)));
    pDC->SetBrush(wxBrush(wxColor(220, 220, 220)));
    pDC->DrawText(min_price_text, min_x, min_y);
    pDC->DrawText(max_price_text, max_x, max_y);
    // std::cout << "min: " << m_minRectPriceIndex << "," << min_x << "," << min_y << "," << min_price_text <<
    // std::endl; std::cout << "max: " << m_maxRectPriceIndex << "," << max_x << "," << max_y << "," << max_price_text
    // << std::endl;
}

/**
 * @func 绘制十字线
 * @param wxDC* pDC 设备绘制上下文
 * @param int centerX 十字线中心X坐标
 * @param int centerY 十字线中心Y坐标
 * @param int w 控件宽度
 * @param int h 控件高度
 * @author songhuabiao
 */
void RichKlineCtrl::DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w,
                                  int h) {  // 光标十字线
    wxPen dash_pen(wxColor(200, 200, 200), 1, wxPENSTYLE_DOT);
    pDC->SetPen(dash_pen);
    pDC->DrawLine(0, centerY, w, centerY);        // 横线
    pDC->DrawLine(centerX, m_pos.y, centerX, h);  // 竖线
}

/**
 * @brief 绘制EMA曲线
 * @param pDC
 * @param m_visibleKlineCount 可见矩形区域内K线总数目
 * @param m_maxRectPrice 可见矩形区域最高价
 * @param m_minRectPrice 可见矩形区域最低价
 * @param minX 可见矩形区域最小X坐标
 * @param minY 可见矩形区域最小Y坐标
 * @param maxX 可见矩形区域最大X坐标
 * @param maxY 可见矩形区域最大Y坐标
 * @param klineWidth 可见矩形区域K线宽度
 * @param klineSpan 可见矩形区域K线之间间距
 */
void RichKlineCtrl::DrawEmaCurves(wxDC* pDC,
                                  int m_visibleKlineCount,
                                  float m_maxRectPrice,
                                  float m_minRectPrice,
                                  int minX,
                                  int minY,
                                  int maxX,
                                  int maxY,
                                  double klineWidth) {
    if (m_pKlines->size() <= 2) {  // 只有一条K线数据无法绘制EMA曲线
        return;
    }
    double hPrice = m_maxRectPrice - m_minRectPrice;
    int hRect = maxY - minY;
    int wRect = maxX - minX;
    double hZoomRatio = hRect / hPrice;
    for (auto& curve : m_emaCurves) {
        if (curve.visible) {
            pDC->SetPen(wxPen(curve.color));
            wxPointList* pPtList = new wxPointList();
            int nKline = 0;
            double x, y;
            for (int i = m_klineRng.begin; i <= m_klineRng.end; i++) {
                double ema_price = curve.ema_price.at(i);
                // 定义样条曲线的控制点
                x = static_cast<int>(nKline * klineWidth);
                y = static_cast<int>((m_maxRectPrice - ema_price) * hZoomRatio + minY);
                wxPoint* pt = new wxPoint(x, y);
                pPtList->Append(pt);
                nKline += 1;
            }
            // 使用三次样条曲线绘制平滑曲线
            pDC->DrawSpline(pPtList);
            pPtList->DeleteContents(true);
            pPtList->Clear();
            delete pPtList;
            pPtList = nullptr;
        }
    }
}

// 显示周期为n的EMA平滑移动价格曲线
bool RichKlineCtrl::ShowEmaCurve(int n) {
    for (auto& curve : m_emaCurves) {
        if (curve.period == n) {
            curve.visible = true;
            return true;
        }
    }
    return false;
}

// 隐藏周期为n的EMA平滑移动价格曲线
bool RichKlineCtrl::HideEmaCurve(int n) {
    for (auto& curve : m_emaCurves) {
        if (curve.period == n) {
            curve.visible = false;
            return true;
        }
    }
    return false;
}

// 添加周期为n的EMA平滑移动价格曲线
bool RichKlineCtrl::AddEmaCurve(int n, wxColor color, bool visible) {
    if (m_emaCurves.size() >= 8) {  // 一个界面最多显示8条EMA平滑移动价格曲线
        return false;
    }
    ShareEmaCurve curve;
    curve.color = color;
    curve.period = n;
    curve.visible = visible;
    curve.ema_price = {};
    curve.ema_price.reserve(m_pKlines->size());  // 防止std::vector 频繁申请内存
    FormulaEma::GetEmaPrice(*m_pKlines, curve.ema_price, n);
    m_emaCurves.push_back(curve);
    return true;
}

// 删除周期为n的EMA平滑移动价格曲线
bool RichKlineCtrl::DelEmaCurve(int n) {
    auto it = std::remove_if(m_emaCurves.begin(), m_emaCurves.end(), [n](const ShareEmaCurve& curve) {
        return curve.period == n;
    });
    if (it == m_emaCurves.end()) {
        return false;
    }
    m_emaCurves.erase(it, m_emaCurves.end());
    return true;
}

// 检查是否有EMA平滑移动曲线显示
bool RichKlineCtrl::HasEmaCurve() {
    if (m_emaCurves.size() == 0) {
        return false;
    }
    for (auto& curve : m_emaCurves) {
        if (curve.visible) {
            return true;
        }
    }
    return false;
}

void RichKlineCtrl::CalcMinuteKlineAvgPrice(std::vector<minuteKline>& minuteKlines, std::vector<double>& avg_price) {
    int nklines = 0;
    for (const auto& kline : minuteKlines) {
        nklines += 1;
        avg_price.push_back(kline.price / nklines);
    }
}

void RichKlineCtrl::DrawMinuteKlineCurves(wxDC* pDC) {
}

////////////////////// 5日分时图相关函数  //////////////////////
void RichKlineCtrl::DrawFiveDayMinuteKlines(wxDC* pDC) {
}

////////////////////// 分时图相关函数  //////////////////////
void RichKlineCtrl::DrawMinuteKlines(wxDC* pDC) {
    // 绘制曲线
    pDC->SetPen(wxPen(wxColor(255, 255, 0)));
    wxPointList* pPtList = new wxPointList();
    int nKline = 0;
    size_t nKlines = m_pMinuteKlines->size() - 1;
    double x, y;
    double wRect = GetInnerWidth();
    double hRect = GetInnerHeight();
    double m_maxRectPrice = GetRectMaxPrice(*m_pMinuteKlines, 0, nKlines - 1);
    double m_minRectPrice = GetRectMinPrice(*m_pMinuteKlines, 0, nKlines - 1);
    double hPrice = m_maxRectPrice - m_minRectPrice;
    double hZoomRatio = hRect / hPrice;

    // for (size_t i = 0; i < nKlines; i++) {
    //     double ema_price = curve.ema_price.at(i);
    //     // 定义样条曲线的控制点
    //     x = static_cast<int>(nKline * wRect / nKlines);
    //     y = static_cast<int>((m_maxRectPrice - ema_price) * hZoomRatio + 0);
    //     wxPoint* pt = new wxPoint(x, y);
    //     pPtList->Append(pt);
    //     nKline += 1;
    // }
    // 使用三次样条曲线绘制平滑曲线
    pDC->DrawSpline(pPtList);
    pPtList->DeleteContents(true);
    pPtList->Clear();
    delete pPtList;
    pPtList = nullptr;
    // 绘制均线
}

void RichKlineCtrl::DrawMinuteKlineBackground(wxDC* pDC) {
    wxColor clr(255, 0, 0);
    const int nrows = 16;
    const int ncols = 2;
    int row_height = (m_height - (nrows + 1)) / nrows;
    int col_width = (m_width - (ncols + 1)) / ncols;

    wxPen solid_pen(clr, 1, wxPENSTYLE_SOLID);
    pDC->SetPen(solid_pen);
    // 绘制横向实线
    wxPoint pt_start = m_pos;
    wxPoint pt_end = wxPoint(m_pos.x + m_width, m_pos.y);
    for (int i = 0; i < 3; i++) {
        pDC->DrawLine(pt_start, pt_end);
        pt_start.y += (row_height + 1) * nrows / 2;
        pt_end.y += (row_height + 1) * nrows / 2;
    }
    // 绘制竖线实线
    pt_start = m_pos;
    pt_end = wxPoint(m_pos.x, m_pos.y + m_height);
    for (int i = 0; i < 3; i++) {
        pDC->DrawLine(pt_start, pt_end);
        pt_start.x += col_width;
        pt_end.x += col_width;
    }

    // 绘制横向虚线
    wxPen dot_pen(clr, 1, wxPENSTYLE_LONG_DASH);
    pDC->SetPen(dot_pen);
    pt_start = wxPoint(m_pos.x, m_pos.y + 1 + row_height);
    pt_end = wxPoint(m_pos.x + m_width, m_pos.y + 1 + row_height);
    for (int i = 1; i <= nrows; i++) {
        if (i == 8 || i == 16) {
            continue;  // 跳过实线绘制
        }
        pDC->DrawLine(pt_start, pt_end);
        pt_start.y += (row_height + 1) * i;
        pt_end.y += (row_height + 1) * i;
    }
}
