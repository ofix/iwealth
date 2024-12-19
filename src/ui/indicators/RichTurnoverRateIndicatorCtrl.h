#ifndef RICH_TURNOVER_RATE_INDICATOR_CTRL_H
#define RICH_TURNOVER_RATE_INDICATOR_CTRL_H

#include "RichIndicatorCtrl.h"

class RichTurnoverRateIndicatorCtrl : public RichIndicatorCtrl {
   public:
    RichTurnoverRateIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                                  const wxPoint& pos = wxDefaultPosition,
                                  const wxSize& size = wxDefaultSize);
    virtual void Draw(wxDC* pDC) override;
    void DrawTurnoverRateBar(wxDC* pDC);
    virtual std::string GetName() override;
    virtual std::string GetFormulaName() override;

   protected:
    double GetMaxTurnoverRateInRange();
    double GetFiveDayMaxTurnoverRate();
};

#endif  // end of RICH_TURNOVER_RATE_INDICATOR_CTRL_H