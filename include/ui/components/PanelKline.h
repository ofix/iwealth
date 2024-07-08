#ifndef PANELKLINE_H
#define PANELKLINE_H

#include <wx/panel.h>
#include <string>
#include <vector>
#include "stock/Stock.h"
#include "ui/components/RichKlineCtrl.h"
#include "ui/components/RichKlineInfoCtrl.h"
#include "ui/components/RichRadioCtrl.h"

class PanelKline : public wxPanel {
   public:
    PanelKline(wxWindow* parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize);
    virtual ~PanelKline();
    void SetShareCode(const std::string& share_code);

   protected:
    RichKlineCtrl* m_pKlineCtrl;
    RichKlineInfoCtrl* m_pKlineInfoCtrl;
    RichRadioCtrl* m_pRadioCtrl;  // 日K线/周K线/月K线/季度K线/年K线

    std::vector<uiKline>* m_pKlines;  // 当前需要显示的K线

   protected:
    static const long ID_KLINE_CTRL;
    static const long ID_KLINE_INFO_CTRL;
    static const long ID_RAIDO_CTRL;
    std::string m_share_code;
};

#endif