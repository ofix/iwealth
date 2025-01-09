#ifndef RICH_CHECK_BOX_EVENT_H
#define RICH_CHECK_BOX_EVENT_H

#include <wx/event.h>
#include <vector>

class RichCheckBoxEvent;
wxDECLARE_EVENT(wxEVT_RICH_CHECKBOX, RichCheckBoxEvent);
typedef void (wxEvtHandler::*RichCheckBoxEventFunction)(RichCheckBoxEvent&);
#define RichCheckBoxEventHandler(func) wxEVENT_HANDLER_CAST(RichCheckBoxEventFunction, func)
#define EVT_RICH_CHECKBOX(id, func) wx__DECLARE_EVT1(wxEVT_RICH_CHECKBOX, id, RichCheckBoxEventHandler(func))

class RichCheckBoxEvent : public wxEvent {
   public:
    RichCheckBoxEvent(wxEventType type = wxEVT_RICH_CHECKBOX, int id = wxID_ANY);
    RichCheckBoxEvent(const RichCheckBoxEvent& event);
    wxEvent* Clone() const override {
        return new RichCheckBoxEvent(*this);
    }
    void GetChecked(int& index, bool& checked) const;
    void SetChecked(int index, bool checked);

   private:
    bool m_checked;
    int m_index;
};

#endif  // end of RICH_CHECK_BOX_EVENT_H