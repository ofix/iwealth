#ifndef RICH_CTRL_KLINE_H
#define RICH_CTRL_KLINE_H

#include <wx/control.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/textfile.h>
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
    void SetMode(KlineType mode);  // Minute|Five Day Minute|Day|Week|Month|Quarter|Year
    KlineType GetMode() const;

    bool LoadKlines(const std::string& share_code, const KlineType& kline_type = KlineType::Day);
    bool LoadKlines(const KlineType& kline_type);
    std::string GetShareCode() const;

    void OnPaint(wxDC* pDC);
    void DrawMinuteKlines(wxDC* pDC);         // 分时图
    void DrawFiveDayMinuteKlines(wxDC* pDC);  // 5日分时图
    void DrawDayKlines(wxDC* pDC);            // 日K线|周K线|月K线|季K线|年K线
    void OnBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftMouseDown(wxMouseEvent& event);

   protected:
    int GetInnerWidth();
    int GetInnerHeight();

    void DrawMinuteKlineBackground(wxDC* pDC);
    void DrawMinuteKlineCurves(wxDC* pDC);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);  // 光标十字线
    void DrawAnalysisBar(wxDC* pDC);
    void DrawEmaCurves(wxDC* pDC,
                       int visibleKLineCount,
                       float rect_price_max,
                       float rect_price_min,
                       int minX,
                       int minY,
                       int maxX,
                       int maxY,
                       int klineWidth,
                       int klineSpan);                            // 绘制所有EMA曲线
    bool HideEmaCurve(int n);                                     // 隐藏周期为n的EMA平滑移动价格曲线
    bool ShowEmaCurve(int n);                                     // 显示周期为n的EMA平滑移动价格曲线
    bool AddEmaCurve(int n, wxColor color, bool visible = true);  // 添加周期为n的EMA平滑移动价格曲线
    bool DelEmaCurve(int n);                                      // 删除周期为n的EMA平滑移动价格曲线
    bool HasEmaCurve();
    float GetRectMinPrice(std::vector<uiKline>& uiKlines, int begin, int end);
    float GetRectMaxPrice(std::vector<uiKline>& uiKlines, int begin, int end);
    float GetRectMinPrice(std::vector<minuteKline>& minuteKlines, int begin, int end);
    float GetRectMaxPrice(std::vector<minuteKline>& minuteKlines, int begin, int end);
    void CalcMinuteKlineAvgPrice(std::vector<minuteKline>& minuteKlines, std::vector<double>& avg_price);
    uiKlineRange GetKlineRangeZoomIn(long totalKLines,
                                     long widthContainer,
                                     int16_t klineWidth = 5,
                                     int16_t klineSpan = 2,
                                     long crossLine = NO_CROSS_LINE);  // 放大K线图
    uiKlineRange GetKlineRangeZoomOut(long totalKLines,
                                      long crossLine = NO_CROSS_LINE);  // 以十字线为中心，否则右侧缩放
    wxPoint GetCrossLinePt(long n);

   protected:
    void RemoveCache();
    KlineType m_mode;  // Minute|Five Day Minute|Day|Week|Month|Quarter|Year
    wxPoint m_pos;     // 起始位置
    int m_width;
    int m_height;
    int m_curKline;         // the current k line under the cursor
    int m_klineWidth;       // single k line width
    int m_klineSpan;        // span between two single k line
    float m_rectPriceMax;   // the maximum price in the drawing rect
    float m_rectPriceMin;   // the minimum price in the drawing rect
    wxPoint m_crossLinePt;  // the current k line mouse point
    int m_crossLine;

    StockDataStorage* m_pStorage;               // 股票存储中心
    std::string m_shareCode;                    // 股票代码
    std::vector<uiKline>* m_pKlines;            // 当前绘制的K线数据
    std::vector<minuteKline>* m_pMinuteKlines;  // 当前绘制的分时图数据
    std::vector<ShareEmaCurve> m_emaCurves;     // 指数移动平均线
    std::vector<minuteKline> m_minuteKlines;    // 分时图
    std::vector<minuteKline> m_fiveDayKlines;   // 5日分时图

    uiKlineRange m_klineRng;
    int m_visibleKlineCount;  // 可见的K线数量
    int m_paddingTop;         // padding top for klines control
    int m_paddingBottom;      // padding bottom for klines control
    int m_paddingRight;       // padding right for klines control
    bool m_showAnalysisBar;   // if true, draw volume,MCDA,KDJ index
    int m_analysisType;
    friend class RichDialogKlineInfo;
    friend class RichVolumeBarCtrl;
};

#endif  // RICH_CTRL_KLINE_H
