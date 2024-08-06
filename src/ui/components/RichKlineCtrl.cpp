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
    : m_pStorage(pStorage), m_pos(pos), m_width(size.GetWidth()), m_height(size.GetHeight()), m_pKlines(nullptr) {
    Init();
}

void RichKlineCtrl::Init() {
    m_mode = KlineType::Day;
    m_klineWidth = 7;
    m_klineSpan = m_klineWidth * 0.8;
    m_scaleKlineSize = 0;
    m_shareCode = "";
    m_klineRng.begin = 0;
    m_klineRng.end = 0;
    m_curKline = -1;
    m_rectPriceMax = 0;
    m_rectPriceMin = 0;
    m_paddingTop = 10;
    m_paddingBottom = 10;
    m_paddingRight = 10;
    m_crossLine = NO_CROSS_LINE;
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
        m_visibleKlineCount = 120;
    }

    if (kline_type == KlineType::Minute || kline_type == KlineType::FiveDay) {
        m_pStorage->QueryMinuteKlines(share_code, kline_type, &m_pMinuteKlines);
        std::cout << "minute kline size = " << m_pKlines->size() << std::endl;
    } else {
        m_pStorage->QueryKlines(share_code, kline_type, &m_pKlines);
        std::cout << "kline size = " << m_pKlines->size() << std::endl;
    }
    m_emaCurves.clear();
    SetMode(kline_type);

    m_klineRng.begin = (m_pKlines->size() - 1 > 120) ? m_pKlines->size() - 1 - 120 : 0;
    m_klineRng.end = m_pKlines->size() - 1;

    // m_klineRng = GetKlineRangeZoomIn(m_pKlines->size(), m_width, m_klineWidth, m_klineSpan);

    // 添加 EMA 曲线的时候会自动计算相关数据
    // AddEmaCurve(99, wxColor(255, 0, 255));
    // AddEmaCurve(255, wxColor(255, 255, 0));
    // AddEmaCurve(905, wxColor(0, 255, 0));
    return true;
}

// 股票代码保持不变，切换K线类型
bool RichKlineCtrl::LoadKlines(const KlineType& kline_type) {
    return LoadKlines(m_shareCode, kline_type);
}

std::string RichKlineCtrl::GetShareCode() const {
    return m_shareCode;
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

int RichKlineCtrl::GetInnerWidth() {
    return m_width - m_paddingRight;
}

int RichKlineCtrl::GetInnerHeight() {
    return m_height * 0.7 - m_paddingTop - m_paddingBottom;
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
    pDC->DrawLine(0, centerY, w, centerY);  // 横线
    pDC->DrawLine(centerX, 0, centerX, h);  // 竖线
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
        }
    }
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
    m_rectPriceMin = min;
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
        }
    }
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
    m_rectPriceMax = max;
    return max;
}

void RichKlineCtrl::CalcLineWidth() {
    double w = m_width / m_klineCount;
    if (m_klineCount * 2 < m_width) {  // 需要显示的K线数量 < 显示容器宽度的一半
        int m_klineWidth = w * 0.75;
        int m_klineSpan = w * 0.25;
        if (m_klineWidth > 1 && m_klineWidth % 2 == 0) {
            m_klineWidth -= 1;
        }
        m_klineCount = m_width / (m_klineWidth + m_klineWidth);
    } else {
        int m_klineWidth = w;
        int m_klineSpan = 0;
    }
}
/**
 *@todo 获取K线的范围
 *@param long totalKLines //总共的K线数
 *@param long rect 容器的宽度
 *@param int16_t klineWidth 日K线的宽度
 *@param int16_t klineSpan 日K线的间距
 *@param long crossLine 十字线所在位置
 *@return KlineRange
 *@author songhuabiao
 */
