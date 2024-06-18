#ifndef PANELSTOCKQUOTA_H
#define PANELSTOCKQUOTA_H

//(*Headers(PanelStockQuote)
#include <wx/panel.h>
#include "ui/components/RichGrid.h"

//*)

class PanelStockQuote : public wxPanel {
   public:
    PanelStockQuote(wxWindow* parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize);
    virtual ~PanelStockQuote();

    //(*Declarations(PanelStockQuote)
    RichGrid* m_gridCtrlQuota;
    //*)

    void LoadStockMarketQuote();  // 加载股票市场行情

   protected:
    //(*Identifiers(PanelStockQuote)
    static const long ID_GRIDCTRL_QUOTA;
    //*)

   private:
    //(*Handlers(PanelStockQuote)
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
