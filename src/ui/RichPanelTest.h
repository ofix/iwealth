#ifndef RICH_PANEL_TEST_H
#define RICH_PANEL_TEST_H

#include <wx/dcbuffer.h>
#include <wx/wx.h>
#include <wxSVG/svg.h>

class RichPanelTest : public wxPanel {
   public:
    RichPanelTest(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize);
    virtual ~RichPanelTest();
    void OnPaint(wxPaintEvent& event);
    void OnBackground(wxEraseEvent& event);
    void TestLoadSvgFile(wxDC* pDC);

   private:
    DECLARE_EVENT_TABLE()
};

#endif