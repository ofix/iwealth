#ifndef RICH_RADIO_EVENT_H
#define RICH_RADIO_EVENT_H

#include <wx/event.h>

class RichRadioEvent;
wxDECLARE_EVENT(wxEVT_RICH_RADIO, RichRadioEvent);
typedef void (wxEvtHandler::*RichRadioEventFunction)(RichRadioEvent&);
#define RichRadioEventHandler(func) wxEVENT_HANDLER_CAST(RichRadioEventFunction, func)
#define EVT_RICH_RADIO(id, func) wx__DECLARE_EVT1(wxEVT_RICH_RADIO, id, RichRadioEventHandler(func))

class RichRadioEvent : public wxEvent {
   public:
    RichRadioEvent(wxEventType type = wxEVT_RICH_RADIO, int id = wxID_ANY);
    RichRadioEvent(const RichRadioEvent& event);
    wxEvent* Clone() const override {
        return new RichRadioEvent(*this);
    }
    int GetSelection() const;
    void SetSelection(int n);

   private:
    int m_selection;
};

#endif