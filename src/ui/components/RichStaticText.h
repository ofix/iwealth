#ifndef RICH_STATIC_TEXT
#define RICH_STATIC_TEXT

#include <wx/stattext.h>
class RichStaticText : public wxStaticText {
   public:
    RichStaticText(wxWindow* parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxStaticTextNameStr);
    virtual bool AcceptsFocusFromKeyboard() const {
        return false;
    }
    virtual bool AcceptsFocus() const {
        return false;
    }
};

#endif