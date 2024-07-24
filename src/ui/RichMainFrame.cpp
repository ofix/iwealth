///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/RichMainFrame.h
// Purpose:     GUI main entry
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-13 20:33
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/RichMainFrame.h"
#include <wx/artprov.h>
#include <wx/listctrl.h>
#include <wx/platinfo.h>
#include <vector>
#include "stock/StockDataStorage.h"
#include "util/Global.h"

const long RichMainFrame::ID_PANEL_STOCK_QUOTE = wxNewId();
const long RichMainFrame::ID_DIALOG_SHARE_SEARCH = wxNewId();
const long RichMainFrame::ID_PANEL_KLINE = wxNewId();

BEGIN_EVENT_TABLE(RichMainFrame, wxFrame)
EVT_THREAD(ID_QUOTE_DATA_READY, RichMainFrame::OnStorageDataReady)
EVT_THREAD(ID_ASYNC_PROCESS_ERROR, RichMainFrame::OnThreadError)
END_EVENT_TABLE()

RichMainFrame::RichMainFrame(wxWindow* parent, wxWindowID id, const wxPoint& /*point*/, const wxSize& /*size*/) {
    // 必须先实例化 StockDataStorage，否则RichPanel子类创建会失败
    // 数据加载放在窗口显示之后，不阻塞窗口初始化
    m_pStorage = new StockDataStorage(true);
    m_pStorage->SetEventHandler(this);

    Create(parent, id, _("东方巴菲特"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxWANTS_CHARS,
           _T("id"));
    SetClientSize(wxSize(1024, 580));
    SetMinSize(wxSize(1024, 580));  // 设置最小窗口大小为 300x200
    Move(wxDefaultPosition);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
    {
        wxIcon FrameIcon;
        FrameIcon.CopyFromBitmap(
            wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUESTION")), wxART_FRAME_ICON));
        SetIcon(FrameIcon);
    }

    m_panelStack = {};
    m_panelCurrent = nullptr;
    m_panelPos = -1;

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText(_T("财富之眼"));

    Bind(wxEVT_MENU, &RichMainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &RichMainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_CLOSE_WINDOW, &RichMainFrame::OnClose, this);
    Bind(wxEVT_ICONIZE, &RichMainFrame::OnIconize, this);
    Bind(wxEVT_MAXIMIZE, &RichMainFrame::OnMaximize, this);
    // 初始化主窗口面板
    RichPanelStockQuote* panelQuote = new RichPanelStockQuote(PanelType::Quote, m_pStorage, this, ID_PANEL_STOCK_QUOTE,
                                                              wxPoint(384, 48), wxSize(1240, 600));
    panelQuote->LoadStockMarketQuote();
    panelQuote->GetGridCtrl()->Bind(wxEVT_CHAR, &RichMainFrame::OnChar, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &RichMainFrame::OnGridCellLeftClick, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &RichMainFrame::OnGridCellLeftDblClick, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_KEY_DOWN, &RichMainFrame::OnKeyDown, this);
    Bind(wxEVT_RICH_SHARE_SEARCH, &RichMainFrame::OnSearchShare, this);
    AddPanelToStack(panelQuote);

    m_dlgShareSearch = new RichDialogShareSearch(
        m_pStorage, this, ID_DIALOG_SHARE_SEARCH, _T("股票精灵"), wxDefaultPosition, wxDefaultSize,
        (wxDEFAULT_DIALOG_STYLE & ~(wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)) | wxBORDER_NONE);  // 移除默认标题栏
    m_dlgShareSearch->ReLayout(wxSize(290, 380));
    m_dlgShareSearch->Show(false);  // 默认隐藏

    Maximize();  // 主窗口最大化
}

void RichMainFrame::LoadQuote() {
    RichResult result = m_pStorage->Init();
    if (!result.Ok()) {
        wxMessageBox(result.What());
    }
    RichPanelStockQuote* panelQuote = static_cast<RichPanelStockQuote*>(m_panelCurrent);
    panelQuote->LoadStockMarketQuote();
}

StockDataStorage* RichMainFrame::GetStockDataStorage() {
    return m_pStorage;
}

void RichMainFrame::OnClose(wxCloseEvent& event) {
    m_dlgShareSearch->Destroy();
    event.Skip();
}

void RichMainFrame::OnIconize(wxIconizeEvent& event) {
    if (m_dlgShareSearch->IsShown()) {
        m_dlgShareSearch->Show(false);
    }
    event.Skip();
}

void RichMainFrame::OnMaximize(wxMaximizeEvent& event) {
    if (m_dlgShareSearch->IsShown()) {
        m_dlgShareSearch->Show(false);
    }
    event.Skip();
}

// 全局处理 Alt+Tab 按键消息
void RichMainFrame::OnKeyDown(wxKeyEvent& event) {
    int key_code = event.GetKeyCode();
    if (key_code == WXK_TAB && event.AltDown()) {
        std::cout << " alt+tab 按下了" << std::endl;
    } else if (key_code == WXK_ESCAPE) {
        PopPanelFromStack();
    } else {
        event.Skip();
    }
}

// 处理按键按下事件
void RichMainFrame::OnChar(wxKeyEvent& event) {
    int key_code = event.GetKeyCode();
    if (key_code != 8) {
        wxChar key = event.GetUnicodeKey();
        wxString character(key);
        std::string keyword = character.utf8_string();
        m_dlgShareSearch->SetKeyword(keyword);
        AdjustDlgShareSearchPostion();
        m_dlgShareSearch->Show();
    }
    event.Skip();
}

void RichMainFrame::AdjustDlgShareSearchPostion() {
    // 获取主窗口相对桌面的偏移位置
    wxPoint offsetScreen = this->GetScreenPosition();
    // 获取主窗口界面大小
    wxSize frame_size = this->GetSize();
    // 获取股票搜索对话框大小
    wxSize dlg_size = m_dlgShareSearch->GetSize();
    // 根据平台进行补偿
    wxString platform = wxPlatformId::GetCurrent();
    int verticalScrollbarWidth = 0;
    int horizontalScrollbarHeight = 0;
    if (platform == "msw") {
        horizontalScrollbarHeight = 23;
        verticalScrollbarWidth = 32;
    } else if (platform == "unix") {
        horizontalScrollbarHeight = 0;
        verticalScrollbarWidth = 66;
    }
    wxPoint pt;
    pt.x = frame_size.x - dlg_size.x + offsetScreen.x - horizontalScrollbarHeight;
    pt.y = frame_size.y - dlg_size.y + offsetScreen.y - verticalScrollbarWidth;
    m_dlgShareSearch->SetPosition(pt);
}

// 监听GridCtrl鼠标左键按下事件
void RichMainFrame::OnGridCellLeftClick(wxGridEvent& event) {
    if (m_dlgShareSearch) {
        m_dlgShareSearch->Show(false);
    }
    event.Skip();
}

RichPanel* RichMainFrame::GetCurrentPanel() {
    return m_panelCurrent;
}

// 双击股票行情，显示日K线图
void RichMainFrame::OnGridCellLeftDblClick(wxGridEvent& event) {
    int iRow = event.GetRow();
    RichGrid* pGrid = static_cast<RichGrid*>(event.GetEventObject());
    wxString share_code = pGrid->GetCellValue(iRow, 1);
    std::string _share_code = share_code.ToStdString();
    ShowKlinePanel(_share_code);
    event.Skip();
}

void RichMainFrame::OnSearchShare(RichShareSearchEvent& event) {
    ShowKlinePanel(event.GetShareCode());
}

void RichMainFrame::ShowKlinePanel(const std::string& share_code) {
    if (m_panelCurrent->GetType() == PanelType::Kline) {
        dynamic_cast<RichPanelKline*>(m_panelCurrent)->SetShareCode(share_code);
    } else {
        wxPoint pos = m_panelCurrent->GetPosition();
        wxSize size = m_panelCurrent->GetSize();
        RichPanelKline* panelKline = new RichPanelKline(PanelType::Kline, m_pStorage, this, ID_PANEL_KLINE, pos, size);
        panelKline->SetShareCode(share_code);
        AddPanelToStack(panelKline);
    }
}

void RichMainFrame::AddPanelToStack(RichPanel* panel) {
    if (m_panelCurrent != nullptr) {
        m_panelCurrent->Show(false);
    }
    m_panelStack.push_back(panel);
    m_panelCurrent = panel;
    m_panelPos += 1;
    m_panelCurrent->Show(true);
}

void RichMainFrame::PopPanelFromStack() {
    if (m_panelStack.size() > 1) {
        RichPanel* panel = m_panelStack[m_panelStack.size() - 1];
        m_panelStack.pop_back();
        m_panelPos -= 1;
        m_panelCurrent = m_panelStack[m_panelPos];
        // 必须释放，否则会内存泄露，请参考以下链接:
        // https://stackoverflow.com/questions/28746161/does-stdvectorpop-back-set-the-pointers-of-the-objects-in-it-to-nullptr
        delete panel;
        m_panelCurrent->Show();
    }
}

// 监听异步子线程消息
void RichMainFrame::OnStorageDataReady(wxThreadEvent& event) {
    wxString data = event.GetString();
    if (data == "Quote") {
        RichPanelStockQuote* panelQuote = static_cast<RichPanelStockQuote*>(m_panelCurrent);
        panelQuote->LoadStockMarketQuote();
    }
}

void RichMainFrame::OnThreadError(wxThreadEvent& event) {
    wxString error = event.GetString();
    wxMessageBox(error);
}

void RichMainFrame::OnExit(wxCommandEvent& event) {
    event.Skip();
}

void RichMainFrame::OnAbout(wxCommandEvent& /*event*/) {
    wxMessageBox("财富之眼\n作者:宋华彪", "About iWealth", wxOK | wxICON_INFORMATION);
}

RichMainFrame::~RichMainFrame() {
    //(*Destroy(RichMainFrame)
    //*)
}
