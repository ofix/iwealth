#ifndef RICHRADIO_H
#define RICHRADIO_H

#include <wx/control.h>
#include <unordered_map>
#include <vector>
#include "ui/components/RichStaticText.h"

class RichRadioCtrl : public wxControl {
   public:
    RichRadioCtrl(const std::vector<std::string>& options,  // 需要显示的选项
                  int current,                              // 初始化第几项选中
                  wxWindow* parent,                         // 父窗口
                  wxWindowID id,                            // 窗口ID
                  const wxPoint& pos = wxDefaultPosition,   // 窗口位置
                  const wxSize& size = wxDefaultSize,       // 窗口大小
                  long style = 0);
    void OnClick(wxMouseEvent& event);
    void OnBackground(wxEraseEvent& event);
    virtual void SetSelection(int n);
    // 必须重写此函数，否则会始终获取 wxPanel 的键盘输入焦点，导致RichPanelKline 始终无法获取键盘按下事件
    virtual bool AcceptsFocusFromKeyboard() const {
        return false;
    }

   protected:
    virtual bool Layout();
    int GetSelectionIndex(const std::string& label);

    std::vector<RichStaticText*> m_optionCtrls;
    int m_active;
    std::unordered_map<std::string, int> m_optionMap;
    wxString m_activeLabel;
    wxColor m_clrBackground;
    wxColor m_clrTextDefault;
    wxColor m_clrTextActive;
    DECLARE_EVENT_TABLE();
};

#endif