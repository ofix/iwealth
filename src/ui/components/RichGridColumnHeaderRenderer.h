#ifndef RICHGRIDCOLUMNHEADERRENDER_H
#define RICHGRIDCOLUMNHEADERRENDER_H
#include <wx/dc.h>
#include <wx/dcsvg.h>
#include <wx/font.h>
#include <wx/grid.h>

class RichGridColumnHeaderRenderer : public wxGridColumnHeaderRenderer {
   public:
    RichGridColumnHeaderRenderer();
    virtual void DrawLabel(const wxGrid& WXUNUSED(grid),
                           wxDC& dc,
                           const wxString& value,
                           const wxRect& rect,
                           int horizAlign,
                           int vertAlign,
                           int WXUNUSED(textOrientation)) const;
    virtual void DrawBorder(const wxGrid& WXUNUSED(grid), wxDC& dc, wxRect& rect) const;

   protected:
    wxDECLARE_NO_COPY_CLASS(RichGridColumnHeaderRenderer);
};

#endif