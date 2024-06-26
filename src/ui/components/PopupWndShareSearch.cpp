#include "ui/components/PopupWndShareSearch.h"

//(*InternalHeaders(PopupWndShareSearch)
#include <wx/intl.h>
#include <wx/string.h>
#include "stock/StockDataStorage.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"

//*)

//(*IdInit(PopupWndShareSearch)
const long PopupWndShareSearch::ID_TEXTCTRL_KEYWORD = wxNewId();
const long PopupWndShareSearch::ID_LISTCTRL_SHARELIST = wxNewId();
//*)

BEGIN_EVENT_TABLE(PopupWndShareSearch, wxPopupWindow)
//(*EventTable(PopupWndShareSearch)
//*)
END_EVENT_TABLE()

PopupWndShareSearch::PopupWndShareSearch(wxWindow* parent, int flags) {
    //(*Initialize(PopupWndShareSearch)
    Create(parent, flags);
    //*)
    m_textctrl_keyword =
        new wxTextCtrl(this, ID_TEXTCTRL_KEYWORD, "", wxPoint(5, 5), wxSize(200, 32), wxTE_PROCESS_ENTER | wxHSCROLL);
    m_listctrl_sharelist =
        new wxListCtrl(this, ID_LISTCTRL_SHARELIST, wxPoint(5, 48), wxSize(200, 148), wxLC_REPORT | wxLC_SINGLE_SEL);
    m_listctrl_sharelist->InsertColumn(0, CN("代码"));
    m_listctrl_sharelist->InsertColumn(1, CN("名称"));
    m_listctrl_sharelist->InsertColumn(2, CN("市场"));
    m_textctrl_keyword->Bind(wxEVT_TEXT, &PopupWndShareSearch::OnSearchShare, this);
    m_textctrl_keyword->Bind(wxEVT_TEXT_ENTER, &PopupWndShareSearch::OnExitSearchShare, this);
}

void PopupWndShareSearch::ReLayout(const wxSize& size) {
    this->SetSize(size);
    wxSize size_textctrl;
    size_textctrl.SetWidth(size.GetWidth() - 10);
    size_textctrl.SetHeight(48);
    m_textctrl_keyword->SetSize(size_textctrl);

    wxSize size_listctrl;
    size_listctrl.SetWidth(size.GetHeight() - 10);
    size_listctrl.SetHeight(280);
    m_listctrl_sharelist->SetSize(size_listctrl);
}

void PopupWndShareSearch::SetShareList(const std::vector<Share*>& shares) {
    m_shares = shares;
    m_listctrl_sharelist->ClearAll();
    m_listctrl_sharelist->long row = 0;
    for (auto& pShare : m_shares) {
        m_listctrl_sharelist->InsertItem(row, _T(""));  // 必须先插入行数据，然后插入表格数据
        m_listctrl_sharelist->SetItem(row, 0, pShare->code);
        m_listctrl_sharelist->SetItem(row, 1, pShare->name);
        m_listctrl_sharelist->SetItem(row, 2, MARKET_NAME(pShare->market));
        row++;
    }
}

void PopupWndShareSearch::SetKeyword(const std::string& keyword) {
    m_keyword += keyword;
    m_textctrl_keyword->SetValue(m_keyword);
    m_textctrl_keyword->SetFocusFromKbd();
}

void PopupWndShareSearch::OnSearchShare(wxCommandEvent& event) {
    wxString input = m_textctrl_keyword->GetValue();
    std::string keyword = input.ToStdString();
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    std::vector<Share*> shares = pStorage->SearchShares(keyword);
    SetShareList(shares);
}

void PopupWndShareSearch::OnExitSearchShare(wxCommandEvent& event) {
    m_textctrl_keyword->Clear();
    this->Show(false);
}

PopupWndShareSearch::~PopupWndShareSearch() {
    //(*Destroy(PopupWndShareSearch)
    //*)
}
