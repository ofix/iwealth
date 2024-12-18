#ifndef RICH_AMOUNT_BAR_CTRL_H
#define RICH_AMOUNT_BAR_CTRL_H

#include <wx/dcclient.h>
#include <vector>
#include "stock/Stock.h"
#include "RichIndicatorCtrl.h"

class RichKlineCtrl;
class RichAmountIndicatorCtrl: public RichIndicatorCtrl{
   public:
#define GUARD(x, y) ((x) < (y) ? (y) : (x))
    RichAmountIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize);
    virtual ~RichAmountIndicatorCtrl();
    void OnPaint(wxDC* pDC);
    virtual void Draw(wxDC* pDC) override;
    virtual std::string GetName() override;
    virtual std::string GetFormulaName() override;
    void DrawAmountBar(wxDC* pDC);
    // void DrawMinuteBar(wxDC* pDC, std::vector<minuteKline>* pMinuteKlines, KlineType kline_type = KlineType::Minute);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);
    // void SetMode(int mode);

   protected:
    double GetMaxAmountInRange();
};

#endif  // RICH_AMOUNT_BAR_CTRL_H
