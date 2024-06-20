#ifndef RICHKLINEINFOCTRL_H
#define RICHKLINEINFOCTRL_H

#include <wx/control.h>
#include <wx/datetime.h>
#include <wx/dcclient.h>
#include <string>
#include "stock/Stock.h"

class RichKlineCtrl;
class RichKlineInfoCtrl : public wxControl {
   public:
    RichKlineInfoCtrl(RichKlineCtrl* pKlineCtrl,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize);
    virtual ~RichKlineInfoCtrl();
    void OnDraw(wxDC* pDC);
    uiKline GetCurrentKlineInfo();
    uiKline GetPrevKlineInfo();
    std::string FormatDay(std::string);
    std::string GetWeek(std::string day);
    std::string CalcWeek(int year, int month, int day);

   protected:
    RichKlineCtrl* m_pKlineCtrl;
};

#endif  // RICHKLINEINFOCTRL_H
