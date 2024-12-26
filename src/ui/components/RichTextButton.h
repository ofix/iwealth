#ifndef RICH_TEXT_BUTTON_H
#define RICH_TEXT_BUTTON_H

#include <wx/colour.h>
#include <wx/event.h>
#include <wx/button.h>

class RichTextButton : public wxButton {
   public:
    RichTextButton(wxColor& normalColor,
                   wxColor& focusColor,
                   wxWindow* parent,
                   wxWindowID id = wxID_ANY,
                   const wxString& label = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0);
    virtual ~RichTextButton();
    wxColor GetFocusColor() const;
    wxColor GetNormalColor() const;
    void Active();
    void Normal();
    void SetFocusColor(wxColor& color);
    void SetNormalColor(wxColor& color);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

   private:
    wxColour m_focusColor;
    wxColour m_normalColor;
    wxColour m_disableColor;
    wxDECLARE_EVENT_TABLE();
};

#endif