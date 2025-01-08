#ifndef RICH_INDICATOR_CTRL_H
#define RICH_INDICATOR_CTRL_H

#include <wx/dc.h>
#include <string>
#include "ui/RichHelper.h"
#include "util/Color.h"

class RichKlineCtrl;
class RichIndicatorCtrl {
   public:
    RichIndicatorCtrl();
    RichIndicatorCtrl(RichKlineCtrl* pKlineCtrl, int x, int y, int w, int h);
    RichIndicatorCtrl(RichKlineCtrl* pKlineCtrl,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize);
    virtual ~RichIndicatorCtrl();
    virtual void Draw(wxDC* pDC);       // 绘制函数
    virtual wxString GetName();         // 获取指标名称
    virtual wxString GetFormulaName();  // 获取指标公式名称
    void SetMode(int mode);             // 设置日K线当前模式
    void DrawColorTextArr(wxDC* pDC, int x, int y, std::vector<RichColorText>& colorTexts);

    void DrawUpArrow(wxDC* pDC);    // 绘制上箭头
    void DrawDownArrow(wxDC* pDC);  // 绘制下箭头
    bool IsVisible() const;
    void Hide();
    void Show();
    int GetMaxHeight() const;
    int GetMinHeight() const;
    void SetPosition(wxPoint& pos);
    void SetPosition(int x, int y);
    void SetWidth(int width);
    void SetManualHeight(int height);
    void SetMaxHeight(int height);
    void SetMinHeight(int height);
    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;
    RichKlineCtrl* GetKlineCtrl() const;

   protected:
    RichKlineCtrl* m_pKlineCtrl;
    int m_x;                // 起始X坐标
    int m_y;                // 起始Y坐标
    int m_width;            // 显示总宽度
    int m_height;           // 显示总高度
    int m_bodyHeight;       // 实际高度
    int m_titleHeight;      // 标题高度
    int m_mode;             // 当前显示的K线类型
    bool m_visible;         // 是否可见
    int m_upArrowX;         // 上箭头X坐标
    int m_upArrowY;         // 上箭头Y坐标
    int m_upArrowWidth;     // 上箭头宽度
    int m_upArrowHeight;    // 上箭头高度
    int m_downArrowX;       // 下箭头X坐标
    int m_downArrowY;       // 下箭头Y坐标
    int m_downArrowWidth;   // 下箭头宽度
    int m_downArrowHeight;  // 下箭头高度
    int m_heightManual;     // 用户指定的高度，程序不自适应
    int m_minHeight;        // 最小高度
    int m_maxHeight;        // 最大高度
};

#endif