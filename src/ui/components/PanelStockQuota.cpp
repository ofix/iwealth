#include "ui/components/PanelStockQuota.h"

//(*InternalHeaders(PanelStockQuota)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PanelStockQuota)
const long NewPanel::ID_LISTCTRL_QUOTA = wxNewId();
//*)

BEGIN_EVENT_TABLE(PanelStockQuota,wxPanel)
	//(*EventTable(PanelStockQuota)
	//*)
END_EVENT_TABLE()

PanelStockQuota::PanelStockQuota(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(PanelStockQuota)
	Create(parent, id, wxDefaultPosition, wxSize(639,566), wxTAB_TRAVERSAL, _T("id"));
    m_listCtrlQuota = new wxListCtrl(this, ID_LISTCTRL_QUOTA, wxPoint(0,0), wxSize(640,536), wxLC_LIST, wxDefaultValidator, _T("ID_LISTCTRL_QUOTA"));
	//*)

	// StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Label"), wxPoint(232,232), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
}

PanelStockQuota::~PanelStockQuota()
{
	//(*Destroy(PanelStockQuota)
	//*)
}

