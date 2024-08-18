#ifndef RICH_DIALOG_H
#define RICH_DIALOG_H

#include <wx/control.h>
#include <wx/datetime.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <string>
#include "stock/Stock.h"

class RichDialog : public wxDialog {
   public:
    RichDialog(wxWindow* parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize);
    virtual ~RichDialog();
    void OnPaint(wxPaintEvent& event);
    virtual void OnDrawClient(wxDC* pDC);
    void OnLeftMouseDown(wxMouseEvent& event);
    void OnLeftMouseMove(wxMouseEvent& event);
    void OnLeftMouseUp(wxMouseEvent& event);
    // 设置自定义窗口标题栏背景色
    void SetTitleBackgroundColor(wxColor color);
    // 设置自定义窗口标题栏前景色
    void SetTitleForegroundColor(wxColor color);
    // 绘制自定义标题栏
    void DrawTitle(wxDC* pDC);
    // 绘制自定义关闭按钮
    void DrawCloseButton(wxDC* pDC);
    // 鼠标光标是否在标题栏
    bool IsInTitleBar(wxPoint& ptCursor);
    // 鼠标光标是否在关闭按钮
    bool IsInCloseButton(wxPoint& ptCursor);

   protected:
    bool m_inMove;
    wxPoint m_ptPrev;
    wxPoint m_ptNow;
    wxRect m_rectCloseBtn;
    wxColor m_titleForegroundColor;
    wxColor m_titleBackgroundColor;
    DECLARE_EVENT_TABLE()
};

#endif  // RICHKLINEINFOCTRL_H
