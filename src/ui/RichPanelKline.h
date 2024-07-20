#ifndef PANELKLINE_H
#define PANELKLINE_H

#include <string>
#include <vector>
#include "stock/Stock.h"
#include "ui/RichDialogKlineInfo.h"
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
    RichRadioCtrl* m_pRadioCtrl;     // 日K线/周K线/月K线/季度K线/年K线
    wxStaticText* m_pShareNameCtrl;  // 股票名称控件

    std::vector<uiKline>* m_pKlines;  // 当前需要显示的K线
   protected:
    static const long ID_SHARE_NAME_CTRL;
    static const long ID_RAIDO_CTRL;
    static const long ID_KLINE_CTRL;
    static const long ID_DIALOG_KLINE_INFO;

    wxPoint m_ptKlineCtrl;
    wxSize m_sizeKlineCtrl;
    wxPoint m_ptVolumeBarCtrl;
    wxSize m_sizeVolumeBarCtrl;

    Share* m_pShare;  // 当前显示的股票指针

   private:
    DECLARE_EVENT_TABLE()
};

#endif