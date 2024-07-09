#include "ui/components/RichRadioCtrl.h"
#include <wx/gdicmn.h>
#include "ui/components/RichRadioEvent.h"

RichRadioCtrl::RichRadioCtrl() {
}

RichRadioCtrl::RichRadioCtrl(const std::vector<std::string>& options,  // 需要显示的选项
                             int current,                              // 初始化第几项选中
                             wxWindow* parent,                         // 父窗口
                             wxWindowID id,                            // 窗口ID
                             const wxPoint& pos,                       // 窗口位置
                             const wxSize& size,                       // 窗口大小
                             long style                                // 窗口样式
                             )
    : m_active(current) {
    Create(parent, id, pos, size, style);
    m_activeLabel = wxString(options[current]);
    m_clrDefault = wxColor(180, 180, 180);
    m_clrActive = wxColor(233, 223, 0);
    for (size_t i = 0; i < options.size(); i++) {
        wxStaticText* option_ctrl = new wxStaticText(this, wxNewId(), wxString(options[i]));
        option_ctrl->Bind(wxEVT_LEFT_DOWN, &RichRadioCtrl::OnClick, this);
        m_optionCtrls.emplace_back(option_ctrl);
        m_optionMap.insert({options[i], i});
    }
}

bool RichRadioCtrl::Layout() {
    if (m_optionCtrls.size() < 1) {
        return false;
    }
    // int x = GetPosition().x;
    // wxSize size = GetSize();
    // int w = size.GetWidth() / m_optionCtrls.size();
    return true;
}

int RichRadioCtrl::GetSelectionIndex(const std::string& label) {
    if (m_optionMap.find(label) != m_optionMap.end()) {
        return m_optionMap[label];
    }
    return -1;
}

void RichRadioCtrl::SetSelection(int n) {
    if ((size_t)n > m_optionCtrls.size()) {
        return;
    }
    if (m_active != n) {
        m_optionCtrls[m_active]->SetForegroundColour(m_clrDefault);
        m_active = n;
        m_activeLabel = m_optionCtrls[m_active]->GetLabel();
        m_optionCtrls[m_active]->SetForegroundColour(m_clrActive);
    }
}

void RichRadioCtrl::OnClick(wxMouseEvent& event) {
    if (event.LeftDown()) {
        wxStaticText* pControl = static_cast<wxStaticText*>(event.GetEventObject());
        wxString label = pControl->GetLabel();
        if (label != m_activeLabel) {  // 需要切换
            m_optionCtrls[m_active]->SetForegroundColour(m_clrDefault);
            m_active = GetSelectionIndex(label.ToStdString());
            m_activeLabel = m_optionCtrls[m_active]->GetLabel();
            m_optionCtrls[m_active]->SetForegroundColour(m_clrActive);
            // 发送自定义事件
            RichRadioEvent radio_event = RichRadioEvent(wxEVT_RICH_RADIO, GetId());
            QueueEvent(radio_event.Clone());
        }
    }
}