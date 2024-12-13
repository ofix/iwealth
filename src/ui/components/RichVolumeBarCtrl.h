#ifndef RICHVOLUMEBAR_H
#define RICHVOLUMEBAR_H

#include <wx/dcclient.h>
#include <vector>
#include "stock/Stock.h"
#include "ui/components/RichIndicatorCtrl.h"

class RichKlineCtrl;
class RichVolumeBarCtrl: public RichIndicatorCtrl{
   public:
#define GUARD(x, y) ((x) < (y) ? (y) : (x))
    RichVolumeBarCtrl(RichKlineCtrl* pKlineCtrl,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize);
    virtual ~RichVolumeBarCtrl();
    void OnPaint(wxDC* pDC);
    virtual void DoDraw(wxDC* pDC);
    void DrawVolumeBar(wxDC* pDC);
    void DrawAmountBar(wxDC* pDC);
    void DrawMinuteBar(wxDC* pDC, std::vector<minuteKline>* pMinuteKlines, KlineType kline_type = KlineType::Minute);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);
    void SetMode(int mode);

   protected:
    double GetMaxVolumeInRange();
    double GetMaxAmountInRange();
    double GetMaxVolume();
    double GetFiveDayMaxVolume();

   protected:
    RichKlineCtrl* m_pKlineCtrl;
    int m_mode;
};

#endif  // RICHVOLUMEBAR_H
