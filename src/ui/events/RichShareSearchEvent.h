#ifndef RICH_SHARE_SEARCH_EVENT_H
#define RICH_SHARE_SEARCH_EVENT_H

#include <wx/event.h>
#include <string>

class RichShareSearchEvent;
wxDECLARE_EVENT(wxEVT_RICH_SHARE_SEARCH, RichShareSearchEvent);
typedef void (wxEvtHandler::*RichShareSearchEventFunction)(RichShareSearchEvent&);
#define RichShareSearchEventHandler(func) wxEVENT_HANDLER_CAST(RichShareSearchEventFunction, func)
#define EVT_RICH_SHARE_SEARCH(id, func) wx__DECLARE_EVT1(wxEVT_RICH_SHARE_SEARCH, id, RichShareSearchEventHandler(func))

class RichShareSearchEvent : public wxEvent {
   public:
    RichShareSearchEvent(wxEventType type = wxEVT_RICH_SHARE_SEARCH, int id = wxID_ANY);
    RichShareSearchEvent(const RichShareSearchEvent& event);
    wxEvent* Clone() const override {
        return new RichShareSearchEvent(*this);
    }
    std::string GetShareCode() const;
    void SetShareCode(const std::string& share_code);

   private:
    std::string m_share_code;
};

#endif