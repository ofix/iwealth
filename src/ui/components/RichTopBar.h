#ifndef RICH_TOP_BAR_H
#define RICH_TOP_BAR_H

#include <wx/gdicmn.h>
#include <wx/wx.h>
#include <vector>
#include "ui/components/RichTopMenu.h"

enum class TopBarHitState {
    DEFAULT = 0,
    Menu = 1,
    BOX_MINIMIZE = 2,
    BOX_MAXIMIZE = 3,
    BOX_CLOSE = 4,
};

class RichTopBar : public wxWindow {
   public:
    RichTopBar(wxWindow* parent,                        // 父窗口
               wxWindowID id,                           // 窗口ID
               const wxPoint& pos = wxDefaultPosition,  // 窗口位置
               const wxSize& size = wxDefaultSize,      // 窗口大小
               long style = 0,
               const wxString& name = wxPanelNameStr);
    virtual ~RichTopBar();
    wxSize DoGetBestClientSize() const override;
    void AddMenu(RichTopMenu* menu);
    void SetTopBarStyle(long style);
    long GetTopBarStyle() const;

   protected:
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent&);
    void OnMouseMove(wxMouseEvent& event);
    void OnHitTest(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent&);
    void FinishDrag();
    bool CanMove();

    void OnPaint(wxPaintEvent& event);
    void DrawIcon(wxDC* pDC);     // logo
    void DrawTitle(wxDC* pDC);    // 窗口标题
    void DrawTopMenu(wxDC* pDC);  // 系统菜单

    void DrawMinimizeBox(wxDC* pDC);  // 最小化按钮
    void DrawMaximizeBox(wxDC* pDC);  // 最大化按钮
    void DrawCloseBox(wxDC* pDC);     // 关闭按钮

   private:
    // style
    long m_topBarStyle;
    bool m_bLeftMouseDown;
    bool m_dragging;
    wxPoint m_dragStartMouse;
    wxPoint m_dragStartWindow;

    std::vector<RichTopMenu*> m_menus;
    wxImage* m_icon;

    TopBarHitState m_hitState;
};

#endif