#ifndef RICHVOLUMEBAR_H
#define RICHVOLUMEBAR_H

#include <wx/control.h>
#include <wx/dcclient.h>
#include <vector>
#include "stock/Stock.h"

class RichKlineCtrl;
class RichVolumeBarCtrl : public wxControl {
   public:
#define GUARD(x, y) ((x) < (y) ? (y) : (x))
    RichVolumeBarCtrl(RichKlineCtrl* pKlineCtrl,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize);
    virtual ~RichVolumeBarCtrl();
    void OnDraw(wxDC* pDC);

   protected:
    double GetMaxVolumeInRange();

   protected:
    RichKlineCtrl* m_pKlineCtrl;
};

#endif  // RICHVOLUMEBAR_H
