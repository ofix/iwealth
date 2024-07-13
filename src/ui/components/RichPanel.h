#ifndef RICH_PANEL_H
#define RICH_PANEL_H

#include <wx/panel.h>

enum class PanelType { Quote, Kline };

class RichPanel : public wxPanel {
   public:
    RichPanel(PanelType type,
              wxWindow* parent,
              wxWindowID id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL,
              const wxString& name = wxPanelNameStr);
    PanelType GetType();

   protected:
    PanelType m_type;
};

#endif