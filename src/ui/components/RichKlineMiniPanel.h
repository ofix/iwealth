#ifndef RICH_KLINE_MINI_PANEL_H
#define RICH_KLINE_MINI_PANEL_H
#include "RichMiniPanel.h"

class RichKlineMiniPanel : public RichMiniPanel {
   public:
    RichKlineMiniPanel(wxWindow* parent,
                       wxWindowID id = wxID_ANY,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxTAB_TRAVERSAL,
                       const wxString& name = wxPanelNameStr);
    virtual ~RichKlineMiniPanel();

   private:
};

#endif