#ifndef RICHKLINEINFOCTRL_H
#define RICHKLINEINFOCTRL_H

#include <string>
#include <wx/dcclient.h>
#include <wx/datetime.h>
#include "stock/Stock.h"

class RichKlineCtrl;
class RichKlineInfoCtrl {
    public:
        RichKlineInfoCtrl(RichKlineCtrl* pKlineCtrl, long x=0,long y=0,long w=0,long h=0);
        virtual ~RichKlineInfoCtrl();
        void OnDraw(wxDC* pDC);
        KlineItem GetCurrentKlineInfo();
        KlineItem GetPrevKlineInfo();
        std::string FormatDay(std::string);
        std::string GetWeek(std::string day);
        std::string CalcWeek(int year,int month, int day);
    protected:
        RichKlineCtrl* m_pKlineCtrl;
};

#endif // RICHKLINEINFOCTRL_H
