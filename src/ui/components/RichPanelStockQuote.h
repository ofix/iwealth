#ifndef PANELSTOCKQUOTA_H
#define PANELSTOCKQUOTA_H

#include "ui/components/RichGrid.h"
#include "ui/components/RichPanel.h"

class RichPanelStockQuote : public RichPanel {
   public:
    RichPanelStockQuote(PanelType type,
                        wxWindow* parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize);
    virtual ~RichPanelStockQuote();
    RichGrid* GetGridCtrl();
    RichGrid* m_gridCtrlQuote;
    void LoadStockMarketQuote();  // 加载股票市场行情
    void OnGridQuoteHeaderClick(wxGridEvent& event);
    void OnGridQuoteHeaderDblClick(wxGridEvent& event);

   protected:
    static const long ID_GRIDCTRL_QUOTA;

   private:
    DECLARE_EVENT_TABLE()
};

#endif
