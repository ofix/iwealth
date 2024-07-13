///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/ui/commponents/RichPanelKline.cpp
// Purpose:     Day/Week/Month/Quoter/Year Kline panel
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-13 19:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichPanelKline.h"
#include <wx/dcbuffer.h>
#include "ui/RichApplication.h"

const long RichPanelKline::ID_KLINE_CTRL = wxNewId();
const long RichPanelKline::ID_DIALOG_KLINE_INFO = wxNewId();
const long RichPanelKline::ID_RAIDO_CTRL = wxNewId();

// 以下函数实现必须写，否则会爆错误 undefined reference to 'vtable for RichKlineCtrl'
BEGIN_EVENT_TABLE(RichPanelKline, RichPanel)
EVT_PAINT(RichPanelKline::OnPaint)
EVT_SIZE(RichPanelKline::OnSize)
EVT_LEFT_DOWN(RichPanelKline::OnLeftMouseDown)
EVT_KEY_DOWN(RichPanelKline::OnKeyDown)
EVT_ERASE_BACKGROUND(RichPanelKline::OnBackground)
END_EVENT_TABLE()

RichPanelKline::RichPanelKline(PanelType type, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : RichPanel(type, parent, id, pos, size, wxTAB_TRAVERSAL | wxWANTS_CHARS, _T("Panel_Kline")) {
    std::vector<std::string> options = {"分时", "日线", "周线", "月线", "季线", "年线"};
    m_pRadioCtrl = new RichRadioCtrl(options, 1, this, ID_RAIDO_CTRL, wxPoint(2, 2), wxSize(600, 28));
    wxSize klineSize = size;
    klineSize.DecBy(wxSize(0, 30));  // 这里不能使用DecTo,会导致RichKlineCtrl控件宽度为0
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    m_pKlineCtrl = new RichKlineCtrl(pStorage, wxPoint(2, 30), klineSize);
    m_pVolumeBarCtrl = new RichVolumeBarCtrl(m_pKlineCtrl);
    m_pDialogKlineInfo = new RichDialogKlineInfo(m_pKlineCtrl, this, ID_DIALOG_KLINE_INFO, pos, wxSize(200, 400));
    m_pDialogKlineInfo->Show(false);
    // 使用自动双缓冲
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    // 事件绑定， Radio 控件将始终获取鼠标键盘焦点
    m_pRadioCtrl->Bind(wxEVT_LEFT_UP, &RichPanelKline::OnLeftMouseDown, this);
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
    m_share_code = share_code;
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    if (!pStorage->IsLocalFileShareKlinesExist(share_code)) {
        pStorage->FetchKlineSync(share_code, KlineType::Day);
        pStorage->SaveShareKlines(share_code, KlineType::Day);
    }
    m_pKlineCtrl->LoadKlines(share_code);
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
    Refresh();  // 界面需要重绘
    event.Skip();
}
void RichPanelKline::OnLeftMouseDown(wxMouseEvent& event) {
    m_pKlineCtrl->OnLeftMouseDown(event);
    Refresh();
    event.Skip();
}