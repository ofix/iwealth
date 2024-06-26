#ifndef SHARESEARCHPANEL_H
#define SHARESEARCHPANEL_H

//(*Headers(PopupWndShareSearch)
#include <wx/listctrl.h>
#include <wx/popupwin.h>
#include <wx/textctrl.h>
//*)
#include <vector>
#include "stock/Stock.h"

class PopupWndShareSearch : public wxPopupWindow {
   public:
    PopupWndShareSearch(wxWindow* parent, int flags = wxBORDER_NONE);
    virtual ~PopupWndShareSearch();
    void SetKeyword(const std::string& keyword);
    void SetShareList(const std::vector<Share*>& shares);
    void ReLayout(const wxSize& size);
    // 搜索股票
    void OnSearchShare(wxCommandEvent& event);
    // 退出搜索
    void OnExitSearchShare(wxCommandEvent& event);

    //(*Declarations(PopupWndShareSearch)
    static const long ID_TEXTCTRL_KEYWORD;
    static const long ID_LISTCTRL_SHARELIST;
    //*)

   protected:
    //(*Identifiers(PopupWndShareSearch)
    //*)
    wxListCtrl* m_listctrl_sharelist;
    wxTextCtrl* m_textctrl_keyword;

   private:
    //(*Handlers(PopupWndShareSearch)
    //*)
    std::vector<Share*> m_shares;
    std::string m_keyword;

    DECLARE_EVENT_TABLE()
};

#endif
