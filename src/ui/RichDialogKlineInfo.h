#ifndef RICH_KLINE_INFO_CTRL_H
#define RICH_KLINE_INFO_CTRL_H

#include <wx/control.h>
#include <wx/datetime.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <string>
#include "stock/Stock.h"

class RichDialogKlineInfo : public wxDialog {
   public:
    RichDialogKlineInfo(wxWindow* parent,
                        wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize);
    virtual ~RichDialogKlineInfo();
    void OnPaint(wxPaintEvent& event);
    void SetCurrentKline(uiKline* pKline);
    void SetPreviousKline(uiKline* pKline);
    std::string FormatDay(std::string);
    std::string GetWeek(std::string day);
    std::string CalcWeek(int year, int month, int day);

   protected:
    uiKline* m_pKline;
    uiKline* m_pPreviousKline;
    DECLARE_EVENT_TABLE()
};

#endif  // RICHKLINEINFOCTRL_H
