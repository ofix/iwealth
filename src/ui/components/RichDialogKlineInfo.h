#ifndef RICHKLINEINFOCTRL_H
#define RICHKLINEINFOCTRL_H

#include <wx/control.h>
#include <wx/datetime.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <string>
#include "stock/Stock.h"

class RichKlineCtrl;
class RichDialogKlineInfo : public wxDialog {
   public:
    RichDialogKlineInfo(RichKlineCtrl* pKlineCtrl,
                        wxWindow* parent,
                        wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize);
    virtual ~RichDialogKlineInfo();
    void OnDraw(wxDC& dc);
    uiKline GetCurrentKlineInfo();
    uiKline GetPrevKlineInfo();
    std::string FormatDay(std::string);
    std::string GetWeek(std::string day);
    std::string CalcWeek(int year, int month, int day);

   protected:
    RichKlineCtrl* m_pKlineCtrl;
};

#endif  // RICHKLINEINFOCTRL_H
