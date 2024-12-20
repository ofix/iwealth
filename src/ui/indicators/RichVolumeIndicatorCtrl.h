#ifndef RICHVOLUMEBAR_H
#define RICHVOLUMEBAR_H

#include <wx/dcclient.h>
#include <vector>
#include "RichIndicatorCtrl.h"
#include "stock/Stock.h"

class RichKlineCtrl;
class RichVolumeIndicatorCtrl : public RichIndicatorCtrl {
   public:
#define GUARD(x, y) ((x) < (y) ? (y) : (x))
    RichVolumeIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize);
    virtual ~RichVolumeIndicatorCtrl();
    void DrawTitleBar(wxDC* pDC);
    virtual void Draw(wxDC* pDC) override;
    virtual wxString GetName() override;
    virtual wxString GetFormulaName() override;
    wxString GetTodayVolume();
    wxString GetYesterdayVolume();
    wxString GetWeekVolume();

    void DrawVolumeBar(wxDC* pDC);
    // void DrawMinuteBar(wxDC* pDC, std::vector<minuteKline>* pMinuteKlines, KlineType kline_type = KlineType::Minute);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);

   protected:
    std::vector<RichColorText> m_colorTextArr;
    double GetMaxVolumeInRange();
    double GetMaxVolume();
    double GetFiveDayMaxVolume();
};

#endif  // RICHVOLUMEBAR_H
