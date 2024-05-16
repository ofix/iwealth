#ifndef RICHKLINECTRL_H
#define RICHKLINECTRL_H

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
#define KLINE_MODES \
    (KLINE_MODE_DAY | KLINE_MODE_WEEK | KLINE_MODE_MONTH | KLINE_MODE_YEAR)
#define NO_CROSS_LINE -1

class RichVolumeBarCtrl;
class RichKlineInfoCtrl;
class RichKlineCtrl : public wxControl {
    wxDECLARE_DYNAMIC_CLASS(RichKlineCtrl);
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(RichKlineCtrl);

   public:
    RichKlineCtrl();
    RichKlineCtrl(std::string strShareCode, wxWindow* parent, wxWindowID id);
    RichKlineCtrl(wxWindow* parent,
                  wxWindowID id,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator);
    bool Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator);
    virtual ~RichKlineCtrl();
    void Init();
    void SetShare(Share share);
    Share GetShare() const;
    void SetMode(int iMode);  // Day|Week|Month|Year
    int GetMode() const;
    void SetCsvPath(std::string strPath);
    std::string GetCsvPath() const;

    // event callback functions
    void OnPaint(wxPaintEvent& event);
    void OnBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftMouseDown(wxMouseEvent& event);

   protected:
    bool ReadCsv();
    int GetInnerWidth();
    int GetInnerHeight();
    // draw helper functions
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
    void DrawCrossLine(wxDC* pDC, int centerX, int centerY, int w, int h);  //光标十字线
    void DrawAnalysisBar(wxDC* pDC);
    std::vector<uiKline> GetWeekKlines();
    std::vector<uiKline> GetMonthKlines();
    float GetRectMinPrice(std::vector<uiKline>& data, int begin, int end);
    float GetRectMaxPrice(std::vector<uiKline>& data, int begin, int end);
    uiKlineRange GetKlineRangeZoomIn(long totalKLines,
                                     long widthContainer,
                                     int16_t klineWidth = 5,
                                     int16_t klineSpan = 2,
                                     long crossLine = NO_CROSS_LINE);  //放大K线图
    uiKlineRange GetKlineRangeZoomOut(
        long totalKLines,
        long crossLine = NO_CROSS_LINE);  //以十字线为中心，否则右侧缩放
    wxPoint GetCrossLinePt(long n);

   protected:
    Share m_share;          // the share whose k line want to be draw
    int m_iMode;            // DAY|Week|Month|Year
    int m_iOrigin;          // 0-CSV 1-pointer
    int m_width;            // the drawing rect width
    int m_height;           // the drawing rect height
    int m_curKline;         // the current k line under the cursor
    int m_klineWidth;       // single k line width
    int m_klineSpan;        // span between two single k line
    float m_rectPriceMax;   // the maximum price in the drawing rect
    float m_rectPriceMin;   // the minimum price in the drawing rect
    wxPoint m_crossLinePt;  // the current k line mouse point
    int m_crossLine;
    std::string m_csvPath;               // csv file path of day k line history data
    std::vector<uiKline> m_klines;       // the k line data
    std::vector<uiKline> m_weekKlines;   // week k line data;
    std::vector<uiKline> m_monthKlines;  // month k line data;
    uiKlineRange m_klineRng;
    int m_paddingTop;     // padding top for klines control
    int m_paddingBottom;  // padding bottom for klines control
    int m_paddingRight;   // padding right for klines control
    // volume bar ctrl
    RichVolumeBarCtrl* m_pVolumeBar;
    RichKlineInfoCtrl* m_pInfoToolbar;
    bool m_showAnalysisBar;  // if true, draw volume,MCDA,KDJ index
    int m_analysisType;
    friend class RichVolumeBarCtrl;
    friend class RichKlineInfoCtrl;
};

#endif  // RICHKLINECTRL_H
