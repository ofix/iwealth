#include "ui/components/RichGrid.h"

#ifndef WX_PRECOMP
#include "wx/checkbox.h"
#include "wx/combobox.h"
#include "wx/dcclient.h"
#include "wx/intl.h"
#include "wx/listbox.h"
#include "wx/log.h"
#include "wx/math.h"
#include "wx/settings.h"
#include "wx/textctrl.h"
#include "wx/utils.h"
#include "wx/valtext.h"
#endif

#include <wx/settings.h>
#include "wx/dcbuffer.h"
#include "wx/hashset.h"
#include "wx/headerctrl.h"
#include "wx/renderer.h"
#include "wx/spinctrl.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"

#if wxUSE_CLIPBOARD
#include "wx/clipbrd.h"
#endif  // wxUSE_CLIPBOARD

#include "ui/RichApplication.h"
#include "ui/components/RichGridCellStringRenderer.h"
#include "ui/components/RichGridColumnHeaderProvider.h"
#include "ui/components/RichGridColumnHeaderRenderer.h"
#include "util/FileTool.h"
#include "util/Global.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"
#include "wx/generic/gridsel.h"
#include "wx/generic/private/grid.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

namespace {

// #define DEBUG_ATTR_CACHE
#ifdef DEBUG_ATTR_CACHE
static size_t gs_nAttrCacheHits = 0;
static size_t gs_nAttrCacheMisses = 0;
#endif

// this struct simply combines together the default header renderers
//
// as the renderers ctors are trivial, there is no problem with making them
// globals
struct DefaultHeaderRenderers {
    wxGridColumnHeaderRendererDefault colRenderer;
    wxGridRowHeaderRendererDefault rowRenderer;
    wxGridCornerHeaderRendererDefault cornerRenderer;
} gs_defaultHeaderRenderers;

}  // anonymous namespace

RichGrid::RichGrid() {
    Init();
}

RichGrid::RichGrid(wxWindow* parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
    : wxGrid(parent, id, pos, size, style, name) {
    // 自定义单元格渲染器，解决行选中无法独立设置每个单元格的文字颜色
    this->SetDefaultRenderer(new RichGridCellStringRenderer());
    // 自定义数据来源，解决直接排序的效率问题，避免字符串排序
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    m_pGridDataTable = new RichGridTable(RichGridTableDataType::Stock, pStorage);
    this->SetTable(m_pGridDataTable);
    // 自定义表格头渲染，解决排序没有指示器的问题
    this->GetTable()->SetAttrProvider(new RichGridColumnHeaderProvider());
    LoadColumnLabelImages();
    // 绑定鼠标滚轮事件
    // Bind(wxEVT_MOUSEWHEEL, &RichGrid::OnMouseWheel, this);
}

void RichGrid::SortMultiColumns() {
    m_pGridDataTable->SortMultiColumns();
}

bool RichGrid::SetSortColumn(int iCol) {
    return m_pGridDataTable->SetSortColumn(iCol);
}

bool RichGrid::SetFixedSortColumn(int iCol) {
    return m_pGridDataTable->SetFixedSortColumn(iCol);
}

// 加载本地排序图标
void RichGrid::LoadColumnLabelImages() {
    wxImage imageSortUp;
    wxImage imageSortDown;
    wxImage::AddHandler(new wxPNGHandler);
    if (!imageSortUp.LoadFile(FileTool::CurrentPath() + "assets" + DIRECTORY_SEPARATOR + "up.png", wxBITMAP_TYPE_PNG)) {
        std::cout << "load up png failed" << std::endl;
    }
    m_imgSortUp = wxBitmap(imageSortUp);

    if (!imageSortDown.LoadFile(FileTool::CurrentPath() + "assets" + DIRECTORY_SEPARATOR + "down.png",
                                wxBITMAP_TYPE_PNG)) {
        std::cout << "load down png failed" << std::endl;
    }
    m_imgSortDown = wxBitmap(imageSortDown);
}

void RichGrid::SetColumnLabelAlignment(int iCol, int hAlign, int vAlign) {
    m_colLabelAlignment.insert({iCol, {hAlign, vAlign}});
}

void RichGrid::GetColumnLabelAlignment(int iCol, int* hAlign, int* vAlign) {
    if (m_colLabelAlignment.find(iCol) != m_colLabelAlignment.end()) {
        std::pair<int, int> item = m_colLabelAlignment[iCol];
        *hAlign = item.first;
        *vAlign = item.second;
    } else {
        GetColLabelAlignment(hAlign, vAlign);
    }
}

void RichGrid::DrawColLabel(wxDC& dc, int col) {
    if (GetColWidth(col) <= 0 || m_colLabelHeight <= 0)
        return;

    int colLeft = GetColLeft(col);

    wxRect rect(colLeft, 0, GetColWidth(col), m_colLabelHeight);
    wxGridCellAttrProvider* const attrProvider = m_table ? m_table->GetAttrProvider() : NULL;
    const wxGridColumnHeaderRenderer& rend =
        attrProvider ? attrProvider->GetColumnHeaderRenderer(col)
                     : static_cast<wxGridColumnHeaderRenderer&>(gs_defaultHeaderRenderers.colRenderer);

    if (m_nativeColumnLabels) {
        wxRendererNative::Get().DrawHeaderButton(
            GetColLabelWindow(), dc, rect, 0,
            IsSortingBy(col) ? IsSortOrderAscending() ? wxHDR_SORT_ICON_UP : wxHDR_SORT_ICON_DOWN
                             : wxHDR_SORT_ICON_NONE);
        rect.Deflate(2);
    } else {
        if (m_cursorMode == WXGRID_CURSOR_MOVE_COL) {
            // clear the background:
            // when called from ProcessRowColLabelMouseEvent the background
            // is not cleared at this point
            // wxDCBrushChanger setBrush(dc, m_colLabelWin->GetBackgroundColour());
            // wxDCPenChanger setPen(dc, *wxTRANSPARENT_PEN);
            // dc.DrawRectangle(rect);
        }

        // draw a border if the column is not being drag-moved
        // (in that case it's omitted to have a 'pressed' appearance)
        if (m_cursorMode != WXGRID_CURSOR_MOVE_COL || col != m_dragMoveRowOrCol) {
            // rend.DrawBorder(*this, dc, rect);
        } else {
            // just highlight the current column
            dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)));
            dc.DrawRectangle(rect);
            rect.Deflate(GetBorder() == wxBORDER_NONE ? 2 : 1);
        }
    }

    int hAlign, vAlign;
    GetColumnLabelAlignment(col, &hAlign, &vAlign);
    const int orient = GetColLabelTextOrientation();

    // 渲染排序指示器，需要重新计算文字矩形和排序指示器图片占用的矩形
    wxString colLabelValue = GetColLabelValue(col);
    if ((hAlign == wxALIGN_RIGHT) && m_pGridDataTable->IsSortingColumn(col)) {
        wxRect rectLabelValue = rect;
        rectLabelValue.SetWidth(rect.GetWidth() - 18);
        rend.DrawLabel(*this, dc, colLabelValue, rectLabelValue, hAlign, vAlign, orient);
    } else {
        rend.DrawLabel(*this, dc, colLabelValue, rect, hAlign, vAlign, orient);
    }
    // 绘制图片
    if (m_pGridDataTable->IsSortingColumn(col)) {
        int order = m_pGridDataTable->GetColumnOrder(col);
        DrawSortImage(dc, colLabelValue, rect, hAlign, vAlign, order);
    }
}

