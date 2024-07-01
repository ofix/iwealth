#ifndef SHARESEARCHPANEL_H
#define SHARESEARCHPANEL_H

//(*Headers(DialogShareSearch)
#include <wx/bmpbuttn.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

//*)
#include <vector>
#include "stock/Stock.h"

#include <chrono>

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
    static const long ID_STATICTEXT_TITLE;
    static const long ID_BITMAPBUTTON_CLOSE;
    //*)

   protected:
    //(*Identifiers(DialogShareSearch)
    //*)
    wxListCtrl* m_listctrl_sharelist;
    wxTextCtrl* m_textctrl_keyword;
    wxStaticText* m_statictext_title;
    wxBitmapButton* m_bitmapbutton_close;

    std::chrono::steady_clock::time_point m_oldTimeKeywordInput;
    std::chrono::steady_clock::time_point m_nowTimeKeywordInput;

   private:
    //(*Handlers(DialogShareSearch)
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
