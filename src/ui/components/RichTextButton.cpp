#include "RichTextButton.h"
#include <iostream>

wxBEGIN_EVENT_TABLE(RichTextButton, wxButton)
    EVT_ENTER_WINDOW(RichTextButton::OnMouseEnter)
    EVT_LEAVE_WINDOW(RichTextButton::OnMouseLeave)
wxEND_EVENT_TABLE()

RichTextButton::RichTextButton(wxColor& normalColor,
                               wxColor& focusColor,
                               wxWindow* parent,
                               wxWindowID id,
                               const wxString& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style)
    : wxButton(parent, id, label, pos, size, style), m_normalColor(normalColor), m_focusColor(focusColor) {
    // Bind(wxEVT_ENTER_WINDOW, &RichTextButton::OnMouseEnter, this);
    // Bind(wxEVT_LEAVE_WINDOW, &RichTextButton::OnMouseLeave, this);
    SetForegroundColour(m_normalColor);  // 初始化默认 normalColor
}

RichTextButton::~RichTextButton() {
}

wxColor RichTextButton::GetFocusColor() const {
    return m_focusColor;
}

wxColor RichTextButton::GetNormalColor() const {
    return m_normalColor;
}

void RichTextButton::Active() {
    std::cout<<" active "<<std::endl;
    SetForegroundColour(m_focusColor);
    Refresh();
}

void RichTextButton::Normal() {
    std::cout <<" normal"<<std::endl;
    SetForegroundColour(m_normalColor);
    Refresh();
}

void RichTextButton::SetFocusColor(wxColor& color) {
    m_focusColor = color;
}

void RichTextButton::SetNormalColor(wxColor& color) {
    m_normalColor = color;
}

void RichTextButton::OnMouseEnter(wxMouseEvent& event) {
    std::cout<<"Mouse Enter"<<std::endl;
    SetForegroundColour(m_focusColor);
    Refresh();
}

void RichTextButton::OnMouseLeave(wxMouseEvent& event) {
    std::cout<<"Mouse Leave"<<std::endl;
    SetForegroundColour(m_normalColor);
    Refresh();
}
