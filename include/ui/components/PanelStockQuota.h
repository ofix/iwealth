#ifndef PANELSTOCKQUOTA_H
#define PANELSTOCKQUOTA_H

//(*Headers(PanelStockQuota)
#include <wx/panel.h>
#include "ui/components/RichGrid.h"

//*)

class PanelStockQuota : public wxPanel {
   public:
    PanelStockQuota(wxWindow* parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize);
    virtual ~PanelStockQuota();

    //(*Declarations(PanelStockQuota)
    RichGrid* m_gridCtrlQuota;
    //*)

    void LoadStockMarketQuote();  // 加载股票市场行情

   protected:
    //(*Identifiers(PanelStockQuota)
    static const long ID_GRIDCTRL_QUOTA;
    //*)

   private:
    //(*Handlers(PanelStockQuota)
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
