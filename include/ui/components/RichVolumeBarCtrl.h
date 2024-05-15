#ifndef RICHVOLUMEBAR_H
#define RICHVOLUMEBAR_H

#include <vector>
#include <wx/dcclient.h>
#include "stock/stock.h"

class RichKlineCtrl;
class RichVolumeBarCtrl {
    public:
#define GUARD(x,y) ((x)<(y)?(y):(x))
        RichVolumeBarCtrl(RichKlineCtrl* pKlineCtrl, long x=0,long y=0,long w=0,long h=0);
        virtual ~RichVolumeBarCtrl();
        void OnDraw(wxDC* pDC);
    protected:
        double GetMaxVolumeInRange();
    protected:
        RichKlineCtrl* m_pKlineCtrl;
};

#endif // RICHVOLUMEBAR_H
