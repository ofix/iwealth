#ifndef RICH_PANEL_H
#define RICH_PANEL_H

#include <wx/window.h>
#include "stock/StockDataStorage.h"

enum class PanelType { Quote, Kline };

class RichPanel : public wxWindow {
   public:
    RichPanel(PanelType type,
              StockDataStorage* pStorage,
              wxWindow* parent,
              wxWindowID id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL,
              const wxString& name = wxPanelNameStr);
    PanelType GetType();

   protected:
    PanelType m_type;
    StockDataStorage* m_pStorage;
};

#endif