#ifndef RICH_MINI_PANEL_H
#define RICH_MINI_PANEL_H

#include <wx/panel.h>
#include <wx/wx.h>

class RichMiniPanel : public wxPanel {
   public:
    RichMiniPanel(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxTAB_TRAVERSAL,
                  const wxString& name = wxPanelNameStr);
    virtual ~RichMiniPanel();
    void SetTitle(wxString& title);
    wxString GetTitle();

   private:
    wxString m_title;
};

#endif