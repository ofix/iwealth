#include "ui/components/PanelStockQuote.h"
#include <wx/colour.h>
#include "spider/SpiderShareQuote.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"

//(*InternalHeaders(PanelStockQuote)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PanelStockQuote)
const long PanelStockQuote::ID_GRIDCTRL_QUOTA = wxNewId();
//*)

BEGIN_EVENT_TABLE(PanelStockQuote, wxPanel)
//(*EventTable(PanelStockQuote)
//*)
END_EVENT_TABLE()

PanelStockQuote::PanelStockQuote(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) {
    //(*Initialize(PanelStockQuote)
    Create(parent, id, pos, size, wxTAB_TRAVERSAL, _T("Panel_StockQuote"));
    m_gridCtrlQuote = new RichGrid(this, ID_GRIDCTRL_QUOTA, wxPoint(0, 0), size);
    wxFont quote_font(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("微软雅黑"),
                      wxFONTENCODING_DEFAULT);
    m_gridCtrlQuote->SetFont(quote_font);
    //*)

    /// wxPanel 自适应 wxFrame
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_gridCtrlQuote, 1, wxEXPAND | wxALL, 0);
    this->SetSizer(sizer);
    wxColor background_clr(50, 50, 50);
    m_gridCtrlQuote->SetDefaultCellBackgroundColour(background_clr);
    m_gridCtrlQuote->SetDefaultCellTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuote->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
    m_gridCtrlQuote->DisableDragRowSize();    // 禁止拖拽改变行高
    m_gridCtrlQuote->EnableEditing(false);    // 禁止编辑
    m_gridCtrlQuote->EnableGridLines(false);  // 不划线
    m_gridCtrlQuote->SetGridLineColour(background_clr);
    m_gridCtrlQuote->SetCellHighlightColour(wxColor(255, 255, 255, 0));
    m_gridCtrlQuote->SetCellHighlightPenWidth(0);
    m_gridCtrlQuote->SetLabelBackgroundColour(background_clr);
    m_gridCtrlQuote->SetLabelTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuote->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    m_gridCtrlQuote->SetSelectionBackground(wxColor(100, 100, 100));
    m_gridCtrlQuote->SetSelectionForeground(wxColor(219, 219, 3, 255));
    m_gridCtrlQuote->HideRowLabels();
    m_gridCtrlQuote->SetSortingColumn(3, false);
    /////////////////////////////////////////////////
    /// 插入行情列表表头

    std::vector<std::pair<wxString, int>> columnsStockQuote = {
        {CN("序号"), 56},    {CN("代码"), 120}, {CN("名称"), 140}, {CN("涨幅"), 100}, {CN("现价"), 100},
        {CN("昨收"), 100},   {CN("今开"), 100}, {CN("最高"), 100}, {CN("最低"), 100}, {CN("成交额"), 140},
        {CN("成交量"), 140}, {CN("换手"), 100}, {CN("量比"), 100}, {CN("行业"), 100}, {CN("省份"), 100},
    };
    m_gridCtrlQuote->CreateGrid(30, columnsStockQuote.size(), wxGrid::wxGridSelectRows);
    int icol = 0;
    for (std::pair<wxString, int>& item : columnsStockQuote) {
        m_gridCtrlQuote->SetColSize(icol, item.second);
        m_gridCtrlQuote->SetColLabelValue(icol, item.first);
        m_gridCtrlQuote->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
        icol++;
    }
    // StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Label"), wxPoint(232,232), wxDefaultSize, 0,
    // _T("ID_STATICTEXT1"));
}

// 加载市场当前行情并显示
void PanelStockQuote::LoadStockMarketQuote() {
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    if (pStorage->IsQuoteDataReady()) {
        std::vector<Share> shares = pStorage->GetMarketAllShares();
        long irow = 0;
        m_gridCtrlQuote->AppendRows(shares.size() - 30 + 1, false);
        wxColor clr_green(57, 227, 101);
        wxColor clr_red(255, 0, 0);
        for (Share& share : shares) {
            m_gridCtrlQuote->SetCellValue(irow, 0, std::to_string(irow + 1));                        // 序号
            m_gridCtrlQuote->SetCellValue(irow, 1, CN(share.code));                                  // 股票代码
            m_gridCtrlQuote->SetCellValue(irow, 2, CN(share.name));                                  // 股票名称
            m_gridCtrlQuote->SetCellValue(irow, 3, CN(convertDouble(share.change_rate) + '%'));      // 涨幅
            m_gridCtrlQuote->SetCellValue(irow, 4, CN(convertDouble(share.price_now)));              // 当前价
            m_gridCtrlQuote->SetCellValue(irow, 5, CN(convertDouble(share.price_yesterday_close)));  // 昨天收盘价
            m_gridCtrlQuote->SetCellValue(irow, 6, CN(convertDouble(share.price_open)));             // 开盘价
            m_gridCtrlQuote->SetCellValue(irow, 7, CN(convertDouble(share.price_max)));              // 最高价
            m_gridCtrlQuote->SetCellValue(irow, 8, CN(convertDouble(share.price_min)));              // 最低价
            m_gridCtrlQuote->SetCellValue(irow, 9, CN(std::to_string(share.amount)));                // 成交额
            m_gridCtrlQuote->SetCellValue(irow, 10, CN(std::to_string(share.volume)));               // 成交量
            m_gridCtrlQuote->SetCellValue(irow, 11, CN(convertDouble(share.turnover_rate) + '%'));   // 换手率
            m_gridCtrlQuote->SetCellValue(irow, 12, CN(convertDouble(share.qrr)));                   // 量比
            m_gridCtrlQuote->SetCellValue(irow, 13, CN(share.industry_name));                        // 行业
            m_gridCtrlQuote->SetCellValue(irow, 14, CN(share.province));                             // 省份
            if (share.change_rate > 0) {
                m_gridCtrlQuote->SetCellTextColour(irow, 3, clr_red);
                m_gridCtrlQuote->SetCellTextColour(irow, 4, clr_red);
            } else if (share.change_rate < 0) {
                m_gridCtrlQuote->SetCellTextColour(irow, 3, clr_green);
                m_gridCtrlQuote->SetCellTextColour(irow, 4, clr_green);
            }
            if (share.price_open > share.price_yesterday_close) {
                m_gridCtrlQuote->SetCellTextColour(irow, 6, clr_red);
            } else if (share.price_open < share.price_yesterday_close) {
                m_gridCtrlQuote->SetCellTextColour(irow, 6, clr_green);
            }
            if (share.price_max > share.price_yesterday_close) {
                m_gridCtrlQuote->SetCellTextColour(irow, 7, clr_red);
            } else if (share.price_max < share.price_yesterday_close) {
                m_gridCtrlQuote->SetCellTextColour(irow, 7, clr_green);
            }
            if (share.price_min > share.price_yesterday_close) {
                m_gridCtrlQuote->SetCellTextColour(irow, 8, clr_red);
            } else if (share.price_min < share.price_yesterday_close) {
                m_gridCtrlQuote->SetCellTextColour(irow, 8, clr_green);
            }
            irow++;
        }
    }
    m_gridCtrlQuote->Refresh();
}

PanelStockQuote::~PanelStockQuote() {
    //(*Destroy(PanelStockQuote)
    //*)
}
