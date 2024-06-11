#ifndef PANELSTOCKQUOTA_H
#define PANELSTOCKQUOTA_H

//(*Headers(PanelStockQuota)
#include <wx/listctrl.h>
#include <wx/panel.h>
//*)

class PanelStockQuota: public wxPanel
{
	public:

		PanelStockQuota(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~PanelStockQuota();

		//(*Declarations(PanelStockQuota)
        wxListCtrl* m_listCtrlQuota;
		//*)

	protected:

		//(*Identifiers(PanelStockQuota)
        static const long ID_LISTCTRL_QUOTA;
		//*)

	private:

		//(*Handlers(PanelStockQuota)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
