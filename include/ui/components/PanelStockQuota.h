#ifndef PANELSTOCKQUOTA_H
#define PANELSTOCKQUOTA_H

//(*Headers(PanelStockQuota)
#include <wx/panel.h>
//*)

class PanelStockQuota: public wxPanel
{
	public:

		PanelStockQuota(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~PanelStockQuota();

		//(*Declarations(PanelStockQuota)
		//*)

	protected:

		//(*Identifiers(PanelStockQuota)
		//*)

	private:

		//(*Handlers(PanelStockQuota)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
