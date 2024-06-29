#include "ui/components/RichGridColumnHeaderRenderer.h"
#include <iostream>

RichGridColumnHeaderRenderer::RichGridColumnHeaderRenderer() {
}

void RichGridColumnHeaderRenderer::SetFont(const wxFont& font) {
}

void RichGridColumnHeaderRenderer::DrawLabel(const wxGrid& grid,
                                             wxDC& dc,
                                             const wxString& value,
                                             const wxRect& rect,
                                             int horizAlign,
                                             int vertAlign,
                                             int textOrientation) const {
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc.SetFont(grid.GetLabelFont());

    // Draw text in a different colour and with a shadow when the control
    // is disabled.
    //
    // Note that the colours used here are consistent with wxGenericStaticText
    // rather than our own wxGridCellStringRenderer::SetTextColoursAndFont()
    // because this results in a better disabled appearance for the default
    // bold font used for the labels.
    wxColour colText;
    if (!grid.IsEnabled()) {
        colText = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
        dc.SetTextForeground(colText);

        wxRect rectShadow = rect;
        rectShadow.Offset(1, 1);
        grid.DrawTextRectangle(dc, value, rectShadow, horizAlign, vertAlign, textOrientation);

        colText = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
    } else {
        colText = grid.GetLabelTextColour();
    }

    dc.SetTextForeground(colText);

    grid.DrawTextRectangle(dc, value, rect, horizAlign, vertAlign, textOrientation);
}

void RichGridColumnHeaderRenderer::DrawBorder(const wxGrid& WXUNUSED(grid), wxDC& dc, wxRect& rect) const {
    wxPen pen(wxColor(255, 255, 255));
    dc.SetPen(pen);
    dc.DrawLine(rect.GetBottomRight(), rect.GetLeftBottom());
}