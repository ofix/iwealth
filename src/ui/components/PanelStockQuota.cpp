#include "ui/components/PanelStockQuota.h"
#include "spider/SpiderShareListHexun.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"

//(*InternalHeaders(PanelStockQuota)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PanelStockQuota)
const long PanelStockQuota::ID_LISTCTRL_QUOTA = wxNewId();
//*)

BEGIN_EVENT_TABLE(PanelStockQuota, wxPanel)
//(*EventTable(PanelStockQuota)
//*)
END_EVENT_TABLE()

PanelStockQuota::PanelStockQuota(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    //(*Initialize(PanelStockQuota)
    Create(parent, id, pos, size, wxTAB_TRAVERSAL, _T("Panel_StockQuota"));
    m_listCtrlQuota =
        new wxListCtrl(this, ID_LISTCTRL_QUOTA, wxPoint(0, 0), size, wxLC_REPORT | wxLC_VRULES | wxVSCROLL | wxHSCROLL,
                       wxDefaultValidator, _T("ID_LISTCTRL_STOCK_QUOTE"));
    wxFont ListCtrlQuoteFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("微软雅黑"),
                             wxFONTENCODING_DEFAULT);
    m_listCtrlQuota->SetFont(ListCtrlQuoteFont);
    //*)

    /// 插入行情列表表头
    std::vector<std::pair<wxString, int>> columnsStockQuote = {
        {CN("代码"), 50}, {CN("名称"), 50}, {CN("涨幅"), 50}, {CN("现价"), 50}, {CN("换手"), 50}};

    size_t colIdx = 0;
    for (std::pair<wxString, int>& item : columnsStockQuote) {
        wxListItem col;
        col.SetId(colIdx);
        col.SetText(item.first);
        col.SetWidth(item.second);
        m_listCtrlQuota->InsertColumn(colIdx++, col);
    }

    /////////////////////////////////////////////////
    // StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Label"), wxPoint(232,232), wxDefaultSize, 0,
    // _T("ID_STATICTEXT1"));
}

// 加载市场当前行情并显示
void PanelStockQuota::LoadStockMarketQuote() {
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    if (pStorage) {
        SpiderShareListHexun* spiderKline = new SpiderShareListHexun(pStorage);
        spiderKline->Crawl();  // 当前线程同步爬取市场行情数据
        std::vector<Share> shares = pStorage->GetMarketAllShares();
        long row = 0;
        for (Share& share : shares) {
            m_listCtrlQuota->InsertItem(row, _T(""));  // 必须先插入行数据，然后插入表格数据
            m_listCtrlQuota->SetItem(row, 0, CN(share.code));
            m_listCtrlQuota->SetItem(row, 1, CN(share.name));
            m_listCtrlQuota->SetItem(row, 2, CN(std::to_string(share.change_rate * 100) + '%'));
            m_listCtrlQuota->SetItem(row, 3, CN(std::to_string(share.price_now)));
            m_listCtrlQuota->SetItem(row, 4, CN(std::to_string(share.turnover_rate * 100) + '%'));
            row++;
        }
    }
}

PanelStockQuota::~PanelStockQuota() {
    //(*Destroy(PanelStockQuota)
    //*)
}
