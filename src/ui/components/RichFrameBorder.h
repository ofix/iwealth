#ifndef RICH_RISIZE_WND_H
#define RICH_RISIZE_WND_H

#include <wx/colour.h>
#include <wx/window.h>

class RichFrameBorder : public wxWindow {
   public:
    RichFrameBorder(wxWindow* parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    wxColor& bg_clr = wxColor(0, 0, 0));
    void OnPaint(wxPaintEvent& event);

   private:
    wxColor m_bgClr;
};

#endif