#ifndef RICH_TAB_PAGE_H
#define RICH_TAB_PAGE_H
#include <wx/string.h>
#include <wx/wx.h>
#include <vector>

class RichTabPage : public wxPanel {
   public:
    RichTabPage(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxT(""));
    virtual ~RichTabPage();

    virtual wxString GetPageTitle(size_t nPage) const;  // 获取 tab 标签标题
    virtual bool SetPageTitle(size_t nPage, const wxString& title);
    virtual int ChangeSelection(size_t page);
    virtual int GetSelection() const;
    virtual int SetSelection(size_t page);  // Sets the selection to the given page, returning the previous selection.
    int FindPage(
        const wxWindow* page) const;  // Returns the index of the specified tab window or wxNOT_FOUND if not found.
    virtual bool DeleteAllPages();
    virtual bool DeletePage(size_t nPage);  // Deletes the specified page, and the associated window.
    virtual bool RemovePage(size_t nPage);  // Deletes the specified page, without deleting the associated window
    wxWindow* GetPage(size_t nPage) const;
    virtual int GetPageCount() const;
    virtual bool InsertPage(size_t index,
                            wxWindow* page,
                            const wxString& text,
                            bool select = false,
                            const wxString& svgPath = wxT(""));

    wxString GetSelectPageTitle();
    void SetActivePageTitle(wxString& title);

   private:
    int m_selected;
    std::vector<wxString> m_tabTitles;
    wxString m_activePageTitle;
    std::vector<wxWindow*> m_panels;
};

#endif