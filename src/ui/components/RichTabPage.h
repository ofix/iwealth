#ifndef RICH_TAB_PAGE_H
#define RICH_TAB_PAGE_H
#include <wx/wx.h>
#include <wx/string.h>

class RichTabPage : public wxPanel {
   public:
    RichTabPage(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxT(""));
    virtual ~RichTabPage();
    virtual int GetRowCount() const;
    virtual int GetSelection() const;
    virtual int SetSelection(size_t page);
    virtual int ChangeSelection(size_t page);
    virtual bool InsertPage(size_t index, wxWindow* page, const wxString& text, bool select = false);

    wxString GetSelectPageTitle();
    void SetActivePageTitle(wxString& title);

   private:
    int m_selected;
    std::vector<wxString> m_tabTitles;
    wxString m_activePageTitle;
};

#endif