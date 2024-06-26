#include "ui/RichMainFrame.h"
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/listctrl.h>
#include <vector>
#include "stock/StockDataStorage.h"
#include "util/Global.h"

//(*IdInit(PanelStockQuote)
const long RichMainFrame::ID_PANEL_STOCK_QUOTE = wxNewId();
//*)
const long RichMainFrame::ID_DIALOG_SHARE_SEARCH = wxNewId();

// catch the event from the thread
BEGIN_EVENT_TABLE(RichMainFrame, wxFrame)
EVT_THREAD(ID_QUOTE_DATA_READY, RichMainFrame::OnStorageDataReady)
END_EVENT_TABLE()

RichMainFrame::RichMainFrame(wxWindow* parent, wxWindowID id, const wxPoint& /*point*/, const wxSize& /*size*/) {
    //(*Initialize(RichMainFrame)
    Create(parent, id, _("东方巴菲特"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxWANTS_CHARS,
           _T("id"));
    SetClientSize(wxSize(364, 600));
    Move(wxDefaultPosition);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
    {
        wxIcon FrameIcon;
        FrameIcon.CopyFromBitmap(
            wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUESTION")), wxART_FRAME_ICON));
        SetIcon(FrameIcon);
    }
    //*)

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
    m_panelStockQuote = new PanelStockQuote(this, ID_PANEL_STOCK_QUOTE, wxPoint(384, 48), wxSize(1240, 600));
    m_panelStockQuote->LoadStockMarketQuote();
    m_panelStockQuote->Show();
    m_panelStockQuote->GetGridCtrl()->Bind(wxEVT_CHAR, &RichMainFrame::OnChar, this);
    m_panelStockQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &RichMainFrame::OnGridCellLeftClick, this);
    m_panelStockQuote->GetGridCtrl()->Bind(wxEVT_GRID_CELL_LEFT_DCLICK, &RichMainFrame::OnGridCellLeftDblClick, this);
    m_panelStockQuote->GetGridCtrl()->Bind(wxEVT_KEY_DOWN, &RichMainFrame::OnKeyDown, this);

    m_dlgShareSearch = new DialogShareSearch(this, ID_DIALOG_SHARE_SEARCH, _T("股票精灵"));
    m_dlgShareSearch->ReLayout(wxSize(320, 320));
    m_dlgShareSearch->Show(false);  // 默认隐藏

    Maximize();  // 初始化最大化
}

void RichMainFrame::OnClose(wxCloseEvent& /*event*/) {
    m_dlgShareSearch->Destroy();
}

void RichMainFrame::OnIconize(wxIconizeEvent& /*event*/) {
    if (m_dlgShareSearch->IsShown()) {
        m_dlgShareSearch->Show(false);
    }
}

void RichMainFrame::OnMaximize(wxMaximizeEvent& /*event*/) {
    if (m_dlgShareSearch->IsShown()) {
        m_dlgShareSearch->Show(false);
    }
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
        std::string keyword = character.ToStdString();
        m_dlgShareSearch->SetKeyword(keyword);
        AdjustDlgShareSearchPostion();
        m_dlgShareSearch->Show();
    }
}

void RichMainFrame::AdjustDlgShareSearchPostion() {
    // 获取主窗口相对桌面的偏移位置
    wxPoint offsetScreen = this->GetScreenPosition();
    // 获取主窗口界面大小
    wxSize frame_size = this->GetSize();
    // 获取股票搜索对话框大小
    wxSize dlg_size = m_dlgShareSearch->GetSize();
    // 获取横向滚动条高度
    int horizontalScrollbarHeight = wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y);
    // 获取和纵向滚动条宽度
    int verticalScrollbarWidth = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    wxPoint pt;
    pt.x = frame_size.x - dlg_size.x + offsetScreen.x - horizontalScrollbarHeight - 4;
    pt.y = frame_size.y - dlg_size.y + offsetScreen.y - verticalScrollbarWidth - 6;
    m_dlgShareSearch->SetPosition(pt);
}

// 监听GridCtrl鼠标左键按下事件
void RichMainFrame::OnGridCellLeftClick(wxGridEvent& /*event*/) {
    if (m_dlgShareSearch) {
        m_dlgShareSearch->Show(false);
    }
}

// 双击股票行情，显示日K线图
void RichMainFrame::OnGridCellLeftDblClick(wxGridEvent& event) {
    int iRow = event.GetRow();
    wxString share_code = m_panelStockQuote->GetGridCtrl()->GetCellValue(iRow, 1);
}

// 监听异步子线程消息
void RichMainFrame::OnStorageDataReady(wxThreadEvent& event) {
    wxString data = event.GetString();
    std::cout << "thread event data: " << event.GetString() << std::endl;
    if (data == "Quote") {
        m_panelStockQuote->LoadStockMarketQuote();
    }
}
void RichMainFrame::OnExit(wxCommandEvent& /*event*/) {
    Close(true);
}

void RichMainFrame::OnAbout(wxCommandEvent& /*event*/) {
    wxMessageBox("财富之眼\n作者:宋华彪", "About iWealth", wxOK | wxICON_INFORMATION);
}

RichMainFrame::~RichMainFrame() {
    //(*Destroy(RichMainFrame)
    //*)
}
