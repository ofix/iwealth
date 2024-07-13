#ifndef PANELKLINE_H
#define PANELKLINE_H

#include <string>
#include <vector>
#include "stock/Stock.h"
#include "ui/components/RichDialogKlineInfo.h"
#include "ui/components/RichKlineCtrl.h"
#include "ui/components/RichPanel.h"
#include "ui/components/RichRadioCtrl.h"
#include "ui/components/RichVolumeBarCtrl.h"

class RichPanelKline : public RichPanel {
   public:
    RichPanelKline(PanelType type,
                   wxWindow* parent,
                   wxWindowID id = wxID_ANY,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize);
    virtual ~RichPanelKline();
    void SetShareCode(const std::string& share_code);

    // 事件处理器
    void OnPaint(wxPaintEvent& event);
    void OnBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftMouseDown(wxMouseEvent& event);

   protected:
    RichKlineCtrl* m_pKlineCtrl;
    RichVolumeBarCtrl* m_pVolumeBarCtrl;
    RichDialogKlineInfo* m_pDialogKlineInfo;
    RichRadioCtrl* m_pRadioCtrl;  // 日K线/周K线/月K线/季度K线/年K线

    std::vector<uiKline>* m_pKlines;  // 当前需要显示的K线
   protected:
    static const long ID_KLINE_CTRL;
    static const long ID_DIALOG_KLINE_INFO;
    static const long ID_RAIDO_CTRL;
    std::string m_share_code;

   private:
    DECLARE_EVENT_TABLE()
};

#endif