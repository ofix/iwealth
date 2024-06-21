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

#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"
#include "wx/generic/gridsel.h"
#include "wx/generic/private/grid.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

namespace {

//#define DEBUG_ATTR_CACHE
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
    GetColLabelAlignment(&hAlign, &vAlign);
    const int orient = GetColLabelTextOrientation();

    rend.DrawLabel(*this, dc, GetColLabelValue(col), rect, hAlign, vAlign, orient);
}
