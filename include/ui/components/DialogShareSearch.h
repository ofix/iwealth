#ifndef SHARESEARCHPANEL_H
#define SHARESEARCHPANEL_H

//(*Headers(DialogShareSearch)
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
//*)
#include <vector>
#include "stock/Stock.h"

class DialogShareSearch : public wxDialog {
   public:
    DialogShareSearch(wxWindow* parent,
                      wxWindowID id,
                      const wxString& title,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxDEFAULT_DIALOG_STYLE,
                      const wxString& name = wxDialogNameStr);
    virtual ~DialogShareSearch();
    void SetKeyword(const std::string& keyword);
    void SetShareList(const std::vector<Share*>& shares);
    void ReLayout(const wxSize& size);
    // 搜索股票
    void OnSearchShare(wxCommandEvent& event);
    // 退出搜索
    void OnExitSearchShare(wxCommandEvent& event);

    //(*Declarations(DialogShareSearch)
    static const long ID_TEXTCTRL_KEYWORD;
    static const long ID_LISTCTRL_SHARELIST;
    //*)

   protected:
    //(*Identifiers(DialogShareSearch)
    //*)
    wxListCtrl* m_listctrl_sharelist;
    wxTextCtrl* m_textctrl_keyword;

   private:
    //(*Handlers(DialogShareSearch)
    //*)
    std::vector<Share*> m_shares;

    DECLARE_EVENT_TABLE()
};

#endif
