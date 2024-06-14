#include "ui/RichMainFrame.h"
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/listctrl.h>
#include <vector>

//(*IdInit(PanelStockQuota)
const long RichMainFrame::ID_PANEL_STOCK_QUOTE = wxNewId();
//*)

RichMainFrame::RichMainFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    //(*Initialize(RichMainFrame)
    Create(parent, id, _("东方巴菲特"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(364,600));
    Move(wxDefaultPosition);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUESTION")),wxART_FRAME_ICON));
    	SetIcon(FrameIcon);
    }
    //*)

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText(_T("财富之眼"));

    Bind(wxEVT_MENU, &RichMainFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &RichMainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &RichMainFrame::OnExit, this, wxID_EXIT);
    // 初始化主窗口面板
    m_panelStockQuota = new PanelStockQuota(this, ID_PANEL_STOCK_QUOTE, wxPoint(384, 48), wxSize(1240, 600));
    m_panelStockQuota->LoadStockMarketQuote();

    Maximize(); // 初始化最大化
}

void RichMainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void RichMainFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
}

void RichMainFrame::OnHello(wxCommandEvent& event) {
    wxLogMessage("Hello world from wxWidgets!");
}

RichMainFrame::~RichMainFrame() {
    //(*Destroy(RichMainFrame)
    //*)
}
