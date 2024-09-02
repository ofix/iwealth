#ifndef RICH_TOP_BAR_H
#define RICH_TOP_BAR_H

#include <wx/gdicmn.h>
#include <wx/wx.h>
#include <vector>

class RichTopMenu;
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

   protected:
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent&);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent&);
    void FinishDrag();

    void OnPaint(wxPaintEvent& event);
    void DrawIcon(wxDC* pDC);         // logo
    void DrawTitle(wxDC* pDC);        // 窗口标题
    void DrawTopMenu(wxDC* pDC);      // 系统菜单
    void DrawMimizeBox(wxDC* pDC);    // 最小化按钮
    void DrawMaximizeBox(wxDC* pDC);  // 最大化按钮
    void DrawCloseBox(wxDC* pDC);     // 关闭按钮

   private:
    wxPoint m_dragStartMouse;
    wxPoint m_dragStartWindow;
    std::vector<RichTopMenu*> m_menus;
    wxImage* m_icon;
    bool m_dragging;
};

#endif