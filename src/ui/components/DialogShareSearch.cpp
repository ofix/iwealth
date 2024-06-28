#include "ui/components/DialogShareSearch.h"

//(*InternalHeaders(DialogShareSearch)
#include <wx/intl.h>
#include <wx/string.h>
#include "stock/StockDataStorage.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"

//*)

//(*IdInit(DialogShareSearch)
const long DialogShareSearch::ID_TEXTCTRL_KEYWORD = wxNewId();
const long DialogShareSearch::ID_LISTCTRL_SHARELIST = wxNewId();
const long DialogShareSearch::ID_STATICTEXT_TITLE = wxNewId();
const long DialogShareSearch::ID_BITMAPBUTTON_CLOSE = wxNewId();
//*)

BEGIN_EVENT_TABLE(DialogShareSearch, wxDialog)
//(*EventTable(DialogShareSearch)
//*)
END_EVENT_TABLE()

DialogShareSearch::DialogShareSearch(wxWindow* parent,
                                     wxWindowID id,
                                     const wxString& title,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxString& name) {
    //(*Initialize(DialogShareSearch)
    Create(parent, id, title, pos, size, style, name);
    //*)
    // 自定义标题栏标题
    m_statictext_title = new wxStaticText(this, ID_STATICTEXT_TITLE, _T("按键精灵"), wxPoint(5, 5), wxDefaultSize);
    m_statictext_title->SetFont(RichApplication::GetDefaultFont(12));
    // 自定义右侧关闭按钮
    m_bitmapbutton_close = wxBitmapButton::NewCloseButton(this, ID_BITMAPBUTTON_CLOSE);
    m_bitmapbutton_close->SetPosition(wxPoint(255, 5));
    m_bitmapbutton_close->Bind(wxEVT_BUTTON, &DialogShareSearch::OnExitSearchShare, this);

    // 搜索关键字
    m_textctrl_keyword =
        new wxTextCtrl(this, ID_TEXTCTRL_KEYWORD, "", wxPoint(5, 32), wxSize(200, 32), wxTE_PROCESS_ENTER);
    m_textctrl_keyword->Bind(wxEVT_TEXT, &DialogShareSearch::OnSearchShare, this);
    m_textctrl_keyword->Bind(wxEVT_TEXT_ENTER, &DialogShareSearch::OnExitSearchShare, this);
    m_textctrl_keyword->SetFont(RichApplication::GetDefaultFont(11));
    // 列表框
    m_listctrl_sharelist = new wxListCtrl(this, ID_LISTCTRL_SHARELIST, wxPoint(5, 75), wxSize(200, 148),
                                          wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER);
    m_listctrl_sharelist->InsertColumn(0, CN("代码"), 0, 90);
    m_listctrl_sharelist->InsertColumn(1, CN("名称"), 0, 100);
    m_listctrl_sharelist->InsertColumn(2, CN("市场"), 0, 90);
    m_listctrl_sharelist->SetFont(RichApplication::GetDefaultFont(12));
}

void DialogShareSearch::ReLayout(const wxSize& size) {
    this->SetSize(size);
    m_bitmapbutton_close->SetPosition(wxPoint(size.x - 20, 5));
    wxSize size_textctrl;
    size_textctrl.SetWidth(size.GetWidth() - 10);
    size_textctrl.SetHeight(32);
    m_textctrl_keyword->SetSize(size_textctrl);

    wxSize size_listctrl;
    size_listctrl.SetWidth(size.GetWidth() - 10);
    size_listctrl.SetHeight(300);
    m_listctrl_sharelist->SetSize(size_listctrl);
}

void DialogShareSearch::SetShareList(const std::vector<Share*>& shares) {
    m_listctrl_sharelist->DeleteAllItems();
    long row = 0;
    for (auto& pShare : shares) {
        m_listctrl_sharelist->InsertItem(row, _T(""));  // 必须先插入行数据，然后插入表格数据
        m_listctrl_sharelist->SetItem(row, 0, CN(pShare->code));
        m_listctrl_sharelist->SetItem(row, 1, CN(pShare->name));
        m_listctrl_sharelist->SetItem(row, 2, CN(MARKET_NAME(pShare->market)));
        row++;
    }
    if (shares.size() > 0) {
        // m_listctrl_sharelist->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_listctrl_sharelist->SetItemBackgroundColour(0, wxColor(201, 201, 237));
        m_listctrl_sharelist->SetItemTextColour(0, wxColor(52, 52, 227));
    }
}

void DialogShareSearch::SetKeyword(const std::string& keyword) {
    m_textctrl_keyword->SetValue(keyword);
    m_textctrl_keyword->SetFocus();
    m_textctrl_keyword->SetInsertionPointEnd();  // 移动光标到输入字符串末尾
    Raise();                                     // 将搜索窗口置于最顶部
}

void DialogShareSearch::OnSearchShare(wxCommandEvent& /*event*/) {
    wxString input = m_textctrl_keyword->GetValue();
    std::string keyword = input.ToStdString();
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    std::vector<Share*> shares = pStorage->SearchShares(keyword);
    SetShareList(shares);
}

void DialogShareSearch::OnExitSearchShare(wxCommandEvent& /*event*/) {
    m_textctrl_keyword->Clear();
    this->Show(false);
}

DialogShareSearch::~DialogShareSearch() {
    //(*Destroy(DialogShareSearch)
    //*)
}
