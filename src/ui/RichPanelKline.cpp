///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/RichPanelKline.cpp
// Purpose:     minute/five day minute/day/week/month/quarter/year kline panel
// Author:      songhuabiao
// Modified by:
// Created:     2024-06-14 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichPanelKline.h"
#include <wx/dcbuffer.h>
#include <wx/display.h>
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"
#include "ui/RichMainFrame.h"
#include "ui/indicators/RichAmountIndicatorCtrl.h"
#include "ui/indicators/RichMinuteVolumeIndicatorCtrl.h"
#include "ui/indicators/RichTurnoverRateIndicatorCtrl.h"
#include "ui/indicators/RichVolumeIndicatorCtrl.h"
#include "util/Color.h"
#include "util/Macro.h"

const long RichPanelKline::ID_SHARE_NAME_CTRL = wxNewId();
const long RichPanelKline::ID_ADD_FAVORITE_CTRL = wxNewId();
const long RichPanelKline::ID_KLINE_CTRL = wxNewId();
const long RichPanelKline::ID_DIALOG_KLINE_INFO = wxNewId();
const long RichPanelKline::ID_RADIO_CTRL = wxNewId();
const long RichPanelKline::ID_SECOND_RADIO_CTRL = wxNewId();

// 以下函数实现必须写，否则会爆错误 undefined reference to 'vtable for RichPanelKline'
BEGIN_EVENT_TABLE(RichPanelKline, RichPanel)
EVT_PAINT(RichPanelKline::OnPaint)
EVT_SIZE(RichPanelKline::OnSize)
EVT_LEFT_DOWN(RichPanelKline::OnLeftMouseDown)
// EVT_KEY_DOWN(RichPanelKline::OnKeyDown)
EVT_ERASE_BACKGROUND(RichPanelKline::OnBackground)
EVT_RICH_RADIO(RichPanelKline::ID_RADIO_CTRL, RichPanelKline::OnKlineChanged)
EVT_RICH_RADIO(RichPanelKline::ID_SECOND_RADIO_CTRL, RichPanelKline::OnVolumeBarChanged)
EVT_MOUSEWHEEL(RichPanelKline::OnMouseWheel)
END_EVENT_TABLE()

