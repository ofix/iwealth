#include "ui/components/PanelStockQuota.h"

//(*InternalHeaders(PanelStockQuota)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PanelStockQuota)
//*)

BEGIN_EVENT_TABLE(PanelStockQuota,wxPanel)
	//(*EventTable(PanelStockQuota)
	//*)
END_EVENT_TABLE()

PanelStockQuota::PanelStockQuota(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(PanelStockQuota)
	Create(parent, id, wxDefaultPosition, wxSize(639,566), wxTAB_TRAVERSAL, _T("id"));
	//*)
}

PanelStockQuota::~PanelStockQuota()
{
	//(*Destroy(PanelStockQuota)
	//*)
}

