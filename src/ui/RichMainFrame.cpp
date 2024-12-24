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
#include <wx/dcbuffer.h>
#include <wx/listctrl.h>
#include <wx/minifram.h>
#include <wx/platinfo.h>
#include <vector>
#include "stock/StockDataStorage.h"
#include "ui/RichHelper.h"
#include "ui/components/RichPngKline.h"
#include "util/Global.h"

const long RichMainFrame::ID_PANEL_STOCK_QUOTE = wxNewId();
const long RichMainFrame::ID_DIALOG_SHARE_SEARCH = wxNewId();
const long RichMainFrame::ID_PANEL_KLINE = wxNewId();
const long ID_MAIN_FRAME = wxNewId();

BEGIN_EVENT_TABLE(RichMainFrame, wxFrame)
EVT_THREAD(ID_QUOTE_DATA_READY, RichMainFrame::OnStorageDataReady)
EVT_THREAD(ID_ASYNC_PROCESS_ERROR, RichMainFrame::OnThreadError)
END_EVENT_TABLE()

RichMainFrame::RichMainFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    // 必须先实例化 StockDataStorage，否则RichPanel子类创建会失败
    // 数据加载放在窗口显示之后，不阻塞窗口初始化
    m_pStorage = new StockDataStorage(true);
    m_pStorage->SetEventHandler(this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_MENU, &RichMainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &RichMainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_SIZE, &RichMainFrame::OnSize, this, ID_MAIN_FRAME);
    // Bind(wxEVT_NC_PAINT, &RichMainFrame::OnPaintTitleBar, this);

    // bind mouse event for frame dragging
    // Bind(wxEVT_LEFT_DOWN, &RichMainFrame::OnMouseLeftDown, this);
    // Bind(wxEVT_LEFT_UP, &RichMainFrame::OnMouseLeftUp, this);
    // Bind(wxEVT_MOUSE_CAPTURE_LOST, &RichMainFrame::OnMouseCaptureLost, this);
    // Bind(wxEVT_MOTION, &RichMainFrame::OnMouseMove, this);

#if defined(_WIN32) || defined(__WIN64)
    Create(parent, id, _(""), wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS | wxRESIZE_BORDER, _T("id"));
#else
    Create(parent, id, _("猛龙证券"), wxDefaultPosition, wxSize(600, 800), wxWANTS_CHARS | wxDEFAULT_FRAME_STYLE,
           _T(""));
#endif
    SetClientSize(wxSize(1024, 580));
    SetMinSize(wxSize(1024, 580));
    Move(wxDefaultPosition);
    SetBackgroundColour(wxColor(255, 255, 255));
    {
        // wxIcon FrameIcon;
        // FrameIcon.CopyFromBitmap(
        //     wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUESTION")), wxART_FRAME_ICON));
        // SetIcon(FrameIcon);
    }

    m_panelStack = {};
    m_panelCurrent = nullptr;
    m_panelPos = -1;

    // m_topBar = new RichTopBar(this, ID_TOP_BAR, wxPoint(0, 0), wxSize(-1, 32), 0, _("猛龙证券"));

    // // RichMenu
    // RichTopMenu* menuFavoriteShare = new RichTopMenu(CN("自选"), m_topBar);
    // RichTopMenu* menuMarketQuote = new RichTopMenu(CN("行情"), m_topBar);
    // RichTopMenu* menuDiscovery = new RichTopMenu(CN("挖掘"), m_topBar);
    // RichTopMenu* menuFormula = new RichTopMenu(CN("智选"), m_topBar);
    // m_topBar->AddMenu(menuFavoriteShare);
    // m_topBar->AddMenu(menuMarketQuote);
    // m_topBar->AddMenu(menuDiscovery);
    // m_topBar->AddMenu(menuFormula);

    // wxMenu* menuFile = new wxMenu;
    // menuFile->Append(wxID_EXIT);

    // wxMenu* menuHelp = new wxMenu;
    // menuHelp->Append(wxID_ABOUT);

    // wxMenuBar* menuBar = new wxMenuBar;
    // menuBar->Append(menuFile, "&File");
    // menuBar->Append(menuHelp, "&Help");

    // SetMenuBar(menuBar);

    // CreateStatusBar();
    // SetStatusText(_T("财富之眼"));

    // 初始化主窗口面板
    RichPanelStockQuote* panelQuote = new RichPanelStockQuote(PanelType::Quote, m_pStorage, this, ID_PANEL_STOCK_QUOTE,
                                                              wxPoint(0, 32), wxSize(1240, 600));
    panelQuote->LoadStockMarketQuote();
    panelQuote->GetGridCtrl()->Bind(wxEVT_CHAR, &RichMainFrame::OnChar, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &RichMainFrame::OnGridCellLeftClick, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &RichMainFrame::OnGridCellLeftDblClick, this);
    Bind(wxEVT_RICH_SHARE_SEARCH, &RichMainFrame::OnSearchShare, this);
    // 修正RichPanelKline 按下ESC按键，RichMainFrame::OnKeyDown 无法捕获的问题
    Bind(wxEVT_CHAR_HOOK, &RichMainFrame::OnKeyDown, this);
    AddPanelToStack(panelQuote);

    m_dlgShareSearch = new RichDialogShareSearch(
        m_pStorage, this, ID_DIALOG_SHARE_SEARCH, _T("股票精灵"), wxDefaultPosition, wxDefaultSize,
        (wxDEFAULT_DIALOG_STYLE & ~(wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)) | wxBORDER_NONE);  // 移除默认标题栏
    m_dlgShareSearch->ReLayout(wxSize(290, 380));
    m_dlgShareSearch->Show(false);  // 默认隐藏

    wxBoxSizer* verticalSizer = new wxBoxSizer(wxVERTICAL);
    verticalSizer->Add(panelQuote, 1, wxEXPAND | wxALL, 0);
    SetSizer(verticalSizer);
    Layout();
    Maximize(true);  // 主窗口最大化
}