RichPanelKline::RichPanelKline(PanelType type,
                               StockDataStorage* pStorage,
                               wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size)
    : RichPanel(type, pStorage, parent, id, pos, size, wxTAB_TRAVERSAL | wxWANTS_CHARS, _T("Panel_Kline")) {
    // 股票名称
    m_pShareNameCtrl =
        new wxStaticText(this, ID_SHARE_NAME_CTRL, wxT("---"), wxPoint(KLINE_PANEL_PADDING, KLINE_PANEL_PADDING),
                         wxSize(64, KLINE_PANEL_TOP_CTRL_HEIGHT));
    m_pShareNameCtrl->SetBackgroundColour(KLINE_PANEL_BACKGROUND_COLOR);
    m_pShareNameCtrl->SetForegroundColour(wxColor(200, 200, 200));

    // 日/周/月/季/年 K线
    std::vector<std::string> options = {"分时", "五日", "日线", "周线", "月线", "季线", "年线"};
    m_pRadioCtrl =
        new RichRadioCtrl(options, 2, this, ID_RADIO_CTRL, wxPoint(KLINE_PANEL_PADDING + 64, KLINE_PANEL_PADDING),
                          wxSize(400, KLINE_PANEL_TOP_CTRL_HEIGHT));
    // 事件绑定， Radio 控件将始终获取鼠标键盘焦点
    m_pRadioCtrl->Bind(wxEVT_LEFT_UP, &RichPanelKline::OnLeftMouseDown, this);
    // K线主图
    m_ptKlineCtrl = wxPoint(KLINE_PANEL_PADDING, KLINE_PANEL_TOP_CTRL_HEIGHT * 2 + KLINE_PANEL_PADDING * 3);
    m_sizeKlineCtrl = size;
    m_sizeKlineCtrl.DecBy(wxSize(0, KLINE_PANEL_TOP_CTRL_HEIGHT * 2 +
                                        KLINE_PANEL_PADDING * 3));  // 这里不能使用DecTo,会导致RichPanelKline控件宽度为0

    // 自选按钮
    m_pAddFavoriteCtrl = new wxStaticText(this, ID_SHARE_NAME_CTRL, wxT("+")),
    m_pAddFavoriteCtrl->SetFont(RichHelper::GetBoldFont());
    m_pAddFavoriteCtrl->SetForegroundColour(wxColor(200, 200, 200));

    /**
     * @todo 对齐第一行控件
     * wxBoxSizer can lay out its children either vertically or horizontally,
     * depending on what flag is being used in its constructor.
     * When using a vertical sizer, each child can be centered, aligned to the right or aligned to the left.
     * Correspondingly, when using a horizontal sizer, each child can be centered, aligned at the bottom or aligned at
     * the top. The stretch factor described in the last paragraph is used for the main orientation, i.e. when using a
     * horizontal box sizer, the stretch factor determines how much the child can be stretched horizontally. The
     * following sample shows the same dialog as in the last sample, only the box sizer is a vertical box sizer now:
     */
    wxBoxSizer* klineSizer = new wxBoxSizer(wxHORIZONTAL);
    klineSizer->AddSpacer(KLINE_PANEL_PADDING);
    klineSizer->Add(m_pShareNameCtrl, 0, wxLEFT | wxRIGHT, KLINE_PANEL_PADDING);
    klineSizer->Add(m_pRadioCtrl, 0, wxLEFT);
    klineSizer->AddStretchSpacer(1);  // 增加中间的空白区域
    klineSizer->Add(m_pAddFavoriteCtrl, 0, wxRIGHT, KLINE_PANEL_PADDING * 2);
    SetSizer(klineSizer);

    // 日K线主图
    m_pKlineCtrl = new RichKlineCtrl(pStorage, m_ptKlineCtrl, m_sizeKlineCtrl);
    int crossLineHeight = GetSize().GetHeight() - KLINE_PANEL_TOP_CTRL_HEIGHT * 2;
    m_pKlineCtrl->SetCrossLineHeight(crossLineHeight);

    // 成交量附图
    RichIndicatorCtrl* pVolumeIndicator = new RichVolumeIndicatorCtrl(m_pKlineCtrl);
    wxASSERT(pVolumeIndicator != nullptr);
    IndicatorInsert(pVolumeIndicator);
    // 成交额附图
    RichIndicatorCtrl* pAmountIndicator = new RichAmountIndicatorCtrl(m_pKlineCtrl);
    wxASSERT(pAmountIndicator != nullptr);
    IndicatorInsert(pAmountIndicator);
    // 换手率附图
    RichIndicatorCtrl* pTurnoverRateIndicator = new RichTurnoverRateIndicatorCtrl(m_pKlineCtrl);
    wxASSERT(pTurnoverRateIndicator != nullptr);
    IndicatorInsert(pTurnoverRateIndicator);
    // 分时图附图
    m_pMinuteIndicator = new RichMinuteVolumeIndicatorCtrl(m_pKlineCtrl);
    m_pMinuteIndicator->Hide();
    wxASSERT(m_pMinuteIndicator != nullptr);

    m_inMinuteMode = false;
    m_oldInMinuteMode = false;
    // 重新计算主图+附图高度
    IndicatorReLayout();

    // 日K线信息
    m_ptKlineInfoCtrl = pos;
    m_ptKlineInfoCtrl.y = pos.y + 46 + KLINE_PANEL_TOP_CTRL_HEIGHT * 2;
    m_pDialogKlineInfo = new RichDialogKlineInfo(this, ID_DIALOG_KLINE_INFO, m_ptKlineInfoCtrl, wxSize(150, 240));
    m_pDialogKlineInfo->Show(false);

    // 使用自动双缓冲
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_KEY_DOWN, &RichPanelKline::OnKeyDown, this);
    // SetFocusIgnoringChildren();  // 忽略子窗口默认获取输入焦点，解决无法移动K线的问题
}

RichPanelKline::~RichPanelKline() {
    if (m_pKlineCtrl != nullptr) {
        delete m_pKlineCtrl;
        m_pKlineCtrl = nullptr;
    }
    if (m_pDialogKlineInfo != nullptr) {
        delete m_pDialogKlineInfo;
        m_pDialogKlineInfo = nullptr;
    }
    // 释放 日/周/月/年 附图指标资源
    for (auto& indicator : m_indicators) {
        delete indicator;
    }
    m_indicators.clear();
    // 释放 分时图/5日分时图 附图指标资源
    if (m_pMinuteIndicator != nullptr) {
        delete m_pMinuteIndicator;
        m_pMinuteIndicator = nullptr;
    }
}

void RichPanelKline::SetShareCode(const std::string& share_code) {
    bool result = m_pKlineCtrl->LoadKlines(share_code);
    if (result) {  // 数据有可能加载失败，弹窗提示用户
        m_pShare = m_pKlineCtrl->GetCurrentShare();
        if (m_pShare != nullptr) {  // 更新股票名称
            m_pShareNameCtrl->SetLabel(CN(m_pShare->name));
        }
    }
    this->Refresh();
}

