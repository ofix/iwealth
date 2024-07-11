#include "ui/components/PanelKline.h"
#include "ui/RichApplication.h"

const long PanelKline::ID_KLINE_CTRL = wxNewId();
const long PanelKline::ID_KLINE_INFO_CTRL = wxNewId();
const long PanelKline::ID_RAIDO_CTRL = wxNewId();

PanelKline::PanelKline(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    Create(parent, id, pos, size, wxTAB_TRAVERSAL | wxWANTS_CHARS, _T("Panel_Kline"));
    std::vector<std::string> options = {"日线", "周线", "月线", "季线", "年线"};
    m_pRadioCtrl = new RichRadioCtrl(options, 0, this, ID_RAIDO_CTRL, wxPoint(2, 2), wxSize(600, 28));
    wxSize klineSize = size;
    klineSize.DecBy(wxSize(0, 30));  // 这里不能使用DecTo,会导致RichKlineCtrl控件宽度为0
    // m_pRadioCtrl->Show();
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    m_pKlineCtrl = new RichKlineCtrl(pStorage, this, ID_KLINE_CTRL, wxPoint(2, 30), klineSize);
    m_pKlineInfoCtrl = new RichKlineInfoCtrl(m_pKlineCtrl, ID_KLINE_INFO_CTRL, pos, wxSize(200, 400));
    m_pKlineInfoCtrl->Show(false);
}

PanelKline::~PanelKline() {
}

void PanelKline::SetShareCode(const std::string& share_code) {
    m_share_code = share_code;
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    if (!pStorage->IsLocalFileShareKlinesExist(share_code)) {
        pStorage->FetchKlineSync(share_code, KlineType::Day);
        pStorage->SaveShareKlines(share_code, KlineType::Day);
    }
    m_pKlineCtrl->LoadKlines(share_code);
    m_pKlineCtrl->Show();
}