#include "ui/components/PanelShareSearch.h"

//(*InternalHeaders(PanelShareSearch)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PanelShareSearch)
const long PanelShareSearch::ID_TEXTCTRL_KEYWORD = wxNewId();
const long PanelShareSearch::ID_LISTCTRL_SHARELIST = wxNewId();
//*)

BEGIN_EVENT_TABLE(PanelShareSearch, wxPanel)
//(*EventTable(PanelShareSearch)
//*)
END_EVENT_TABLE()

PanelShareSearch::PanelShareSearch(wxWindow* parent, wxWindowID id) {
    //(*Initialize(PanelShareSearch)
    Create(parent, id, wxDefaultPosition, wxSize(200, 200), wxTAB_TRAVERSAL, _T("PanelShareSearch"));
    //*)
    m_textctrl_keyword = new wxTextCtrl(this, ID_TEXTCTRL_KEYWORD, "", wxPoint(0, 0), wxSize(200, 48));
    m_listctrl_sharelist = new wxListCtrl(this, ID_LISTCTRL_SHARELIST, wxPoint(0, 0), wxSize(200, 48));
    m_listctrl_sharelist->InsertColumn(0, "代码");
    m_listctrl_sharelist->InsertColumn(1, "名称");
    m_listctrl_sharelist->InsertColumn(2, "市场");
}

void PanelShareSearch::SetShareList(const std::vector<Share*>& shares) {
    m_shares = shares;
    size_t nShares = m_shares.size();
    m_listctrl_sharelist->ClearAll();
    long row = 0;
    for (auto& pShare : m_shares) {
        m_listctrl_sharelist->SetItem(row, 0, pShare->code);
        m_listctrl_sharelist->SetItem(row, 1, pShare->name);
        m_listctrl_sharelist->SetItem(row, 2, MARKET_NAME(pShare->market));
        row++;
    }
}

void PanelShareSearch::SetKeyword(const std::string& keyword) {
    m_keyword = keyword;
}

PanelShareSearch::~PanelShareSearch() {
    //(*Destroy(PanelShareSearch)
    //*)
}
