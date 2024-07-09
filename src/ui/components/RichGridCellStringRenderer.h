#ifndef RICHGRIDCELLSTRINGRENDERER_H
#define RICHGRIDCELLSTRINGRENDERER_H
#include <wx/grid.h>
class RichGridCellStringRenderer : public wxGridCellStringRenderer {
   public:
    RichGridCellStringRenderer();

    void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rectCell, int row, int col, bool isSelected);
    void DrawBackground(wxGrid& grid,
                        wxGridCellAttr& attr,
                        wxDC& dc,
                        const wxRect& rectCell,
                        int row,
                        int col,
                        bool isSelected);

   protected:
    // set the text colours before drawing
    void SetTextColoursAndFont(const wxGrid& grid, const wxGridCellAttr& attr, wxDC& dc, bool isSelected);
};

#endif