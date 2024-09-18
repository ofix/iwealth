#ifndef RICH_FRAME_H
#define RICH_FRAME_H

#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/toplevel.h>

#define wxDEFAULT_TOP_MENU_BAR_COLOR wxColor(255, 0, 0)

enum class RichTopBarHitState {
    DEFAULT = 0,
    Menu = 1,
    BOX_MINIMIZE = 2,
    BOX_MAXIMIZE = 3,
    BOX_CLOSE = 4,
};

class RichFrame : public wxTopLevelWindow {
   public:
    RichFrame();
    RichFrame(wxWindow* parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDEFAULT_FRAME_STYLE,
              const wxString& name = wxFrameNameStr);
    bool Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);
    virtual ~RichFrame();
    virtual void SetMenuBar (wxMenuBar *menuBar);
    virtual wxMenuBar* GetMenuBar () const;
    virtual wxPoint GetClientAreaOrigin () const;
    bool ProcessCommand (int id);
    void DeleteMenuBar();
    void SetTopBarStyle(long style);
    long GetTopBarStyle() const;

    // event manipulation
    void OnClose(wxCloseEvent& event);
    void OnIconize(wxIconizeEvent& event);
    void OnMaximize(wxMaximizeEvent& event);

    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent&);
    void OnMouseMove(wxMouseEvent& event);
    void OnHitTest(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent&);
    void FinishDrag();
    bool CanMove();

    // draw related function
    void OnPaint(wxPaintEvent& event);
    void DrawBorder(wxDC* pDC);
    void SetTopMenuBarBkColor(wxColor& color);
    void DrawIcon(wxDC* pDC);
    void DrawMenuBar(wxDC* pDC, wxPoint& pt, wxSize& size);
    void DrawTitle(wxDC* pDC, wxPoint& pt, wxSize& size);

    void DrawMinimizeBox(wxDC* pDC);    // 最小化按钮
    void DrawMaximizeBox(wxDC* pDC);  // 最大化按钮
    void DrawCloseBox(wxDC* pDC);     // 关闭按钮

   private:
    // 顶部窗口按钮
    long m_topBarStyle;
    // resize
    bool m_bLeftMouseDown;
    bool m_dragging;
    wxPoint m_dragStartMouse;
    wxPoint m_dragStartWindow;
    //
    wxMenuBar* m_frameMenuBar;  // menubar
    wxImage* m_frameIcon;       // frame icon at the top left corner
    wxButton* m_btnMaximize;    // maximize button
    wxButton* m_btnMinimize;    // minimize button
    wxButton* m_btnClose;       // close button
    RichTopBarHitState m_hitState; // 顶部按钮选中状态
    wxDECLARE_DYNAMIC_CLASS(RichFrame);
};

#endif