uiKlineRange RichKlineCtrl::GetKlineRangeZoomIn(long totalKlines,
                                                long rect,
                                                int16_t klineWidth,
                                                int16_t klineSpan,
                                                long crossLine) {
    uiKlineRange rng;
    long count = rect / (klineWidth + klineSpan);
    if (count > totalKlines) {
        count = totalKlines - 1;
    }
    if (crossLine != NO_CROSS_LINE) {
        long left = crossLine - m_klineRng.begin;
        long right = m_klineRng.end - crossLine;
        long removed = abs(left + right - count);
        if (count > left + right) {  // more klines to be shown
            rng.begin = m_klineRng.begin - removed * left / (left + right);
            rng.end = m_klineRng.end + removed * right / (left + right);
        } else {  // less klines to be shown
            rng.begin = m_klineRng.begin + removed * left / (left + right);
            rng.end = m_klineRng.end - removed * right / (left + right);
        }
    } else {
        rng.begin = totalKlines - 1 - count;
        rng.end = totalKlines - 1;
    }

    return rng;
}

/**
 * @todo 根据 十字线 位置，计算缩小查看的日K线可视范围
 * @param int
 */
uiKlineRange RichKlineCtrl::GetKlineRangeZoomOut(long totalKLines, long crossLine) {
    uiKlineRange rng;
    if (crossLine != NO_CROSS_LINE) {
        long left = crossLine - m_klineRng.begin;
        long right = m_klineRng.end - crossLine;
        rng.begin = m_klineRng.begin - 360 * left / (left + right);
        rng.end = m_klineRng.end + 360 * right / (left + right);
    } else {
        rng.begin = m_klineRng.begin - 360;
        rng.end = m_klineRng.end;
    }
    if (rng.begin <= 0) {
        rng.begin = 0;
    }
    if (rng.end >= totalKLines - 1) {
        rng.end = totalKLines - 1;
    }
    return rng;
}

/**
 * @func 获取第N条日K线的十字线位置
 * @param long n 从左往右数第N条K线
 * @param return wxPoint
 * @author songhuabiao
 */
wxPoint RichKlineCtrl::GetCrossLinePt(long n) {
    double x, y;
    long total = m_klineRng.end - m_klineRng.begin;
    uiKline item = m_pKlines->at(n);
    double hPrice = m_rectPriceMax - m_rectPriceMin;
    y = m_paddingTop + (1 - (item.price_close - m_rectPriceMin) / hPrice) * GetInnerHeight();
    if (total > m_width) {  // 一屏幕已经显示不下了
        x = (n - m_klineRng.begin) / total * m_width;
    } else {
        x = (m_klineWidth + m_klineSpan) * (n - m_klineRng.begin) + m_klineWidth / 2;
    }
    return wxPoint(x, y);
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
        DrawDayKlines(pDC);
    }
}

