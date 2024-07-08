#include "ui/components/PanelKline.h"

const long PanelKline::ID_KLINECTRL = wxNewId();
const long PanelKline::ID_KLINEINFOCTRL = wxNewId();

PanelKline::PanelKline(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    Create(parent, id, pos, size, wxTAB_TRAVERSAL | wxWANTS_CHARS, _T("Panel_Kline"));
    m_pKlineCtrl = new RichKlineCtrl(this, ID_KLINECTRL, wxPoint(0, 0), size);
    m_pKlineInfoCtrl = new RichKlineInfoCtrl(m_pKlineCtrl, ID_KLINEINFOCTRL, wxPoint(0, 0), wxDefaultSize);
}

PanelKline::~PanelKline() {
}

void PanelKline::SetShareCode(const std::string& share_code) {
    m_share_code = share_code;
}