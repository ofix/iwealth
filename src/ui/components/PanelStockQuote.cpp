#include "ui/components/PanelStockQuote.h"
#include <wx/colour.h>
#include "spider/SpiderShareQuote.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"
#include "ui/components/RichGridCellStringRenderer.h"
#include "ui/components/RichGridColumnHeaderProvider.h"
#include "ui/components/RichGridColumnHeaderRenderer.h"

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
    Create(parent, id, pos, size, wxTAB_TRAVERSAL | wxWANTS_CHARS, _T("Panel_StockQuote"));
    m_gridCtrlQuote = new RichGrid(this, ID_GRIDCTRL_QUOTA, wxPoint(0, 0), size);

    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    // 自定义数据来源，解决直接排序的效率问题，避免字符串排序
    m_gridCtrlQuote->SetTable(new RichGridTable(RichGridTableDataType::Stock, pStorage));
    // 自定义表格头渲染，解决排序没有指示器的问题
    m_gridCtrlQuote->GetTable()->SetAttrProvider(new RichGridColumnHeaderProvider());

    m_gridCtrlQuote->SetFont(RichApplication::GetDefaultFont(14));

    /// wxPanel 自适应 wxFrame，通过 wxSizer 即可实现
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_gridCtrlQuote, 1, wxEXPAND | wxALL, 0);
    this->SetSizer(sizer);
    wxColor background_clr(0, 0, 0);
    m_gridCtrlQuote->SetDefaultCellBackgroundColour(background_clr);
    m_gridCtrlQuote->SetDefaultCellTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuote->SetDefaultCellAlignment(wxALIGN_RIGHT, wxALIGN_CENTER);
    m_gridCtrlQuote->DisableDragRowSize();    // 禁止拖拽改变行高
    m_gridCtrlQuote->EnableEditing(false);    // 禁止编辑
    m_gridCtrlQuote->EnableGridLines(false);  // 不划线
    m_gridCtrlQuote->SetGridLineColour(background_clr);
    m_gridCtrlQuote->SetCellHighlightColour(wxColor(255, 255, 255, 0));
    m_gridCtrlQuote->SetCellHighlightPenWidth(0);
    m_gridCtrlQuote->SetLabelBackgroundColour(background_clr);
    m_gridCtrlQuote->SetSelectionBackground(wxColor(100, 100, 100));
    // m_gridCtrlQuote->SetRowLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTRE);
    // m_gridCtrlQuote->SetSelectionForeground(wxColor(219, 219, 3, 255));
    m_gridCtrlQuote->SetDefaultRowSize(25);
    m_gridCtrlQuote->HideRowLabels();
    m_gridCtrlQuote->SetSortingColumn(3, false);
    m_gridCtrlQuote->SetDefaultCellFont(RichApplication::GetDefaultFont());
    m_gridCtrlQuote->SetSelectionMode(wxGrid::wxGridSelectRows);
    m_gridCtrlQuote->SetDefaultRenderer(new RichGridCellStringRenderer());
    m_gridCtrlQuote->SetLabelFont(RichApplication::GetDefaultFont(14));
    m_gridCtrlQuote->SetLabelTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuote->SetColLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
    m_gridCtrlQuote->SetScrollRate(0, static_cast<int>(36 * 8.5));
    m_gridCtrlQuote->Bind(wxEVT_GRID_LABEL_LEFT_CLICK, &PanelStockQuote::OnGridQuoteHeaderClick, this);
    m_gridCtrlQuote->Bind(wxEVT_GRID_LABEL_LEFT_DCLICK, &PanelStockQuote::OnGridQuoteHeaderDblClick, this);
    /////////////////////////////////////////////////
    /// 插入行情列表表头

    std::vector<std::pair<wxString, int>> columnsStockQuote = {
        {CN("序号"), 56},  {CN("代码"), 120},   {CN("名称"), 140},   {CN("涨幅"), 100},
        {CN("现价"), 100}, {CN("昨收"), 100},   {CN("今开"), 100},   {CN("最高"), 100},
        {CN("最低"), 100}, {CN("成交额"), 140}, {CN("成交量"), 140}, {CN("换手"), 100},
        {CN("振幅"), 100}, {CN("量比"), 100},   {CN("行业"), 140},   {CN("省份"), 100},
    };
    // m_gridCtrlQuote->CreateGrid(30, columnsStockQuote.size(), wxGrid::wxGridSelectRows);
    int icol = 0;
    for (std::pair<wxString, int>& item : columnsStockQuote) {
        m_gridCtrlQuote->SetColSize(icol, item.second);
        // m_gridCtrlQuote->SetColLabelValue(icol, item.first);
        icol++;
    }
    wxGridCellAttr* pAlignLeftAttr = new wxGridCellAttr(
        wxColour(192, 192, 192), wxColor(0, 0, 0), RichApplication::GetDefaultFont(14), wxALIGN_LEFT, wxALIGN_CENTRE);
    wxGridCellAttr* pAlignCenterAttr = new wxGridCellAttr(
        wxColour(192, 192, 192), wxColor(0, 0, 0), RichApplication::GetDefaultFont(14), wxALIGN_CENTER, wxALIGN_CENTRE);
    m_gridCtrlQuote->SetColAttr(0, pAlignCenterAttr);
    m_gridCtrlQuote->SetColAttr(1, pAlignLeftAttr);
    m_gridCtrlQuote->SetColAttr(2, pAlignLeftAttr);
    m_gridCtrlQuote->SetColAttr(13, pAlignCenterAttr);
    m_gridCtrlQuote->SetColAttr(14, pAlignLeftAttr);
    m_gridCtrlQuote->SetColAttr(15, pAlignLeftAttr);
    m_gridCtrlQuote->SetColumnLabelAlignment(0, wxALIGN_CENTER, wxALIGN_CENTER);
    m_gridCtrlQuote->SetColumnLabelAlignment(1, wxALIGN_LEFT, wxALIGN_CENTER);
    m_gridCtrlQuote->SetColumnLabelAlignment(2, wxALIGN_LEFT, wxALIGN_CENTER);
    m_gridCtrlQuote->SetColumnLabelAlignment(13, wxALIGN_CENTER, wxALIGN_CENTER);
    m_gridCtrlQuote->SetColumnLabelAlignment(14, wxALIGN_LEFT, wxALIGN_CENTER);
    m_gridCtrlQuote->SetColumnLabelAlignment(15, wxALIGN_LEFT, wxALIGN_CENTER);
}

