#ifndef RICHGRID_H
#define RICHGRID_H

#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/grid.h>

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
};

#endif