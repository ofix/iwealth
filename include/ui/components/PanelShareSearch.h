#ifndef SHARESEARCHPANEL_H
#define SHARESEARCHPANEL_H

//(*Headers(PanelShareSearch)
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
//*)
#include <vector>
#include "stock/Stock.h"

class PanelShareSearch : public wxPanel {
   public:
    PanelShareSearch(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~PanelShareSearch();
    void SetKeyword(const std::string& keyword);
    void SetShareList(const std::vector<Share*>& shares);

    //(*Declarations(PanelShareSearch)
    static const long ID_TEXTCTRL_KEYWORD;
    static const long ID_LISTCTRL_SHARELIST;
    //*)

   protected:
    //(*Identifiers(PanelShareSearch)
    //*)
    wxListCtrl* m_listctrl_sharelist;
    wxTextCtrl* m_textctrl_keyword;

   private:
    //(*Handlers(PanelShareSearch)
    //*)
    std::vector<Share*> m_shares;
    std::string m_keyword;

    DECLARE_EVENT_TABLE()
};

#endif
