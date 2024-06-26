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
    m_textctrl_keyword =
        new wxTextCtrl(this, ID_TEXTCTRL_KEYWORD, "", wxPoint(5, 5), wxSize(200, 32), wxTE_PROCESS_ENTER | wxHSCROLL);
    m_listctrl_sharelist =
        new wxListCtrl(this, ID_LISTCTRL_SHARELIST, wxPoint(5, 48), wxSize(200, 148), wxLC_REPORT | wxLC_SINGLE_SEL);
    m_listctrl_sharelist->InsertColumn(0, CN("代码"));
    m_listctrl_sharelist->InsertColumn(1, CN("名称"));
    m_listctrl_sharelist->InsertColumn(2, CN("市场"));
    m_textctrl_keyword->Bind(wxEVT_TEXT, &DialogShareSearch::OnSearchShare, this);
    m_textctrl_keyword->Bind(wxEVT_TEXT_ENTER, &DialogShareSearch::OnExitSearchShare, this);
}

void DialogShareSearch::ReLayout(const wxSize& size) {
    this->SetSize(size);
    wxSize size_textctrl;
    size_textctrl.SetWidth(size.GetWidth() - 10);
    size_textctrl.SetHeight(32);
    m_textctrl_keyword->SetSize(size_textctrl);

    wxSize size_listctrl;
    size_listctrl.SetWidth(size.GetHeight() - 10);
    size_listctrl.SetHeight(280);
    m_listctrl_sharelist->SetSize(size_listctrl);
}

void DialogShareSearch::SetShareList(const std::vector<Share*>& shares) {
    m_shares = shares;
    m_listctrl_sharelist->DeleteAllItems();
    long row = 0;
    for (auto& pShare : m_shares) {
        m_listctrl_sharelist->InsertItem(row, _T(""));  // 必须先插入行数据，然后插入表格数据
        m_listctrl_sharelist->SetItem(row, 0, CN(pShare->code));
        m_listctrl_sharelist->SetItem(row, 1, CN(pShare->name));
        m_listctrl_sharelist->SetItem(row, 2, CN(MARKET_NAME(pShare->market)));
        row++;
    }
}

void DialogShareSearch::SetKeyword(const std::string& keyword) {
    m_keyword += keyword;
    m_textctrl_keyword->SetValue(m_keyword);
    m_textctrl_keyword->SetFocusFromKbd();
    m_textctrl_keyword->SetFocus();
}

void DialogShareSearch::OnSearchShare(wxCommandEvent& event) {
    wxString input = m_textctrl_keyword->GetValue();
    std::string keyword = input.ToStdString();
    std::cout << "search: " << keyword << std::endl;
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    std::vector<Share*> shares = pStorage->SearchShares(keyword);
    SetShareList(shares);
}

void DialogShareSearch::OnExitSearchShare(wxCommandEvent& event) {
    m_textctrl_keyword->Clear();
    this->Show(false);
}

DialogShareSearch::~DialogShareSearch() {
    //(*Destroy(DialogShareSearch)
    //*)
}
