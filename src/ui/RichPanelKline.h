#ifndef RICH_PANEL_KLINE_H
#define RICH_PANEL_KLINE_H

#include <string>
#include <vector>
#include "stock/Stock.h"
#include "ui/RichDialogKlineInfo.h"
#include "ui/components/RichCheckBox.h"
#include "ui/components/RichKlineCtrl.h"
#include "ui/components/RichPanel.h"
#include "ui/components/RichRadioCtrl.h"
#include "ui/events/RichCheckBoxEvent.h"
#include "ui/events/RichRadioEvent.h"
#include "ui/indicators/RichIndicatorCtrl.h"

class RichPanelKline : public RichPanel {
   public:
    RichPanelKline(PanelType type,
                   StockDataStorage* pStorage,
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
    void OnKlineChanged(RichRadioEvent& event);
    void OnVolumeBarChanged(RichRadioEvent& event);
    void OnEmaPriceReferenceLineChanged(RichCheckBoxEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void ShowDialogKlineInfoIfNeeded();

    // 附图指标相关操作
    void IndicatorMoveUp(int i);                                  // 附图指标上移
    void IndicatorMoveDown(int i);                                // 附图指标下移
    void IndicatorInsert(RichIndicatorCtrl* pIndicator);          // 增加一个附图指标到末尾
    void IndicatorDelete(int i);                                  // 删除附图指标
    void IndicatorReplace(int i, RichIndicatorCtrl* pIndicator);  // 替换指定位置的附图指标
    void IndicatorReLayout();                                     // 重新布局附图指标
    void ShowIndicators();                                        // 显示所有附图指标
    void HideIndicators();                                        // 隐藏所有附图指标
    void ShowMinuteIndicator();                                   // 显示分时附图指标
    void HideMinuteIndicator();                                   // 隐藏分时附图指标

   protected:
    RichKlineCtrl* m_pKlineCtrl;

    RichDialogKlineInfo* m_pDialogKlineInfo;
    wxStaticText* m_pShareNameCtrl;     // 股票名称控件
    wxStaticText* m_pAddFavoriteCtrl;   // 增加自选
    RichRadioCtrl* m_pRadioCtrl;        // 日K线/周K线/月K线/季度K线/年K线
    RichCheckBox* m_pEmaPriceCheckBox;  // EMA均价参考线选择框

    std::vector<uiKline>* m_pKlines;               // 当前需要显示的K线
    std::vector<RichIndicatorCtrl*> m_indicators;  // 日线/周线/月线/年线 附图指标集合
    RichIndicatorCtrl* m_pMinuteIndicator;         // 分时/五日分时 附图指标
    KlineType m_curKlineType;                      // 当前K线界面类别(分时/5日/日线/周线/月线/年线)
    KlineType m_oldKlineType;                      // 之前的K线类型
    bool m_oldInMinuteMode;                        // 之前是否是分时模式，如果值相同，无需ReLayout
   protected:
    static const long ID_SHARE_NAME_CTRL;
    static const long ID_ADD_FAVORITE_CTRL;
    static const long ID_RADIO_CTRL;
    static const long ID_SECOND_RADIO_CTRL;
    static const long ID_KLINE_CTRL;
    static const long ID_DIALOG_KLINE_INFO;
    static const long ID_EMA_PRICE_CHECKBOX;

    wxPoint m_ptKlineCtrl;
    wxSize m_sizeKlineCtrl;
    wxPoint m_ptVolumeBarCtrl;
    wxSize m_sizeVolumeBarCtrl;
    wxPoint m_ptKlineInfoCtrl;

    Share* m_pShare;  // 当前显示的股票指针

   private:
    DECLARE_EVENT_TABLE()
};

#endif