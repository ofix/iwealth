#ifndef RICH_KLINE_INFO_CTRL_H
#define RICH_KLINE_INFO_CTRL_H

#include <wx/control.h>
#include <wx/datetime.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <string>
#include "stock/Stock.h"
#include "ui/components/RichDialog.h"

class RichDialogKlineInfo : public RichDialog {
   public:
    RichDialogKlineInfo(wxWindow* parent,
                        wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize);
    virtual ~RichDialogKlineInfo();
    virtual void OnDrawClient(wxDC* pDC);
    void SetCurrentKline(uiKline* pKline);
    void SetPreviousKline(uiKline* pKline);
    std::string FormatDay(std::string);
    std::string GetWeek(std::string day);
    std::string CalcWeek(int year, int month, int day);

    void SetTitleBackgroundColor(wxColor color);
    void SetTitleForegroundColor(wxColor color);
    // 绘制自定义标题栏
    void DrawTitle(wxDC* pDC);
    // 绘制自定义关闭按钮
    void DrawCloseButton(wxDC* pDC);
    // 是否鼠标在关闭按钮位置上
    bool IsInTitleBar(wxPoint& ptCursor);
    bool IsInCloseButton(wxPoint& ptCursor);

   protected:
    uiKline* m_pKline;
    uiKline* m_pPreviousKline;
};

#endif  // RICHKLINEINFOCTRL_H
