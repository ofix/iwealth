#ifndef RICHVOLUMEBAR_H
#define RICHVOLUMEBAR_H

#include <wx/dcclient.h>
#include <vector>
#include "stock/Stock.h"

class RichKlineCtrl;
class RichVolumeBarCtrl {
   public:
#define GUARD(x, y) ((x) < (y) ? (y) : (x))
    RichVolumeBarCtrl(RichKlineCtrl* pKlineCtrl,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize);
    virtual ~RichVolumeBarCtrl();
    void OnPaint(wxDC* pDC);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);

   protected:
    double GetMaxVolumeInRange();

   protected:
    RichKlineCtrl* m_pKlineCtrl;
};

#endif  // RICHVOLUMEBAR_H
