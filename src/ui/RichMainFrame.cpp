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
    // 初始化主窗口面板
    m_panelStockQuote = new PanelStockQuote(this, ID_PANEL_STOCK_QUOTE, wxPoint(384, 48), wxSize(1240, 600));
    m_panelStockQuote->LoadStockMarketQuote();
    m_panelStockQuote->Show();
    m_panelStockQuote->GetGridCtrl()->Bind(wxEVT_CHAR, &RichMainFrame::OnChar, this);

    m_dlgShareSearch = new DialogShareSearch(this, ID_DIALOG_SHARE_SEARCH, _T("股票精灵"));
    m_dlgShareSearch->ReLayout(wxSize(320, 320));
    m_dlgShareSearch->Show(false);  // 默认隐藏

    Maximize();  // 初始化最大化
}

// 处理按键按下事件
void RichMainFrame::OnChar(wxKeyEvent& event) {
    int keycode = event.GetKeyCode();
    if (keycode != 8) {
        wxChar key = event.GetUnicodeKey();
        wxString character(key);
        std::string keyword = character.ToStdString();
        wxSize frame_size = this->GetSize();
        wxSize popup_size = m_dlgShareSearch->GetSize();
        wxPoint pt;
        std::cout << "[Grid] " << event.GetKeyCode() << std::endl;

        pt.x = frame_size.GetWidth() - popup_size.GetWidth() - 210;
        pt.y = frame_size.GetHeight() - popup_size.GetHeight() - 110;
        m_dlgShareSearch->SetPosition(pt);
        m_dlgShareSearch->SetKeyword(keyword);
        m_dlgShareSearch->Show();
    }
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