RichGrid* PanelStockQuote::GetGridCtrl() {
    return m_gridCtrlQuote;
}

void PanelStockQuote::OnGridQuoteHeaderClick(wxGridEvent& event) {
    int col = event.GetCol();
    m_gridCtrlQuote->SetSortColumn(col);
    m_gridCtrlQuote->SortMultiColumns();
    std::cout << "left click" << std::endl;
    LoadStockMarketQuote();
    m_gridCtrlQuote->Refresh();
}

void PanelStockQuote::OnGridQuoteHeaderDblClick(wxGridEvent& event) {
    int col = event.GetCol();
    m_gridCtrlQuote->SetFixedSortColumn(col);
    m_gridCtrlQuote->SortMultiColumns();
    std::cout << "left double click" << std::endl;
    LoadStockMarketQuote();
    m_gridCtrlQuote->Refresh();
}

// 加载市场当前行情并显示
void PanelStockQuote::LoadStockMarketQuote() {
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    if (pStorage->IsQuoteDataReady()) {
        std::vector<Share> shares = pStorage->GetMarketAllShares();
        long irow = 0;
        // m_gridCtrlQuote->AppendRows(shares.size() - 30 + 1, false);
        wxColor clr_green(57, 227, 101);
        wxColor clr_red(255, 0, 0);
        for (Share& share : shares) {
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
            if (share.price_amplitude > 0) {
                m_gridCtrlQuote->SetCellTextColour(irow, 12, clr_red);
            } else {
                m_gridCtrlQuote->SetCellTextColour(irow, 12, clr_green);
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
