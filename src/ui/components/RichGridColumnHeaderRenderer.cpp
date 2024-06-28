#include "ui/components/RichGridColumnHeaderRenderer.h"

RichGridColumnHeaderRenderer::RichGridColumnHeaderRenderer() {
}

void RichGridColumnHeaderRenderer::SetFont(const wxFont& font) {
    m_font = font;
}

void RichGridColumnHeaderRenderer::DrawLabel(const wxGrid& WXUNUSED(grid),
                                             wxDC& dc,
                                             const wxString& value,
                                             const wxRect& rect,
                                             int horizAlign,
                                             int vertAlign,
                                             int WXUNUSED(textOrientation)) const {
    dc.SetFont(m_font);
    dc.SetTextForeground(wxColour(192, 192, 192));
    dc.DrawLabel(value, rect, horizAlign | vertAlign);
}

void RichGridColumnHeaderRenderer::DrawBorder(const wxGrid& WXUNUSED(grid), wxDC& dc, wxRect& rect) const {
}