// 日K线|周K线|月K线|季K线|年K线
void RichKlineCtrl::DrawDayKlines(wxDC* pDC) {
    float rect_price_max = GetRectMaxPrice(*m_pKlines, m_klineRng.begin, m_klineRng.end);
    float rect_price_min = GetRectMinPrice(*m_pKlines, m_klineRng.begin, m_klineRng.end);
    int visible_klines = m_klineRng.end - m_klineRng.begin + 1;

    double hPrice = rect_price_max - rect_price_min;
    int minY = m_paddingTop;
    int hRect = GetInnerHeight() - m_paddingTop;
    int wRect = GetInnerWidth() - 0;
    double hScale = hRect / hPrice;
    double wScale = static_cast<double>(wRect) / m_visibleKlineCount;

    m_klineWidth = 1;
    m_klineSpan = 1;
    if (m_visibleKlineCount < wRect) {
        double avg_width = static_cast<double>(wRect) / m_visibleKlineCount;
        m_klineWidth = static_cast<int>(avg_width * 0.8);
        if (m_klineWidth < 3) {
            m_klineWidth = 3;
        }
        if (m_klineWidth % 2 == 0) {
            m_klineWidth -= 1;
        }
        m_klineSpan = static_cast<int>(avg_width * 0.2);
        if (m_klineSpan + m_klineWidth > static_cast<int>(avg_width)) {
            m_klineSpan -= 1;
        }
    }

    uiKline kline;
    int nKline = 0;
    for (int i = m_klineRng.begin; i <= m_klineRng.end; i++) {
        kline = m_pKlines->at(i);
        double x1, y1, x2, y2;
        double xShadow, yShadowUp, yShadowDown;  // 上影线,下影线(如果有的话)
        if (m_visibleKlineCount > wRect) {
            if (kline.price_open < kline.price_close) {  // 红盘
                // 绘制K线实体
                x1 = nKline * wScale;  // nKline/m_visibleKlineCount == x1/wRect;
                y1 = (rect_price_max - kline.price_close) * hScale + minY;
                x2 = x1;
                y2 = (rect_price_max - kline.price_open) * hScale + minY;
                pDC->SetPen(*wxRED_PEN);
                pDC->SetBrush(*wxBLACK_BRUSH);
                pDC->DrawLine(x1, y1, x2, y2);
                // 绘制上影线
                xShadow = x1;
                yShadowUp = (rect_price_max - kline.price_max) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowUp, xShadow, y1);
                // 绘制下影线
                yShadowDown = (rect_price_max - kline.price_min) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowDown, xShadow, y2 - 1);
            } else if (kline.price_open > kline.price_close) {  // 绿盘
                x1 = nKline * wScale;
                y1 = (rect_price_max - kline.price_open) * hScale + minY;
                x2 = x1;
                y2 = (rect_price_max - kline.price_close) * hScale + minY;
                pDC->SetPen(wxPen(wxColor(84, 255, 255)));
                pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
                pDC->DrawLine(x1, y1, x2, y2);
                // 绘制上影线
                xShadow = x1;
                yShadowUp = (rect_price_max - kline.price_max) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowUp, xShadow, y1);
                // 绘制下影线
                yShadowDown = (rect_price_max - kline.price_min) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowDown, xShadow, y2 - 1);
            }
        } else {
            if (kline.price_open < kline.price_close) {  // 红盘
                // 绘制K线实体
                x1 = nKline * (m_klineWidth + m_klineSpan);
                y1 = (rect_price_max - kline.price_close) * hScale + minY;
                x2 = x1 + m_klineWidth;
                y2 = (rect_price_max - kline.price_open) * hScale + minY;
                pDC->SetPen(*wxRED_PEN);
                pDC->SetBrush(*wxBLACK_BRUSH);
                pDC->DrawRectangle(x1, y1, x2 - x1, y2 - y1);
                // 绘制上影线
                xShadow = (x1 + x2) / 2;
                yShadowUp = (rect_price_max - kline.price_max) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowUp, xShadow, y1);
                // 绘制下影线
                yShadowDown = (rect_price_max - kline.price_min) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowDown, xShadow, y2 - 1);
            } else if (kline.price_open > kline.price_close) {  // 绿盘
                x1 = nKline * (m_klineWidth + m_klineSpan);
                y1 = (rect_price_max - kline.price_open) * hScale + minY;
                x2 = x1 + m_klineWidth;
                y2 = (rect_price_max - kline.price_close) * hScale + minY;
                pDC->SetPen(wxPen(wxColor(84, 255, 255)));
                pDC->SetBrush(wxBrush(wxColor(84, 255, 255)));
                pDC->DrawRectangle(x1, y1, x2 - x1, y2 - y1);
                // 绘制上影线
                xShadow = (x1 + x2) / 2;
                yShadowUp = (rect_price_max - kline.price_max) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowUp, xShadow, y1);
                // 绘制下影线
                yShadowDown = (rect_price_max - kline.price_min) * hScale + minY;
                pDC->DrawLine(xShadow, yShadowDown, xShadow, y2 - 1);
            }
        }
        // 考虑跌停
        // 考虑涨停
        nKline++;
    }

    // DrawEmaCurves(pDC, visible_klines, rect_price_max, rect_price_min, 0, m_paddingTop, GetInnerWidth(),
    //               GetInnerHeight(), m_klineWidth, m_klineSpan);
    if (m_crossLine != NO_CROSS_LINE) {
        DrawCrossLine(pDC, m_crossLinePt.x, m_crossLinePt.y, m_width, m_height * 0.7);
    }
}

void RichKlineCtrl::DrawMinuteKlineCurves(wxDC* pDC) {
}

////////////////////// 5日分时图相关函数  //////////////////////
void RichKlineCtrl::DrawFiveDayMinuteKlines(wxDC* pDC) {
}

