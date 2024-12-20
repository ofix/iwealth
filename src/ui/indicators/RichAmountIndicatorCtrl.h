#ifndef RICH_AMOUNT_INDICATOR_CTRL_H
#define RICH_AMOUNT_INDICATOR_CTRL_H

#include <wx/dcclient.h>
#include <vector>
#include "RichIndicatorCtrl.h"
#include "stock/Stock.h"

#define GUARD(x, y) ((x) < (y) ? (y) : (x))

class RichKlineCtrl;
class RichAmountIndicatorCtrl : public RichIndicatorCtrl {
   public:
    RichAmountIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize);
    virtual ~RichAmountIndicatorCtrl();
    virtual void Draw(wxDC* pDC) override;
    virtual std::string GetName() override;
    virtual std::string GetFormulaName() override;
    void DrawAmountBar(wxDC* pDC);
    // void DrawMinuteBar(wxDC* pDC, std::vector<minuteKline>* pMinuteKlines, KlineType kline_type = KlineType::Minute);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);

   protected:
    double GetMaxAmountInRange();
};

#endif  // RICH_AMOUNT_INDICATOR_CTRL_H
