#ifndef RICH_CTRL_KLINE_H
#define RICH_CTRL_KLINE_H

#include <wx/control.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/textfile.h>
#include <stack>
#include <unordered_map>
#include <vector>

#include "stock/Stock.h"

#define NO_CROSS_LINE -1

class StockDataStorage;
class RichKlineCtrl {
   public:
    RichKlineCtrl(StockDataStorage* pStorage,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize);
    virtual ~RichKlineCtrl();
    void Init();
    uiKline* GetCurrentKline();    // 获取光标当前所在K线
    uiKline* GetPreviousKline();   // 获取光标前一天所在K线
    void SetMode(KlineType mode);  // Minute|Five Day Minute|Day|Week|Month|Quarter|Year
    KlineType GetMode() const;
    bool IsFocus();

    bool LoadKlines(const std::string& share_code, const KlineType& kline_type = KlineType::Day);
    bool LoadKlines(const KlineType& kline_type);
    bool LoadPrevKlines();
    bool LoadNextKlines();
    std::string GetShareCode() const;
    Share* GetCurrentShare() const;

    void OnBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftMouseDown(wxMouseEvent& event);
    void ZoomIn();   // 放大K线图
    void ZoomOut();  // 以十字线为中心，否则右侧缩放
    void CalcVisibleKlineWidth();
    wxPoint GetCrossLinePt(long n);

    // 绘制函数
    void OnPaint(wxAutoBufferedPaintDC* pDC);
    void DrawCrossLine(wxDC* pDC);  // 光标十字线
    int GetWidth();
    int GetHeight();
    void SetWidth(int width);
    void SetHeight(int height);
    void SetCrossLineHeight(int height);

   protected:
    int GetInnerWidth();
    int GetInnerHeight();
    float GetRectMinPrice(std::vector<uiKline>& uiKlines, int begin, int end);
    float GetRectMaxPrice(std::vector<uiKline>& uiKlines, int begin, int end);
    float GetRectMinPrice(std::vector<minuteKline>& minuteKlines, int begin, int end);
    float GetRectMaxPrice(std::vector<minuteKline>& minuteKlines, int begin, int end);

    // 日K线|周K线|月K线|季K线|年K线
    void DrawKlineBackground(wxDC* pDC, double price_min, double price_max);
    void DrawDayKlines(wxDC* pDC);
    void DrawEmaText(wxDC* pDC);
    void DrawMinMaxRectPrice(wxDC* pDC);
    bool CheckPtInEmaText(wxPoint& pt);
    void DrawEmaCurves(wxDC* pDC,
                       float rect_price_max,
                       float rect_price_min,
                       int minX,
                       int minY,
                       int maxX,
                       int maxY,
                       double klineWidth);  // 绘制所有EMA曲线
    void DrawEmaPriceReferenceLine(wxDC* pDC,
                                   int n,
                                   wxColor& clr,
                                   wxPoint& ptStart,
                                   wxPoint& ptEnd);  // 绘制周期为n的EMA均价参考线
    bool ShowEmaCurve(int n);                        // 显示周期为n的EMA平滑移动价格曲线
    bool HideEmaCurve(int n);                        // 隐藏周期为n的EMA平滑移动价格曲线
    bool AddEmaCurve(int n, wxColour color,
                     bool visible = true);  // 添加周期为n的EMA平滑移动价格曲线
    bool DelEmaCurve(int n);                // 删除周期为n的EMA平滑移动价格曲线
    bool HasEmaCurve();                     // 判断是否显示EMA曲线
    double GetNearestEmaPrice(int n);       // 获取最近N周期均线价格(5日线/10日线/20日线)

    // 分时图/5日分时图
    void CalcMinuteKlineAvgPrice(std::vector<minuteKline>& minuteKlines, std::vector<double>& avg_price);
    void DrawFiveDayMinuteKlines(wxDC* pDC);                                                       // 5日分时图
    void DrawFiveDayMinuteKlineBackground(wxDC* pDC, double ref_close_price, double delta_price);  // 5日分时图背景
    void DrawMinuteKlines(wxDC* pDC);                                                              // 分时图
    void DrawMinuteKlineBackground(wxDC* pDC, double yesterday_close_pricem, double delta_price);  // 分时图背景
    void PrintDebugInfo(std::string prefix = "");                                                  // 打印调试信息

   protected:
    void RemoveCache();
    KlineType m_mode;  // Minute|Five Day Minute|Day|Week|Month|Quarter|Year
    wxPoint m_pos;     // 起始位置
    int m_width;
    int m_height;
    int m_crossLineHeight;
    double m_klineInnerWidth;  // K线实际宽度，有可能为小数
    double m_klineWidth;       // K线宽度，包含K线间距,有可能为小数
    int m_visibleKlineCount;   // 可见的K线数量
    float m_minRectPrice;      // 如果有EMA均线，K线高度需要变化
    float m_maxRectPrice;      // 如果有EMA均线，K线高度需要变化
    float m_minKlinePrice;     // 可见K线中最低价
    float m_maxKlinePrice;     // 可见K线中最高价
    int m_minRectPriceIndex;   // 可见K线中最低价K线位置
    int m_maxRectPriceIndex;   // 可见K险种最高价K线位置
    int m_curKline;            // 当前K线十字线
    wxPoint m_crossLinePt;     // 当前K线十字线坐标
    int m_crossLine;

    StockDataStorage* m_pStorage;                        // 股票存储中心
    std::string m_shareCode;                             // 股票代码
    Share* m_pShare;                                     // 股票指针
    std::vector<uiKline>* m_pKlines;                     // 当前绘制的K线数据
    std::vector<minuteKline>* m_pMinuteKlines;           // 当前绘制的分时图数据
    std::vector<ShareEmaCurve> m_emaCurves;              // 指数移动平均线
    std::vector<minuteKline>* m_pFiveDayMinuteKlines;    // 5日分时图
    std::unordered_map<int, double> m_nearestEmaPrices;  // 5/10/20/30/60/90等日均线最新价格
    bool m_showEmaPriceReferenceLine;                    // 是否显示EMA日均线参考线

    uiKlineRange m_klineRng;

    int m_paddingTop;        // 上边距
    int m_paddingBottom;     // 下边距
    int m_paddingLeft;       // 绘制分时图的左边距
    int m_paddingRight;      // 绘制分时图的右边
    bool m_showAnalysisBar;  // if true, draw volume,MCDA,KDJ index
    int m_analysisType;
    friend class RichDialogKlineInfo;
    friend class RichVolumeIndicatorCtrl;
    friend class RichAmountIndicatorCtrl;
    friend class RichTurnoverRateIndicatorCtrl;
    friend class RichMinuteVolumeIndicatorCtrl;
};

#endif  // RICH_CTRL_KLINE_H
