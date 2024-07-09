#ifndef RICHGRID_H
#define RICHGRID_H

#include <wx/bitmap.h>
#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/grid.h>
#include <wx/image.h>
#include <unordered_map>
#include "ui/components/RichGridTable.h"

struct RichColLabelAlignment {
    int iCol;
    int horiz = wxALIGN_CENTER;
    int vert = wxALIGN_CENTER;
};

class RichGrid : public wxGrid {
   public:
    RichGrid();
    RichGrid(wxWindow* parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxWANTS_CHARS,
             const wxString& name = wxASCII_STR(wxGridNameStr));
    virtual void DrawColLabel(wxDC& dc, int col) wxOVERRIDE;
    void SetColumnLabelAlignment(int iCol, int hAlign, int vAlign);
    void GetColumnLabelAlignment(int iCol, int* hAlign, int* vAlign);
    void OnKeyDown(wxKeyEvent& event);
    void MoveSelectedListItem(int dir);

    bool SetSortColumn(int iCol);
    bool SetFixedSortColumn(int iCol);
    // 多列分组排序
    bool SortMultiColumns();

    void LoadColumnLabelImages();
    wxRect GetSortIndicatorRect(wxDC& dc, wxString& colLabelValue, wxRect& rect, int hAlign, int vAlign);
    void DrawSortImage(wxDC& dc, wxString& colLabelValue, wxRect& rect, int hAlign, int vAlign, int order);

   protected:
    wxBitmap m_imgSortUp;
    wxBitmap m_imgSortDown;
    std::unordered_map<int, std::pair<int, int>> m_colLabelAlignment;
};

#endif