/**
 * @brief 绘制EMA曲线
 * @param pDC
 * @param m_visibleKlineCount 可见矩形区域内K线总数目
 * @param rect_price_max 可见矩形区域最高价
 * @param rect_price_min 可见矩形区域最低价
 * @param minX 可见矩形区域最小X坐标
 * @param minY 可见矩形区域最小Y坐标
 * @param maxX 可见矩形区域最大X坐标
 * @param maxY 可见矩形区域最大Y坐标
 * @param klineWidth 可见矩形区域K线宽度
 * @param klineSpan 可见矩形区域K线之间间距
 */
void RichKlineCtrl::DrawEmaCurves(wxDC* pDC,
                                  int m_visibleKlineCount,
                                  float rect_price_max,
                                  float rect_price_min,
                                  int minX,
                                  int minY,
                                  int maxX,
                                  int maxY,
                                  int klineWidth,
                                  int klineSpan) {
    double hPrice = rect_price_max - rect_price_min;
    int hRect = maxY - minY;
    int wRect = maxX - minX;
    double hScale = hRect / hPrice;
    for (auto& curve : m_emaCurves) {
        if (curve.visible) {
            pDC->SetPen(wxPen(curve.color));
            wxPointList* pPtList = new wxPointList();
            int nKline = 0;
            double x, y;
            for (int i = m_klineRng.begin; i <= m_klineRng.end; i++) {
                double ema_price = curve.ema_price.at(i);
                // 定义样条曲线的控制点
                x = static_cast<int>(nKline * wRect / m_visibleKlineCount);
                y = static_cast<int>((rect_price_max - ema_price) * hScale + minY);
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

void RichKlineCtrl::OnBackground(wxEraseEvent& event) {
    wxDC* pDC = event.GetDC();
    pDC->SetBrush(wxBrush(wxColour(0, 0, 0)));
}

void RichKlineCtrl::OnSize(wxSizeEvent& event) {
    wxSize size = event.GetSize();
    m_width = size.GetWidth();
    m_height = size.GetHeight();
    m_klineRng = GetKlineRangeZoomIn(m_pKlines->size(), m_width, m_klineWidth, m_klineSpan);
}

void RichKlineCtrl::OnKeyDown(wxKeyEvent& event) {
    int max = m_pKlines->size();
    int key = event.GetKeyCode();
    if (key == WXK_LEFT) {            // look left
        if (m_klineRng.begin == 0) {  // no more klines in the left
            return;
        }
        if (m_crossLine == m_klineRng.begin) {
            m_klineRng.begin -= 1;
            m_klineRng.end -= 1;
        } else if (m_crossLine >= m_klineRng.begin && m_crossLine <= m_klineRng.end) {
            if (m_crossLine != 0) {
                m_crossLine -= 1;
                m_crossLinePt = GetCrossLinePt(m_crossLine);
            }
        }
    } else if (key == WXK_RIGHT) {  // look right
        if (m_crossLine == m_klineRng.end && m_klineRng.end != max) {
            m_klineRng.begin += 1;
            m_klineRng.end += 1;
        } else if (m_crossLine >= m_klineRng.begin && m_crossLine <= m_klineRng.end) {
            if (m_crossLine != max - 1) {
                m_crossLine += 1;
                m_crossLinePt = GetCrossLinePt(m_crossLine);
            }
        }
    } else if (key == WXK_UP) {  // scale up klines
        if (m_crossLine != NO_CROSS_LINE && m_crossLine <= m_klineRng.begin &&
            m_crossLine >= m_klineRng.end) {  // 以十字线为中心放大
            m_visibleKlineCount = m_visibleKlineCount / 2;
            if (m_visibleKlineCount < 10) {
                m_visibleKlineCount = 10;
            }
            m_klineRng.begin = m_crossLine - m_visibleKlineCount / 2;
            m_klineRng.end = m_crossLine + m_visibleKlineCount / 2;
            if (m_klineRng.end > m_pKlines->size() - 10) {
                m_klineRng.end = m_pKlines->size() - 10;
                m_klineRng.begin = m_klineRng.end - m_visibleKlineCount;
            }
            if (m_klineRng.begin < 0) {
                m_klineRng.begin = 0;
                m_klineRng.end = m_klineRng.begin + m_visibleKlineCount;
            }
        } else {
            m_visibleKlineCount = m_visibleKlineCount / 2;
            if (m_visibleKlineCount < 10) {
                m_visibleKlineCount = 10;
            }
            m_klineRng.begin = m_pKlines->size() - 1 - m_visibleKlineCount;
        }
    } else if (key == WXK_DOWN) {  // scale down klines
        if (m_crossLine != NO_CROSS_LINE && m_crossLine <= m_klineRng.begin &&
            m_crossLine >= m_klineRng.end) {  // 以十字线为中心缩小
            if (m_visibleKlineCount != m_pKlines->size() - 1) {
                m_visibleKlineCount = m_visibleKlineCount * 2;
                if (m_visibleKlineCount > m_pKlines->size() - 1) {
                    m_visibleKlineCount = m_pKlines->size() - 1;
                }
                m_klineRng.end = m_crossLine + m_visibleKlineCount / 2;
                if (m_klineRng.end > m_pKlines->size() - 10) {
                    m_klineRng.end = m_pKlines->size() - 10;
                }
                m_klineRng.begin = m_crossLine - m_visibleKlineCount / 2;
                if (m_klineRng.begin < 0) {
                    m_klineRng.begin = 0;
                }
            }
        } else {
            m_visibleKlineCount = m_visibleKlineCount * 2;
            if (m_visibleKlineCount > m_pKlines->size() - 1) {
                m_visibleKlineCount = m_pKlines->size() - 1;
            }
            m_klineRng.end = m_pKlines->size() - 1;
            m_klineRng.begin = m_klineRng.end - m_visibleKlineCount;
        }
    } else if (key == WXK_HOME) {
        m_crossLine = m_klineRng.begin;
    } else if (key == WXK_END) {
        m_crossLine = m_klineRng.end;
    } else if (key == WXK_ESCAPE) {
        m_crossLine = NO_CROSS_LINE;
    }

    if (m_crossLine != NO_CROSS_LINE) {
        if (m_crossLine >= m_klineRng.begin && m_crossLine <= m_klineRng.end) {
            m_crossLinePt = GetCrossLinePt(m_crossLine);  // 修正十字线的位置
        }
    }
}

void RichKlineCtrl::OnLeftMouseDown(wxMouseEvent& event) {
    if (event.LeftIsDown()) {  // 左键按下
        long x, y;
        event.GetPosition(&x, &y);
        if (this->m_klineWidth == 1) {
            m_crossLinePt.x = x;
            m_crossLinePt.y = y;
            m_crossLine = m_klineRng.begin + x * (m_klineRng.end - m_klineRng.begin) / m_width;
        } else {  // 获取最靠近的K线
            int k = x / (m_klineWidth + m_klineSpan);
            m_crossLine = m_klineRng.begin + k;
            m_crossLinePt = GetCrossLinePt(m_crossLine);
        }
    }
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

float RichKlineCtrl::GetRectMinPrice(std::vector<minuteKline>& minuteKlines, int begin, int end) {
    float min = 100000000;
    for (int i = begin; i <= end; i++) {
        if (minuteKlines.at(i).price < min) {
            min = minuteKlines.at(i).price;
        }
    }
    return min;
}
float RichKlineCtrl::GetRectMaxPrice(std::vector<minuteKline>& minuteKlines, int begin, int end) {
    float max = -100000000;
    for (int i = begin; i <= end; i++) {
        if (minuteKlines.at(i).price > max) {
            max = minuteKlines.at(i).price;
        }
    }
    return max;
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
    double rect_price_max = GetRectMaxPrice(*m_pMinuteKlines, 0, nKlines - 1);
    double rect_price_min = GetRectMinPrice(*m_pMinuteKlines, 0, nKlines - 1);
    double hPrice = rect_price_max - rect_price_min;
    double hScale = hRect / hPrice;

    // for (size_t i = 0; i < nKlines; i++) {
    //     double ema_price = curve.ema_price.at(i);
    //     // 定义样条曲线的控制点
    //     x = static_cast<int>(nKline * wRect / nKlines);
    //     y = static_cast<int>((rect_price_max - ema_price) * hScale + 0);
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

void RichKlineCtrl::CalcMinuteKlineAvgPrice(std::vector<minuteKline>& minuteKlines, std::vector<double>& avg_price) {
    double price = 0.0;
    int nklines = 0;
    for (const auto& kline : minuteKlines) {
        nklines += 1;
        avg_price.push_back(kline.price / nklines);
    }
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
