#include "ui/RichPanelStockQuote.h"
#include <wx/colour.h>
#include <wx/intl.h>
#include <wx/string.h>
#include "spider/SpiderShareQuote.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"
#include "ui/components/RichGridCellStringRenderer.h"
#include "ui/components/RichGridColumnHeaderProvider.h"
#include "ui/components/RichGridColumnHeaderRenderer.h"

const long RichPanelStockQuote::ID_GRIDCTRL_QUOTA = wxNewId();

BEGIN_EVENT_TABLE(RichPanelStockQuote, wxPanel)

END_EVENT_TABLE()

RichPanelStockQuote::RichPanelStockQuote(PanelType type,
                                         wxWindow* parent,
                                         wxWindowID id,
                                         const wxPoint& pos,
                                         const wxSize& size)
    : RichPanel(type, parent, id, pos, size, wxTAB_TRAVERSAL | wxWANTS_CHARS, _T("PanelQuote")) {
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
    // 默认设置
    m_gridCtrlQuote->SetDefaultCellBackgroundColour(background_clr);
    m_gridCtrlQuote->SetDefaultCellTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuote->SetDefaultCellAlignment(wxALIGN_RIGHT, wxALIGN_CENTER);
    m_gridCtrlQuote->SetDefaultRowSize(25);
    m_gridCtrlQuote->SetDefaultCellFont(RichApplication::GetDefaultFont());
    // 自定义单元格渲染器，解决行选中无法独立设置每个单元格的文字颜色
    m_gridCtrlQuote->SetDefaultRenderer(new RichGridCellStringRenderer());

    m_gridCtrlQuote->DisableDragRowSize();                               // 禁止拖拽改变行高
    m_gridCtrlQuote->EnableEditing(false);                               // 禁止编辑
    m_gridCtrlQuote->EnableGridLines(false);                             // 不显示网格线
    m_gridCtrlQuote->HideRowLabels();                                    // 隐藏默认行标签
    m_gridCtrlQuote->SetCellHighlightPenWidth(0);                        // 单元格不高亮
    m_gridCtrlQuote->SetGridLineColour(background_clr);                  // 网格线颜色
    m_gridCtrlQuote->SetCellHighlightColour(wxColor(255, 255, 255, 0));  // 高亮颜色透明
    m_gridCtrlQuote->SetSortingColumn(3, false);                         // 默认按第3列倒序排列
    m_gridCtrlQuote->SetSelectionMode(wxGrid::wxGridSelectRows);         // 只支持整行选择
    m_gridCtrlQuote->SetSelectionBackground(wxColor(31, 31, 31));        // 设置行选择背景色

    // 标签行列设置
    m_gridCtrlQuote->SetLabelBackgroundColour(background_clr);
    m_gridCtrlQuote->SetLabelFont(RichApplication::GetDefaultFont(14));
    m_gridCtrlQuote->SetLabelTextColour(wxColour(192, 192, 192));
    m_gridCtrlQuote->SetColLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
    // m_gridCtrlQuote->SetRowLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTRE);
    // 鼠标滚轮设置
    m_gridCtrlQuote->SetScrollRate(0, static_cast<int>(36 * 8.5));
    m_gridCtrlQuote->Bind(wxEVT_GRID_LABEL_LEFT_CLICK, &RichPanelStockQuote::OnGridQuoteHeaderClick, this);
    m_gridCtrlQuote->Bind(wxEVT_GRID_LABEL_LEFT_DCLICK, &RichPanelStockQuote::OnGridQuoteHeaderDblClick, this);
    /////////////////////////////////////////////////
    /// 插入行情列表表头

    std::vector<int> columnsStockQuote = {
        56, 120, 140, 100, 100, 100, 100, 100, 100, 140, 140, 100, 100, 100, 140, 100,
    };
    int icol = 0;
    for (auto& colWidth : columnsStockQuote) {
        m_gridCtrlQuote->SetColSize(icol, colWidth);
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

RichGrid* RichPanelStockQuote::GetGridCtrl() {
    return m_gridCtrlQuote;
}

void RichPanelStockQuote::OnGridQuoteHeaderClick(wxGridEvent& event) {
    int col = event.GetCol();
    m_gridCtrlQuote->SetSortColumn(col);
    m_gridCtrlQuote->SortMultiColumns();
    LoadStockMarketQuote();
    m_gridCtrlQuote->MakeCellVisible(0, 0);
}

void RichPanelStockQuote::OnGridQuoteHeaderDblClick(wxGridEvent& event) {
    int col = event.GetCol();
    m_gridCtrlQuote->SetFixedSortColumn(col);
    m_gridCtrlQuote->SortMultiColumns();
    LoadStockMarketQuote();
    m_gridCtrlQuote->MakeCellVisible(0, 0);
}

// 加载市场当前行情并显示
void RichPanelStockQuote::LoadStockMarketQuote() {
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    if (pStorage->IsQuoteDataReady()) {
        std::vector<Share> shares = pStorage->GetMarketAllShares();
        long irow = 0;
        m_gridCtrlQuote->GetTable()->InsertRows(0, shares.size());  // 必须增加这一行，通知视图表格模型数据已变更
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
        m_gridCtrlQuote->SelectRow(0);  // 默认选中第一行
    }
    // 解决启动过程中数据下载完成，RichGridTable::GetNumRows() 依然返回0，导致界面不刷新
    m_gridCtrlQuote->ForceRefresh();  // 使用Refresh()不会有效果
}

RichPanelStockQuote::~RichPanelStockQuote() {
    //(*Destroy(RichPanelStockQuote)
    //*)
}