void RichPanelKline::OnPaint(wxPaintEvent& event) {
    // 自动双缓冲，如果系统默认支持双缓冲，会调用系统，否则使用位图实现，
    // 必须在 Create之前调用 SetBackgroundStyle(wxBG_STYLE_PAINT);
    // 绘制黑色背景
    wxAutoBufferedPaintDC dc(this);
    m_pKlineCtrl->OnPaint(&dc);
    for (auto& indicator : m_indicators) {
        indicator->Draw(&dc);
    }
    if (!m_inMinuteMode) {
        m_pKlineCtrl->DrawCrossLine(&dc);
    }
    m_pMinuteIndicator->Draw(&dc);
}

void RichPanelKline::OnBackground(wxEraseEvent& event) {
    wxSize size = GetClientSize();
    wxRect rect(size);
    wxDC* pDC = event.GetDC();
    pDC->SetBrush(wxBrush(KLINE_PANEL_BACKGROUND_COLOR));
    pDC->DrawRectangle(rect);
}

void RichPanelKline::OnSize(wxSizeEvent& event) {
    IndicatorReLayout();  // 重新布局
    Refresh();            // 界面需要重绘
    event.Skip();         // 必须调用，否则wxSizeEvent只触发一次
}

void RichPanelKline::OnKeyDown(wxKeyEvent& event) {
    m_pKlineCtrl->OnKeyDown(event);
    wxSize size = GetSize();
    size.DecBy(0, 30);
    wxRect rect;
    rect.SetPosition(m_ptKlineCtrl);
    rect.SetSize(size);
    RefreshRect(rect);  // 界面需要重绘
    ShowDialogKlineInfoIfNeeded();
    int key = event.GetKeyCode();
    if (key == WXK_LEFT || key == WXK_RIGHT || key == WXK_UP || key == WXK_DOWN) {
        this->SetFocus();
        event.Skip(true);
    } else {
        // 一定要最后调用
        event.Skip(false);
    }
}

void RichPanelKline::OnMouseWheel(wxMouseEvent& event) {
    int delta = event.GetWheelRotation();
    if (delta > 0) {
        bool result = m_pKlineCtrl->LoadPrevKlines();
        if (result) {  // 数据有可能加载失败，弹窗提示用户
            m_pShare = m_pKlineCtrl->GetCurrentShare();
            if (m_pShare != nullptr) {  // 更新股票名称
                m_pShareNameCtrl->SetLabel(CN(m_pShare->name));
            }
            if (m_pDialogKlineInfo->IsShown()) {  // 更新显示的K线详情
                ShowDialogKlineInfoIfNeeded();
            }
        }
        this->Refresh();
    } else {
        bool result = m_pKlineCtrl->LoadNextKlines();
        if (result) {  // 数据有可能加载失败，弹窗提示用户
            m_pShare = m_pKlineCtrl->GetCurrentShare();
            if (m_pShare != nullptr) {  // 更新股票名称
                m_pShareNameCtrl->SetLabel(CN(m_pShare->name));
            }
            if (m_pDialogKlineInfo->IsShown()) {  // 更新显示的K线详情
                ShowDialogKlineInfoIfNeeded();
            }
        }
        this->Refresh();
    }
}

void RichPanelKline::ShowDialogKlineInfoIfNeeded() {
    uiKline* pCurrentKline = m_pKlineCtrl->GetCurrentKline();
    uiKline* pPreviousKline = m_pKlineCtrl->GetPreviousKline();
    Share* pShare = m_pKlineCtrl->GetCurrentShare();
    m_pDialogKlineInfo->SetCurrentKline(pCurrentKline);
    m_pDialogKlineInfo->SetPreviousKline(pPreviousKline);
    if (!m_pDialogKlineInfo->IsShown()) {
        m_pDialogKlineInfo->Move(m_ptKlineInfoCtrl);
    }
    m_pDialogKlineInfo->Show(pCurrentKline == nullptr ? false : true);
    m_pDialogKlineInfo->SetTitle(CN(pShare->name + "(" + pShare->code + ")"));
    m_pDialogKlineInfo->Refresh();
}

void RichPanelKline::OnLeftMouseDown(wxMouseEvent& event) {
    m_pKlineCtrl->OnLeftMouseDown(event);
    Refresh();
    ShowDialogKlineInfoIfNeeded();
    this->SetFocus();
    event.Skip();
}

