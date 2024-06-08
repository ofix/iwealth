#include "ui/RichMainFrame.h"
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/listctrl.h>
#include <vector>
#include "spider/SpiderShareListHexun.h"
#include "ui/RichApplication.h"

RichMainFrame::RichMainFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    //(*Initialize(RichMainFrame)
    Create(parent, id, _("东方巴菲特333"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(364, 600));
    Move(wxDefaultPosition);
    {
        wxIcon FrameIcon;
        FrameIcon.CopyFromBitmap(
            wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_DEL_BOOKMARK")), wxART_FRAME_ICON));
        SetIcon(FrameIcon);
    }
    ListViewStockQuote = new wxListCtrl(this, ID_LISTVIEW_STOCK_QUOTE, wxPoint(384, 48), wxSize(728, 600),
                                        wxLC_REPORT | wxVSCROLL | wxHSCROLL | wxLC_VRULES, wxDefaultValidator,
                                        _T("ID_LISTVIEW_STOCK_QUOTE"));
    wxFont ListViewStockQuoteFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false,
                                  _T("微软雅黑"), wxFONTENCODING_DEFAULT);
    ListViewStockQuote->SetFont(ListViewStockQuoteFont);
    //*)

    /// 插入行情列表表头
    std::vector<std::pair<wxString, int>> columnsStockQuote = {
        {CN("代码"), 50}, {CN("名称"), 50}, {CN("涨幅"), 50}, {CN("现价"), 50}, {CN("换手"), 50}};

    std::cout << "column size:" << columnsStockQuote.size() << std::endl;

    size_t colIdx = 0;
    for (std::pair<wxString, int>& item : columnsStockQuote) {
        wxListItem col;
        col.SetId(colIdx);
        col.SetText(item.first);
        col.SetWidth(item.second);
        ListViewStockQuote->InsertColumn(colIdx++, col);
    }
    /////////////////////////////////////////////////

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
    SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_MENU, &RichMainFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &RichMainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &RichMainFrame::OnExit, this, wxID_EXIT);
}

void RichMainFrame::LoadStockMarketQuote() {
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    if (pStorage) {
        SpiderShareListHexun* spiderKline = new SpiderShareListHexun(pStorage);
        spiderKline->Crawl();  // 当前线程同步爬取市场行情数据
        std::vector<Share> shares = pStorage->GetMarketAllShares();
        for (Share& share : shares) {
        }
    }
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
