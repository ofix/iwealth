#ifndef RICH_FRAME_H
#define RICH_FRAME_H

#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/toplevel.h>

#define wxDEFAULT_TOP_MENU_BAR_COLOR wxColor(255, 0, 0)

class RichFrame : public wxTopLevelWindow {
   public:
    RichFrame();
    RichFrame(wxWindow* parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDEFAULT_FRAME_STYLE,
              const wxString& name = wxFrameNameStr);
    virtual ~RichFrame();
    bool Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    // event manipulation
    void OnClose(wxCloseEvent& event);
    void OnIconize(wxIconizeEvent& event);
    void OnMaximize(wxMaximizeEvent& event);

    // draw related function
    void OnPaint(wxPaintEvent& event);
    void DrawBorder(wxDC* pDC);
    void SetTopMenuBarBkColor(wxColor& color);
    void DrawIcon(wxDC* pDC);
    void DrawMenuBar(wxDC* pDC, wxPoint& pt, wxSize& size);
    void DrawMaximizeButton(wxDC* pDC, wxPoint& pt, wxSize& size);
    void DrawMimimizeButton(wxDC* pDC, wxPoint& pt, wxSize& size);
    void DrawCloseButton(wxDC* pDC, wxPoint& pt, wxSize& size);
    void DrawTitle(wxDC* pDC, wxPoint& pt, wxSize& size);

   private:
    // resize
    bool m_bLeftMouseDown;
    bool m_dragging;
    wxPoint m_dragStartMouse;
    wxPoint m_dragStartWindow;
    //
    wxMenu* m_frameMenu;      // top menu bar
    wxImage* m_frameIcon;     // frame icon at the top left corner
    wxButton* m_btnMaximize;  // maximize button
    wxButton* m_btnMinimize;  // minimize button
    wxButton* m_btnClose;     // close button
    wxDECLARE_DYNAMIC_CLASS(RichFrame);
};

#endif