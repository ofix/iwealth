#include "ui/components/RichRadioEvent.h"

wxDEFINE_EVENT(wxEVT_RICH_RADIO, RichRadioEvent);

RichRadioEvent::RichRadioEvent(wxEventType type, int id) : wxEvent(id, type) {
}

RichRadioEvent::RichRadioEvent(const RichRadioEvent& event) : wxEvent(event) {
}

int RichRadioEvent::GetSelection() const {
    return m_selection;
}
void RichRadioEvent::SetSelection(int n) {
    m_selection = n;
}