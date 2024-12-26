#ifndef RICH_SHARE_LIST_MINI_PANEL_H
#define RICH_SHARE_LIST_MINI_PANEL_H
#include "RichMiniPanel.h"

class RichShareListMiniPanel : public RichMiniPanel {
   public:
    RichShareListMiniPanel(wxWindow* parent,
                       wxWindowID id = wxID_ANY,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxTAB_TRAVERSAL,
                       const wxString& name = wxT(""));
    virtual ~RichShareListMiniPanel();

   private:
};

#endif