void RichPanelKline::OnKlineChanged(RichRadioEvent& event) {
    int selection = event.GetSelection();
    // 分时图/5日分时图需要隐藏 VolumeBarCtrl
    if (selection == 0 || selection == 1) {
        HideIndicators();
        ShowMinuteIndicator();
        m_inMinuteMode = true;
    } else {
        ShowIndicators();
        HideMinuteIndicator();
        m_inMinuteMode = false;
    }
    bool result = m_pKlineCtrl->LoadKlines(static_cast<KlineType>(selection));
    if (!result) {  // 数据有可能加载失败，弹窗提示用户
        return;
    }
    m_pShare = m_pStorage->FindShare(m_pKlineCtrl->GetShareCode());
    if (m_pShare != nullptr) {  // 更新股票名称
        m_pShareNameCtrl->SetLabel(CN(m_pShare->name));
    }
    if (m_oldInMinuteMode != m_inMinuteMode) {
        IndicatorReLayout();
        m_oldInMinuteMode = m_inMinuteMode;
    }
    this->Refresh();
}

void RichPanelKline::OnVolumeBarChanged(RichRadioEvent& event) {
    int mode = event.GetSelection();
    for (auto& indicator : m_indicators) {
        indicator->SetMode(mode);
    }
    this->Refresh();
}

/////////////////////////////////// 附图指标相关操作 ///////////////////////////////////
// 附图指标上移
void RichPanelKline::IndicatorMoveUp(int i) {
    if (i <= 0 || i >= m_indicators.size()) {
        return;
    }
    std::swap(m_indicators[i], m_indicators[i - 1]);
}

// 附图指标下移
void RichPanelKline::IndicatorMoveDown(int i) {
    if (i >= m_indicators.size() - 1 || i < 0) {
        return;
    }
    std::swap(m_indicators[i], m_indicators[i + 1]);
}

// 增加一个附图指标到末尾
void RichPanelKline::IndicatorInsert(RichIndicatorCtrl* pIndicator) {
    m_indicators.emplace_back(pIndicator);
}

// 删除附图指标
void RichPanelKline::IndicatorDelete(int i) {
    if (i < 0 || i > m_indicators.size() - 1) {
        return;
    }
    if (m_indicators.size() == 0) {
        return;
    }
    m_indicators.erase(m_indicators.begin() + i);
}

// 替换指定位置的附图指标
void RichPanelKline::IndicatorReplace(int i, RichIndicatorCtrl* pIndicator) {
    if (i < 0 || i > m_indicators.size() - 1) {
        return;
    }
    if (m_indicators.size() == 0) {
        return;
    }
    m_indicators.erase(m_indicators.begin() + i);
    m_indicators.insert(m_indicators.begin() + i, pIndicator);
}

// 重新布局附图指标
void RichPanelKline::IndicatorReLayout() {
    // 计算有几个附图指标
    size_t n = m_indicators.size();
    // 每个附图指标的高度可以自适应，用户也可以手动调整高度
    int width = GetSize().GetWidth();
    int height = GetSize().GetHeight();
    wxDisplay display;
    wxRect rect = display.GetClientArea();
    // 全屏模式下的面板高度
    int fullscreen_height = rect.GetHeight() - ((RichApplication*)wxTheApp)->GetMainFrame()->GetTitleBarHeight();
    // 等比例计算窗口缩放后的附图指标高度
    int indicator_height = INDICATOR_CTRL_HEIGHT * height / fullscreen_height;
    int top_height = KLINE_PANEL_TOP_CTRL_HEIGHT * 2 + KLINE_PANEL_PADDING * 3;
    if (m_inMinuteMode) {
        int main_height = height - indicator_height * 2 - top_height - 30;
        m_pKlineCtrl->SetWidth(width);
        m_pKlineCtrl->SetHeight(main_height);
        m_pMinuteIndicator->SetPosition(KLINE_PANEL_PADDING, top_height + main_height + 30);
        m_pMinuteIndicator->SetWidth(width);
        m_pMinuteIndicator->SetManualHeight(indicator_height * 2 - 20);
    } else {
        if (n < 4) {  // 如果附图指标小于4个，附图高度固定+K线主图高度减少
            int main_height = height - n * indicator_height - top_height;
            m_pKlineCtrl->SetWidth(width);
            m_pKlineCtrl->SetHeight(main_height);
            for (size_t i = 0; i < m_indicators.size(); i++) {
                auto indicator = m_indicators[i];
                indicator->SetPosition(KLINE_PANEL_PADDING, top_height + main_height + i * indicator_height);
                indicator->SetWidth(width);
                indicator->SetManualHeight(indicator_height);
            }
        }
    }
}

void RichPanelKline::ShowIndicators() {
    for (size_t i = 0; i < m_indicators.size(); i++) {
        m_indicators[i]->Show();
    }
}

void RichPanelKline::HideIndicators() {
    for (size_t i = 0; i < m_indicators.size(); i++) {
        m_indicators[i]->Hide();
    }
}

void RichPanelKline::ShowMinuteIndicator() {
    m_pMinuteIndicator->Show();
}

void RichPanelKline::HideMinuteIndicator() {
    m_pMinuteIndicator->Hide();
}