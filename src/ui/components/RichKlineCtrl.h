#ifndef RICH_CTRL_KLINE_H
#define RICH_CTRL_KLINE_H

#include <wx/control.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/textfile.h>
#include <stack>
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
    void OnPaint(wxDC* pDC);

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
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);  // 光标十字线
    void DrawEmaCurves(wxDC* pDC,
                       int visibleKLineCount,
                       float rect_price_max,
                       float rect_price_min,
                       int minX,
                       int minY,
                       int maxX,
                       int maxY,
                       double klineWidth);                        // 绘制所有EMA曲线
    bool ShowEmaCurve(int n);                                     // 显示周期为n的EMA平滑移动价格曲线
    bool HideEmaCurve(int n);                                     // 隐藏周期为n的EMA平滑移动价格曲线
    bool AddEmaCurve(int n, wxColor color, bool visible = true);  // 添加周期为n的EMA平滑移动价格曲线
    bool DelEmaCurve(int n);                                      // 删除周期为n的EMA平滑移动价格曲线
    bool HasEmaCurve();                                           // 判断是否显示EMA曲线

    // 分时图/5日分时图
    void CalcMinuteKlineAvgPrice(std::vector<minuteKline>& minuteKlines, std::vector<double>& avg_price);
    void DrawFiveDayMinuteKlines(wxDC* pDC);       // 5日分时图
    void DrawMinuteKlines(wxDC* pDC);              // 分时图
    void DrawMinuteKlineBackground(wxDC* pDC);     // 分时图背景
    void DrawMinuteKlineCurves(wxDC* pDC);         // 分时图曲线
    void PrintDebugInfo(std::string prefix = "");  // 打印调试信息

   protected:
    void RemoveCache();
    KlineType m_mode;  // Minute|Five Day Minute|Day|Week|Month|Quarter|Year
    wxPoint m_pos;     // 起始位置
    int m_width;
    int m_height;
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

    StockDataStorage* m_pStorage;               // 股票存储中心
    std::string m_shareCode;                    // 股票代码
    Share* m_pShare;                            // 股票指针
    std::vector<uiKline>* m_pKlines;            // 当前绘制的K线数据
    std::vector<minuteKline>* m_pMinuteKlines;  // 当前绘制的分时图数据
    std::vector<ShareEmaCurve> m_emaCurves;     // 指数移动平均线
    std::vector<minuteKline> m_minuteKlines;    // 分时图
    std::vector<minuteKline> m_fiveDayKlines;   // 5日分时图

    uiKlineRange m_klineRng;

    int m_paddingTop;        // padding top for klines control
    int m_paddingBottom;     // padding bottom for klines control
    int m_paddingRight;      // padding right for klines control
    bool m_showAnalysisBar;  // if true, draw volume,MCDA,KDJ index
    int m_analysisType;
    friend class RichDialogKlineInfo;
    friend class RichVolumeBarCtrl;
};

#endif  // RICH_CTRL_KLINE_H
