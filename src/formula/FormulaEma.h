#ifndef FORMULA_EMA_H
#define FORMULA_EMA_H

#include <wx/gdicmn.h>
#include <vector>
#include "formula/FormulaBase.h"
#include "stock/Stock.h"

class FormulaEma : public FormulaBase {
   public:
    FormulaEma(FormulaType type = FormulaType::EMA);
    virtual ~FormulaEma();
    static void GetEmaPrice(std::vector<uiKline>& klines, std::vector<double>& ema_prices, int n);
};

#endif