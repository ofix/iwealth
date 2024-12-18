#ifndef RICHVOLUMEBAR_H
#define RICHVOLUMEBAR_H

#include <wx/dcclient.h>
#include <vector>
#include "stock/Stock.h"
#include "RichIndicatorCtrl.h"

class RichKlineCtrl;
class RichVolumeIndicatorCtrl: public RichIndicatorCtrl{
   public:
#define GUARD(x, y) ((x) < (y) ? (y) : (x))
    RichVolumeIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize);
    virtual ~RichVolumeIndicatorCtrl();
    virtual void Draw(wxDC* pDC) override;
    virtual std::string GetName() override;
    virtual std::string GetFormulaName() override;

    void DrawVolumeBar(wxDC* pDC);
    // void DrawMinuteBar(wxDC* pDC, std::vector<minuteKline>* pMinuteKlines, KlineType kline_type = KlineType::Minute);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);
    void SetMode(int mode);

   protected:
    double GetMaxVolumeInRange();
    double GetMaxVolume();
    double GetFiveDayMaxVolume();
};

#endif  // RICHVOLUMEBAR_H
