#ifndef SHARESEARCHPANEL_H
#define SHARESEARCHPANEL_H

#include <wx/bmpbuttn.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include <vector>
#include "stock/Stock.h"
#include "ui/components/RichGrid.h"
#include "ui/components/ShareListGridTable.h"

#include <chrono>

class RichDialogShareSearch : public wxDialog {
   public:
    RichDialogShareSearch(wxWindow* parent,
                          wxWindowID id,
                          const wxString& title,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = wxDEFAULT_DIALOG_STYLE,
                          const wxString& name = wxDialogNameStr);
    virtual ~RichDialogShareSearch();
    void SetKeyword(const std::string& keyword);
    void ReLayout(const wxSize& size);
    // 搜索股票
    void OnSearchShare(wxCommandEvent& event);
    ShareListGridTable* CreateShareListGridTable();
    // 退出搜索
    void OnExitSearchShare(wxCommandEvent& event);
    // 监听上下方向键
    void OnKeyDown(wxKeyEvent& event);
    // void OnLeftClick(wxMouseEvent& event);
    void OnGridCellLeftClick(wxGridEvent& event);

    //(*Declarations(RichDialogShareSearch)
    static const long ID_TEXTCTRL_KEYWORD;
    static const long ID_RICHGRID_SHARELIST;
    static const long ID_STATICTEXT_TITLE;
    static const long ID_BITMAPBUTTON_CLOSE;
    //*)

   protected:
    //(*Identifiers(RichDialogShareSearch)
    //*)
    // wxListCtrl* m_gridShareLIst;
    RichGrid* m_gridShareList;
    wxTextCtrl* m_textCtrlKeyword;
    wxStaticText* m_statictext_title;
    wxBitmapButton* m_bitmapbutton_close;

    std::chrono::steady_clock::time_point m_oldTimeKeywordInput;
    std::chrono::steady_clock::time_point m_nowTimeKeywordInput;

   private:
    //(*Handlers(RichDialogShareSearch)
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
