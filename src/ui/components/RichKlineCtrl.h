#ifndef RICH_CTRL_KLINE_H
#define RICH_CTRL_KLINE_H

#include <wx/control.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/textfile.h>
#include <vector>

#include "stock/Stock.h"

#define KLINE_MODE_DAY 1
#define KLINE_MODE_WEEK 2
#define KLINE_MODE_MONTH 4
#define KLINE_MODE_YEAR 8
#define KLINE_MODES (KLINE_MODE_DAY | KLINE_MODE_WEEK | KLINE_MODE_MONTH | KLINE_MODE_YEAR)
#define NO_CROSS_LINE -1

class StockDataStorage;
class RichKlineCtrl {
   public:
    RichKlineCtrl(StockDataStorage* pStorage,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize);
    virtual ~RichKlineCtrl();
    void Init();
    void SetMode(int iMode);  // Day|Week|Month|Year
    int GetMode() const;

    void LoadKlines(const std::string& share_code);

    void OnPaint(wxDC* pDC);
    void OnBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftMouseDown(wxMouseEvent& event);

   protected:
    int GetInnerWidth();
    int GetInnerHeight();
    void DrawKline(wxDC* pDC,
                   int nKLine,
                   int visibleKLineCount,
                   float price_open,
                   float price_close,
                   float price_max,
                   float price_min,
                   float rect_price_high,
                   float rect_price_low,
                   int minX,
                   int minY,
                   int maxX,
                   int maxY,
                   int lineWidth,
                   int lineSpan);
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);  // 光标十字线
    void DrawAnalysisBar(wxDC* pDC);
    float GetRectMinPrice(std::vector<uiKline>& uiKlines, int begin, int end);
    float GetRectMaxPrice(std::vector<uiKline>& uiKlines, int begin, int end);
    uiKlineRange GetKlineRangeZoomIn(long totalKLines,
                                     long widthContainer,
                                     int16_t klineWidth = 5,
                                     int16_t klineSpan = 2,
                                     long crossLine = NO_CROSS_LINE);  // 放大K线图
    uiKlineRange GetKlineRangeZoomOut(long totalKLines,
                                      long crossLine = NO_CROSS_LINE);  // 以十字线为中心，否则右侧缩放
    wxPoint GetCrossLinePt(long n);

   protected:
    int m_iMode;    // DAY|Week|Month|Year
    wxPoint m_pos;  // 起始位置
    int m_width;
    int m_height;
    int m_curKline;         // the current k line under the cursor
    int m_klineWidth;       // single k line width
    int m_klineSpan;        // span between two single k line
    float m_rectPriceMax;   // the maximum price in the drawing rect
    float m_rectPriceMin;   // the minimum price in the drawing rect
    wxPoint m_crossLinePt;  // the current k line mouse point
    int m_crossLine;

    StockDataStorage* m_pStorage;     // 股票存储中心
    std::string m_shareCode;          // 股票代码
    std::vector<uiKline> m_uiKlines;  // 当前绘制的K线数据

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
