#ifndef RICH_TURNOVER_RATE_INDICATOR_CTRL_H
#define RICH_TURNOVER_RATE_INDICATOR_CTRL_H

#include "RichIndicatorCtrl.h"

class RichTurnoverRateIndicatorCtrl : public RichIndicatorCtrl {
   public:
    RichTurnoverRateIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                                  const wxPoint& pos = wxDefaultPosition,
                                  const wxSize& size = wxDefaultSize);
    virtual ~RichTurnoverRateIndicatorCtrl();
    virtual void Draw(wxDC* pDC) override;
    void DrawTitleBar(wxDC* pDC);
    virtual wxString GetName() override;
    virtual wxString GetFormulaName() override;
    void DrawTurnoverRateBar(wxDC* pDC);
    wxString GetYesterdayTurnoverRate();
    wxString GetTodayTurnoverRate();
    wxString GetWeekTurnoverRate();

   protected:
    std::vector<RichColorText> m_colorTextArr;
    double GetMaxTurnoverRateInRange();
    double GetFiveDayMaxTurnoverRate();
};

#endif  // end of RICH_TURNOVER_RATE_INDICATOR_CTRL_H