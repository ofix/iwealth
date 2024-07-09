#include "ui/RichMainFrame.h"
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/listctrl.h>
#include <wx/platinfo.h>
#include <vector>
#include "stock/StockDataStorage.h"
#include "ui/RichApplication.h"
#include "util/Global.h"

//(*IdInit(PanelStockQuote)
const long RichMainFrame::ID_PANEL_STOCK_QUOTE = wxNewId();
//*)
const long RichMainFrame::ID_DIALOG_SHARE_SEARCH = wxNewId();
const long RichMainFrame::ID_PANEL_KLINE = wxNewId();

// catch the event from the thread
BEGIN_EVENT_TABLE(RichMainFrame, wxFrame)
EVT_THREAD(ID_QUOTE_DATA_READY, RichMainFrame::OnStorageDataReady)
END_EVENT_TABLE()

RichMainFrame::RichMainFrame(wxWindow* parent, wxWindowID id, const wxPoint& /*point*/, const wxSize& /*size*/) {
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
    PanelStockQuote* panelQuote = new PanelStockQuote(this, ID_PANEL_STOCK_QUOTE, wxPoint(384, 48), wxSize(1240, 600));
    panelQuote->LoadStockMarketQuote();
    panelQuote->GetGridCtrl()->Bind(wxEVT_CHAR, &RichMainFrame::OnChar, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &RichMainFrame::OnGridCellLeftClick, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &RichMainFrame::OnGridCellLeftDblClick, this);
    panelQuote->GetGridCtrl()->Bind(wxEVT_KEY_DOWN, &RichMainFrame::OnKeyDown, this);
    AddPanelToStack(panelQuote);

    m_dlgShareSearch = new DialogShareSearch(
        this, ID_DIALOG_SHARE_SEARCH, _T("股票精灵"), wxDefaultPosition, wxDefaultSize,
        (wxDEFAULT_DIALOG_STYLE & ~(wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)) | wxBORDER_NONE);  // 移除默认标题栏
    m_dlgShareSearch->ReLayout(wxSize(290, 380));
    m_dlgShareSearch->Show(false);  // 默认隐藏

    Maximize();  // 初始化最大化
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
    if (event.GetKeyCode() == WXK_TAB && event.AltDown()) {
        std::cout << " alt+tab 被按下了" << std::endl;
    } else {
        event.Skip();
    }
}

// 处理按键按下事件
void RichMainFrame::OnChar(wxKeyEvent& event) {
    int keycode = event.GetKeyCode();
    if (keycode != 8) {
        wxChar key = event.GetUnicodeKey();
        wxString character(key);
        std::string keyword = character.utf8_string();
        m_dlgShareSearch->SetKeyword(keyword);
        AdjustDlgShareSearchPostion();
        m_dlgShareSearch->Show();
        // 如果行情数据表格有选中行，取消选中
        // if (m_panelStockQuote->GetGridCtrl()->IsSelection()) {
        //     m_panelStockQuote->GetGridCtrl()->ClearSelection();
        // }
    }
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

// 双击股票行情，显示日K线图
void RichMainFrame::OnGridCellLeftDblClick(wxGridEvent& event) {
    int iRow = event.GetRow();
    RichGrid* pGrid = static_cast<RichGrid*>(event.GetEventObject());
    wxString share_code = pGrid->GetCellValue(iRow, 1);
    std::string _share_code = share_code.ToStdString();
    wxPoint pos = m_panelCurrent->GetPosition();
    wxSize size = m_panelCurrent->GetSize();
    PanelKline* panelKline = new PanelKline(this, ID_PANEL_KLINE, pos, size);
    panelKline->SetShareCode(_share_code);
    AddPanelToStack(panelKline);

    event.Skip();
}

void RichMainFrame::AddPanelToStack(wxPanel* panel) {
    if (m_panelCurrent != nullptr) {
        m_panelCurrent->Show(false);
    }
    m_panelStack.push_back(panel);
    m_panelCurrent = panel;
    m_panelCurrent->Show(true);
}

wxPanel* RichMainFrame::PopPanelFromStack() {
    if (m_panelStack.size() > 0) {
        wxPanel* panel = m_panelStack[m_panelStack.size() - 1];
        m_panelStack.pop_back();
        return panel;
    }
    return nullptr;
}

// 监听异步子线程消息
void RichMainFrame::OnStorageDataReady(wxThreadEvent& event) {
    wxString data = event.GetString();
    if (data == "Quote") {
        PanelStockQuote* panelQuote = static_cast<PanelStockQuote*>(m_panelCurrent);
        panelQuote->LoadStockMarketQuote();
    }
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