// bool RichMainFrame::CanDragFrame(wxPoint& ptMouse) {
//     wxSize size = GetSize();
//     std::cout << "mouse point: " << ptMouse.x << "," << ptMouse.y << std::endl;
//     std::cout << "frame size: " << size.GetWidth() << "," << size.GetHeight() << std::endl;
//     if (ptMouse.x == 0 || ptMouse.y == 0 || ptMouse.x == size.GetWidth() || ptMouse.y == size.GetHeight()) {
//         std::cout << "can drag frame" << std::endl;
//         return true;
//     }
//     std::cout << "can't drag frame" << std::endl;
//     return false;
// }

// void RichMainFrame::OnMouseLeftDown(wxMouseEvent& event) {
//     m_bLeftMouseDown = true;
//     if (!m_dragging) {
//         wxPoint clientStart = event.GetPosition();
//         if (CanDragFrame(clientStart)) {
//             m_dragging = true;
//             wxPoint clientStart = event.GetPosition();
//             m_dragStartMouse = ClientToScreen(clientStart);
//             m_dragStartWindow = GetParent()->GetPosition();
//             CaptureMouse();
//         }
//     }
// }

// void RichMainFrame::OnMouseLeftUp(wxMouseEvent&) {
//     m_bLeftMouseDown = false;
//     // if (m_hitState == TopBarHitState::BTN_CLOSE) {
//     //     wxCommandEvent event(wxEVT_CLOSE_WINDOW);
//     //     wxPostEvent(GetParent(), event);
//     // }
//     FinishDrag();
// }

// void RichMainFrame::OnMouseMove(wxMouseEvent& event) {
//     std::cout << "mouse move" << std::endl;
//     if (m_dragging) {
//         wxPoint curClientPsn = event.GetPosition();
//         wxPoint curScreenPsn = ClientToScreen(curClientPsn);
//         wxPoint movementVector = curScreenPsn - m_dragStartMouse;
//         // GetParent()->SetPosition(m_dragStartWindow + movementVector);
//     } else {
//         wxPoint curClientPsn = event.GetPosition();
//         if (CanDragFrame(curClientPsn)) {
//             wxCursor we_cursor(wxCURSOR_SIZEWE);
//             SetCursor(we_cursor);
//         } else {
//             wxCursor arrow_cursor(wxCURSOR_ARROW);
//             SetCursor(arrow_cursor);
//         }
//     }
// }

// void RichMainFrame::OnMouseCaptureLost(wxMouseCaptureLostEvent&) {
//     FinishDrag();
// }

// void RichMainFrame::FinishDrag() {
//     if (m_dragging) {
//         m_dragging = false;
//     }
//     if (HasCapture()) {
//         ReleaseMouse();
//     }
// }

int RichMainFrame::GetTitleBarHeight() {
    int h = GetSize().GetHeight() - GetClientSize().GetHeight();
    return h;
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
    std::cout << "MainFrame Maximize" << std::endl;
    event.Skip();
}

void RichMainFrame::OnSize(wxSizeEvent& event) {
    event.Skip();
}

// 全局处理 Alt+Tab 按键消息
void RichMainFrame::OnKeyDown(wxKeyEvent& event) {
    int key = event.GetKeyCode();
    if (key == WXK_TAB && event.AltDown()) {
        std::cout << " alt+tab 按下了" << std::endl;
    } else if (key == WXK_ESCAPE) {
        PopPanelFromStack();
    } else if (key != WXK_ALT && key != WXK_TAB && key != WXK_LEFT && key != WXK_RIGHT && key != WXK_UP &&
               key != WXK_DOWN && key != WXK_SHIFT && key != WXK_HOME && key != WXK_END && key != WXK_NUMPAD_END &&
               key != WXK_NUMPAD_HOME && key != WXK_NUMPAD_PAGEUP && key != WXK_NUMPAD_PAGEDOWN &&
               key != WXK_NUMPAD_LEFT && key != WXK_NUMPAD_RIGHT) {
        OnChar(event);
    }
    // 必须放在最后，否则RichPanelKline 无法响应按键消息
    event.Skip();
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
    wxPoint offsetScreen = GetScreenPosition();
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
        AddPanelToStack(panelKline);
        panelKline->SetShareCode(share_code);
        // 需要动态修改wxBoxSizer绑定的元素,否则缩放窗口，子元素并不会自适应
        wxBoxSizer* verticalSizer = (wxBoxSizer*)GetSizer();
        verticalSizer->Detach(0);
        verticalSizer->Add(panelKline, 1, wxEXPAND | wxALL, 0);
        verticalSizer->Layout();
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