void RichGrid::DrawSortImage(wxDC& dc, wxString& colLabelValue, wxRect& rect, int hAlign, int vAlign, int order) {
    wxRect rectSort = GetSortIndicatorRect(dc, colLabelValue, rect, hAlign, vAlign);
    wxPoint originDeviceOrigin = dc.GetDeviceOrigin();
    double originUserScaleX, originUserScaleY;
    dc.GetUserScale(&originUserScaleX, &originUserScaleY);
    // 绘制排序PNG图标
    dc.SetDeviceOrigin(rectSort.x, rectSort.y);
    if (order == 0) {
        // 一定要强制转换为double，否则传入无效参数0 给SetUserScale，将无法正确渲染图片！！
        dc.SetUserScale(static_cast<double>(rectSort.width) / m_imgSortUp.GetWidth(),
                        static_cast<double>(rectSort.height) / m_imgSortUp.GetHeight());
        dc.DrawBitmap(m_imgSortUp, 0, 0, true);
    } else {
        dc.SetUserScale(static_cast<double>(rectSort.width) / m_imgSortDown.GetWidth(),
                        static_cast<double>(rectSort.height) / m_imgSortDown.GetHeight());
        dc.DrawBitmap(m_imgSortDown, 0, 0, true);
    }
    // 恢复
    dc.SetDeviceOrigin(originDeviceOrigin.x, originDeviceOrigin.y);
    dc.SetUserScale(originUserScaleX, originUserScaleY);
}

wxRect RichGrid::GetSortIndicatorRect(wxDC& dc, wxString& colLabelValue, wxRect& rect, int hAlign, int vAlign) {
    int wLabelText, hLabelText;
    dc.GetTextExtent(colLabelValue, &wLabelText, &hLabelText);
    int wImg = 18;
    int hImg = 18;  // 图片18个像素大小
    wxRect rectSort;
    rectSort.SetWidth(wImg);
    rectSort.SetHeight(hImg);
    if (hAlign == wxALIGN_LEFT) {                           // 左对齐
        rectSort.SetLeft(rect.GetLeft() + wLabelText + 2);  // 图片和文字间隔两个像素
        if (vAlign == wxALIGN_CENTER) {
            rectSort.SetTop(rect.GetTop() + (rect.GetHeight() - hImg) / 2);
        } else if (vAlign == wxALIGN_TOP) {
            rectSort.SetTop(rect.GetTop());
        } else {
            rectSort.SetTop(rect.GetBottom() - hImg);
        }
    } else if (hAlign == wxALIGN_CENTER) {
        rectSort.SetLeft(rect.GetLeft() + rect.GetWidth() / 2 + wLabelText / 2 + 2);
        if (vAlign == wxALIGN_CENTER) {
            rectSort.SetTop(rect.GetTop() + (rect.GetHeight() - hImg) / 2);
        } else if (vAlign == wxALIGN_TOP) {
            rectSort.SetTop(rect.GetTop());
        } else {
            rectSort.SetTop(rect.GetBottom() - hImg);
        }
    } else if (hAlign == wxALIGN_RIGHT) {
        rectSort.SetLeft(rect.GetRight() - 18);
        if (vAlign == wxALIGN_CENTER) {
            rectSort.SetTop(rect.GetTop() + (rect.GetHeight() - hImg) / 2);
        } else if (vAlign == wxALIGN_TOP) {
            rectSort.SetTop(rect.GetTop());
        } else {
            rectSort.SetTop(rect.GetBottom() - hImg);
        }
    }
    return rectSort;
}
