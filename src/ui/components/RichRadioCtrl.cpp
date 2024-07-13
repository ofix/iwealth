#include "ui/components/RichRadioCtrl.h"
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <iostream>
#include "ui/RichHelper.h"
#include "ui/components/RichRadioEvent.h"

BEGIN_EVENT_TABLE(RichRadioCtrl, wxControl)
EVT_ERASE_BACKGROUND(RichRadioCtrl::OnBackground)
END_EVENT_TABLE()

RichRadioCtrl::RichRadioCtrl(const std::vector<std::string>& options,  // 需要显示的选项
                             int current,                              // 初始化第几项选中
                             wxWindow* parent,                         // 父窗口
                             wxWindowID id,                            // 窗口ID
                             const wxPoint& pos,                       // 窗口位置
                             const wxSize& size,                       // 窗口大小
                             long style                                // 窗口样式
                             )
    : m_active(current) {
    Create(parent, id, pos, size, style | wxNO_BORDER);

    m_clrBackground = wxColor(0, 0, 0);
    m_clrTextDefault = wxColor(180, 180, 180);
    m_clrTextActive = wxColor(233, 223, 0);
    for (size_t i = 0; i < options.size(); i++) {
        RichStaticText* option_ctrl = new RichStaticText(this, wxNewId(), CN(options[i]));
        option_ctrl->SetBackgroundColour(m_clrBackground);
        option_ctrl->SetForegroundColour(m_clrTextDefault);
        option_ctrl->Bind(wxEVT_LEFT_DOWN, &RichRadioCtrl::OnClick, this);
        m_optionCtrls.emplace_back(option_ctrl);
        m_optionMap.insert({options[i], i});
    }
    m_optionCtrls[m_active]->SetForegroundColour(m_clrTextActive);
    m_activeLabel = m_optionCtrls[m_active]->GetLabel();
    Layout();
}

bool RichRadioCtrl::Layout() {
    if (m_optionCtrls.size() < 1) {
        return false;
    }
    int x = GetPosition().x;
    int y = GetPosition().y;
    wxClientDC dc(this);
    for (size_t i = 0; i < m_optionCtrls.size(); i++) {
        m_optionCtrls[i]->Move(x, y, 0);
        wxSize text_size = dc.GetTextExtent(m_optionCtrls[i]->GetLabel());
        x += text_size.GetWidth() + 12;
    }
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
        m_optionCtrls[m_active]->SetForegroundColour(m_clrTextDefault);
        m_active = n;
        m_activeLabel = m_optionCtrls[m_active]->GetLabel();
        m_optionCtrls[m_active]->SetForegroundColour(m_clrTextActive);
    }
}

void RichRadioCtrl::OnClick(wxMouseEvent& event) {
    if (event.LeftDown()) {
        RichStaticText* pControl = static_cast<RichStaticText*>(event.GetEventObject());
        wxString label = pControl->GetLabel();
        if (label != m_activeLabel) {  // 需要切换
            m_optionCtrls[m_active]->SetForegroundColour(m_clrTextDefault);
            m_optionCtrls[m_active]->Refresh();
            m_active = GetSelectionIndex(label.utf8_string());  // 不能使用StdString()，会产生乱码，导致无法正确识别
            m_activeLabel = m_optionCtrls[m_active]->GetLabel();
            m_optionCtrls[m_active]->SetForegroundColour(m_clrTextActive);
            m_optionCtrls[m_active]->Refresh();
            // 发送自定义事件
            RichRadioEvent radio_event = RichRadioEvent(wxEVT_RICH_RADIO, GetId());
            QueueEvent(radio_event.Clone());
        }
    }
}

void RichRadioCtrl::OnBackground(wxEraseEvent& event) {
    wxDC* pDC = event.GetDC();
    pDC->SetBrush(wxBrush(wxColour(0, 0, 0)));
}