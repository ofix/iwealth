#ifndef RICHRADIO_H
#define RICHRADIO_H

#include <wx/control.h>
#include <wx/stattext.h>
#include <unordered_map>
#include <vector>

class RichRadioCtrl : public wxControl {
   public:
    RichRadioCtrl(const std::vector<std::string>& options,  // 需要显示的选项
                  int current,                              // 初始化第几项选中
                  wxWindow* parent,                         // 父窗口
                  wxWindowID id,                            // 窗口ID
                  const wxPoint& pos = wxDefaultPosition,   // 窗口位置
                  const wxSize& size = wxDefaultSize,       // 窗口大小
                  long style = 0);

    void SetSelection(int n);
    wxString GetActiveLabel();
    int GetActiveIndex();

   protected:
    void OnClick(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    virtual bool Layout();

    std::vector<wxRect> m_radioRects;
    std::vector<wxString> m_radioLabels;
    int m_active;
    int m_focus;
    wxColor m_clrBackground;
    wxColor m_clrTextDefault;
    wxColor m_clrTextActive;
};

#endif