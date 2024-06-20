#include "ui/RichMainFrame.h"
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/listctrl.h>
#include <vector>
#include "stock/StockDataStorage.h"

//(*IdInit(PanelStockQuote)
const long RichMainFrame::ID_PANEL_STOCK_QUOTE = wxNewId();
//*)

// catch the event from the thread
BEGIN_EVENT_TABLE(RichMainFrame, wxFrame)
EVT_THREAD(ID_QUOTE_DATA_READY, RichMainFrame::OnStorageDataReady)
END_EVENT_TABLE()

RichMainFrame::RichMainFrame(wxWindow* parent, wxWindowID id, const wxPoint& /*point*/, const wxSize& /*size*/) {
    //(*Initialize(RichMainFrame)
    Create(parent, id, _("东方巴菲特"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
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
    m_panelStockQuota = new PanelStockQuote(this, ID_PANEL_STOCK_QUOTE, wxPoint(384, 48), wxSize(1240, 600));
    m_panelStockQuota->Show();

    Maximize();  // 初始化最大化
}

// 监听异步子线程消息
void RichMainFrame::OnStorageDataReady(wxThreadEvent& event) {
    wxString data = event.GetString();
    std::cout << "thread event data: " << event.GetString() << std::endl;
    if (data == "Quote") {
        m_panelStockQuota->LoadStockMarketQuote();
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
