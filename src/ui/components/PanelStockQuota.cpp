#include "ui/components/PanelStockQuota.h"
#include <wx/colour.h>
#include "spider/SpiderShareListHexun.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"

//(*InternalHeaders(PanelStockQuota)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PanelStockQuota)
const long PanelStockQuota::ID_GRIDCTRL_QUOTA = wxNewId();
//*)

BEGIN_EVENT_TABLE(PanelStockQuota, wxPanel)
//(*EventTable(PanelStockQuota)
//*)
END_EVENT_TABLE()

PanelStockQuota::PanelStockQuota(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    //(*Initialize(PanelStockQuota)
    Create(parent, id, pos, size, wxTAB_TRAVERSAL, _T("Panel_StockQuota"));
    m_gridCtrlQuota = new wxGrid(this, ID_GRIDCTRL_QUOTA, wxPoint(0, 0), size);
    wxFont quote_font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("微软雅黑"),
                      wxFONTENCODING_DEFAULT);
    m_gridCtrlQuota->SetFont(quote_font);
    //*)

    /// wxPanel 自适应 wxFrame
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_gridCtrlQuota, 1, wxEXPAND | wxALL, 0);
    this->SetSizer(sizer);
    wxColor background_clr(50, 50, 50);
    m_gridCtrlQuota->SetDefaultCellBackgroundColour(background_clr);
    m_gridCtrlQuota->SetDefaultCellTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuota->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
    m_gridCtrlQuota->DisableDragRowSize();    // 禁止拖拽改变行高
    m_gridCtrlQuota->EnableEditing(false);    // 禁止编辑
    m_gridCtrlQuota->EnableGridLines(false);  // 不划线
    m_gridCtrlQuota->SetGridLineColour(background_clr);
    m_gridCtrlQuota->SetCellHighlightColour(wxColor(255, 255, 255, 0));
    m_gridCtrlQuota->SetCellHighlightPenWidth(0);
    m_gridCtrlQuota->SetLabelBackgroundColour(background_clr);
    m_gridCtrlQuota->SetLabelTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuota->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    m_gridCtrlQuota->SetSelectionBackground(wxColor(100, 100, 100));
    m_gridCtrlQuota->SetSelectionForeground(wxColor(219, 219, 3, 255));
    m_gridCtrlQuota->HideRowLabels();
    m_gridCtrlQuota->SetSortingColumn(3, false);

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

        /// 插入行情列表表头
        std::vector<std::pair<wxString, int>> columnsStockQuote = {
            {CN("序号"), 56},  {CN("代码"), 120}, {CN("名称"), 140},
            {CN("涨幅"), 100}, {CN("现价"), 100}, {CN("换手"), 100},
        };
        m_gridCtrlQuota->CreateGrid(shares.size() + 1, 6, wxGrid::wxGridSelectRows);
        int icol = 0;
        for (std::pair<wxString, int>& item : columnsStockQuote) {
            m_gridCtrlQuota->SetColSize(icol, item.second);
            m_gridCtrlQuota->SetColLabelValue(icol, item.first);
            m_gridCtrlQuota->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
            icol++;
        }
        /// 插入表格内容
        long irow = 0;
        for (Share& share : shares) {
            m_gridCtrlQuota->SetCellValue(irow, 0, std::to_string(irow + 1));
            m_gridCtrlQuota->SetCellValue(irow, 1, CN(share.code));
            m_gridCtrlQuota->SetCellValue(irow, 2, CN(share.name));
            m_gridCtrlQuota->SetCellValue(irow, 3, CN(convertDouble(share.change_rate) + '%'));
            m_gridCtrlQuota->SetCellValue(irow, 4, CN(convertDouble(share.price_now)));
            m_gridCtrlQuota->SetCellValue(irow, 5, CN(convertDouble(share.turnover_rate) + '%'));
            if (share.change_rate > 0) {
                m_gridCtrlQuota->SetCellTextColour(irow, 3, wxColour(255, 0, 0));
                m_gridCtrlQuota->SetCellTextColour(irow, 4, wxColour(255, 0, 0));
            } else {
                m_gridCtrlQuota->SetCellTextColour(irow, 3, wxColour(57, 227, 101));
                m_gridCtrlQuota->SetCellTextColour(irow, 4, wxColour(57, 227, 101));
            }
            irow++;
        }
    }
}

PanelStockQuota::~PanelStockQuota() {
    //(*Destroy(PanelStockQuota)
    //*)
}
