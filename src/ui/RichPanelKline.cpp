///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/RichPanelKline.cpp
// Purpose:     minute/five day minute/day/week/month/quarter/year kline panel
// Author:      songhuabiao
// Modified by:
// Created:     2024-06-14 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/RichPanelKline.h"
#include <wx/dcbuffer.h>
#include "ui/RichHelper.h"

const long RichPanelKline::ID_SHARE_NAME_CTRL = wxNewId();
const long RichPanelKline::ID_KLINE_CTRL = wxNewId();
const long RichPanelKline::ID_DIALOG_KLINE_INFO = wxNewId();
const long RichPanelKline::ID_RADIO_CTRL = wxNewId();

// 以下函数实现必须写，否则会爆错误 undefined reference to 'vtable for RichKlineCtrl'
BEGIN_EVENT_TABLE(RichPanelKline, RichPanel)
EVT_PAINT(RichPanelKline::OnPaint)
EVT_SIZE(RichPanelKline::OnSize)
EVT_LEFT_DOWN(RichPanelKline::OnLeftMouseDown)
EVT_KEY_DOWN(RichPanelKline::OnKeyDown)
EVT_ERASE_BACKGROUND(RichPanelKline::OnBackground)
EVT_RICH_RADIO(RichPanelKline::ID_RADIO_CTRL, RichPanelKline::OnKlineChanged)
EVT_MOUSEWHEEL(RichPanelKline::OnMouseWheel)
END_EVENT_TABLE()

RichPanelKline::RichPanelKline(PanelType type,
                               StockDataStorage* pStorage,
                               wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size)
    : RichPanel(type, pStorage, parent, id, pos, size, wxTAB_TRAVERSAL | wxWANTS_CHARS, _T("Panel_Kline")) {
#define TOP_BAR_HEIGHT 18
    // 股票名称
    m_pShareNameCtrl =
        new wxStaticText(this, ID_SHARE_NAME_CTRL, wxT("---"), wxPoint(2, 0), wxSize(64, TOP_BAR_HEIGHT));
    m_pShareNameCtrl->SetBackgroundColour(wxColor(0, 0, 0));
    m_pShareNameCtrl->SetForegroundColour(wxColor(200, 200, 200));

    // 日/周/月/季/年 K线
    std::vector<std::string> options = {"分时", "五日", "日线", "周线", "月线", "季线", "年线"};
    m_pRadioCtrl = new RichRadioCtrl(options, 2, this, ID_RADIO_CTRL, wxPoint(64, 0), wxSize(400, TOP_BAR_HEIGHT));
    // 事件绑定， Radio 控件将始终获取鼠标键盘焦点
    m_pRadioCtrl->Bind(wxEVT_LEFT_UP, &RichPanelKline::OnLeftMouseDown, this);
    // K线主图
    m_ptKlineCtrl = wxPoint(2, TOP_BAR_HEIGHT + 2);
    m_sizeKlineCtrl = size;
    m_sizeKlineCtrl.DecBy(wxSize(0, TOP_BAR_HEIGHT + 2));  // 这里不能使用DecTo,会导致RichKlineCtrl控件宽度为0
    m_pKlineCtrl = new RichKlineCtrl(pStorage, m_ptKlineCtrl, m_sizeKlineCtrl);
    // 成交量/成交额附图
    m_pVolumeBarCtrl = new RichVolumeBarCtrl(m_pKlineCtrl);
    // 日K线信息
    m_pDialogKlineInfo = new RichDialogKlineInfo(this, ID_DIALOG_KLINE_INFO, pos, wxSize(150, 240));
    m_pDialogKlineInfo->Show(false);
    // 使用自动双缓冲
    SetBackgroundStyle(wxBG_STYLE_PAINT);
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
    m_pVolumeBarCtrl->OnPaint(&dc);
}
void RichPanelKline::OnBackground(wxEraseEvent& event) {
    m_pKlineCtrl->OnBackground(event);
}
void RichPanelKline::OnSize(wxSizeEvent& event) {
    m_pKlineCtrl->OnSize(event);
    Refresh();  // 界面需要重绘
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
    this->SetFocus();
    // 一定要最后调用
    event.Skip();
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
    m_pDialogKlineInfo->Move(10, 60);
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
    bool result = m_pKlineCtrl->LoadKlines(static_cast<KlineType>(selection));
    if (!result) {  // 数据有可能加载失败，弹窗提示用户
        return;
    }
    m_pShare = m_pStorage->FindShare(m_pKlineCtrl->GetShareCode());
    if (m_pShare != nullptr) {  // 更新股票名称
        m_pShareNameCtrl->SetLabel(CN(m_pShare->name));
    }
    this->Refresh();
}