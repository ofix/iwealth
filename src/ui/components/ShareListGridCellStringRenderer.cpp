#include "ui/components/ShareListGridCellStringRenderer.h"
#include <wx/dc.h>
#include <wx/settings.h>
#include <iostream>

ShareListGridCellStringRenderer::ShareListGridCellStringRenderer() : wxGridCellStringRenderer() {
}

void ShareListGridCellStringRenderer::Draw(wxGrid& grid,
                                           wxGridCellAttr& attr,
                                           wxDC& dc,
                                           const wxRect& rectCell,
                                           int row,
                                           int col,
                                           bool isSelected) {
    wxRect rect = rectCell;
    rect.Inflate(-1);

    DrawBackground(grid, attr, dc, rectCell, row, col, isSelected);

    if (attr.CanOverflow()) {
        int hAlign, vAlign;
        attr.GetAlignment(&hAlign, &vAlign);

        int overflowCols = 0;
        int cols = grid.GetNumberCols();
        int best_width = GetBestSize(grid, attr, dc, row, col).GetWidth();
        int cell_rows, cell_cols;
        attr.GetSize(&cell_rows, &cell_cols);  // shouldn't get here if <= 0
        if ((best_width > rectCell.width) && (col < cols) && grid.GetTable()) {
            int i, c_cols, c_rows;
            for (i = col + cell_cols; i < cols; i++) {
                bool is_empty = true;
                for (int j = row; j < row + cell_rows; j++) {
                    // check w/ anchor cell for multicell block
                    grid.GetCellSize(j, i, &c_rows, &c_cols);
                    if (c_rows > 0)
                        c_rows = 0;
                    if (!grid.GetTable()->IsEmptyCell(j + c_rows, i)) {
                        is_empty = false;
                        break;
                    }
                }

                if (is_empty) {
                    rect.width += grid.GetColSize(i);
                } else {
                    i--;
                    break;
                }

                if (rect.width >= best_width)
                    break;
            }

            overflowCols = i - col - cell_cols + 1;
            if (overflowCols >= cols)
                overflowCols = cols - 1;
        }

        if (overflowCols > 0)  // redraw overflow cells w/ proper hilight
        {
            hAlign = wxALIGN_LEFT;  // if oveflowed then it's left aligned
            wxRect clip = rect;
            clip.x += rectCell.width;
            // draw each overflow cell individually
            int col_end = col + cell_cols + overflowCols;
            if (col_end >= grid.GetNumberCols())
                col_end = grid.GetNumberCols() - 1;
            for (int i = col + cell_cols; i <= col_end; i++) {
                // redraw the cell to update the background
                wxGridCellCoords coords(row, i);
                grid.DrawCell(dc, coords);

                clip.width = grid.GetColSize(i) - 1;
                wxDCClipper clipper(dc, clip);

                SetTextColoursAndFont(grid, attr, dc, grid.IsInSelection(row, i));

                grid.DrawTextRectangle(dc, grid.GetCellValue(row, col), rect, hAlign, vAlign);
                clip.x += grid.GetColSize(i) - 1;
            }

            rect = rectCell;
            rect.Inflate(-1);
            rect.width++;
        }
    }

    // now we only have to draw the text
    SetTextColoursAndFont(grid, attr, dc, isSelected);

    grid.DrawTextRectangle(dc, grid.GetCellValue(row, col), rect, attr);
}

void ShareListGridCellStringRenderer::DrawBackground(wxGrid& grid,
                                                     wxGridCellAttr& attr,
                                                     wxDC& dc,
                                                     const wxRect& rect,
                                                     int row,
                                                     int col,
                                                     bool isSelected) {
    dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
    wxColour clr;
    if (grid.IsThisEnabled()) {
        if (isSelected) {
            clr = grid.GetSelectionBackground();  // 不区分是否失去输入焦点，只显示高亮背景色
            // if (grid.HasFocus())
            //     clr = grid.GetSelectionBackground();
            // else
            //     clr = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
        } else {
            clr = attr.GetBackgroundColour();
        }
    } else  // grey out fields if the grid is disabled
    {
        clr = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
    }

    dc.SetBrush(clr);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void ShareListGridCellStringRenderer::SetTextColoursAndFont(const wxGrid& grid,
                                                            const wxGridCellAttr& attr,
                                                            wxDC& dc,
                                                            bool isSelected) {
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    // TODO some special colours for attr.IsReadOnly() case?

    // different coloured text when the grid is disabled
    if (grid.IsThisEnabled()) {
        if (isSelected) {
            wxColour clr;
            if (grid.HasFocus())
                clr = grid.GetSelectionBackground();
            else
                clr = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
            // 修改选中文字颜色
            dc.SetTextForeground(grid.GetSelectionForeground());
            // dc.SetTextForeground(attr.GetTextColour());
        } else {
            dc.SetTextBackground(attr.GetBackgroundColour());
            // 只重写了这一行，注意virtual和非virtual函数的调用关系，Draw virtual函数必须完整复制父类的才能正常工作
            dc.SetTextForeground(attr.GetTextColour());
        }
    } else {
        dc.SetTextBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    }

    dc.SetFont(attr.GetFont());
}