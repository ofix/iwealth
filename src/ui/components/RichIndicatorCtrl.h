#ifndef RICH_INDICATOR_CTRL_H
#define RICH_INDICATOR_CTRL_H

class RichIndicatorCtrl {
   public:
    RichIndicatorCtrl();
    RichIndicatorCtrl(int x, int y, int w, int h);
    virtual ~RichIndicatorCtrl();
    virtual void Draw(wxDC* pDC);
    void DrawUpArrow(wxDC* pDC);    // 绘制上箭头
    void DrawDownArrow(wxDC* pDC);  // 绘制下箭头
    bool IsVisible() const;
    void Hide();
    void Show();
    void SetManualHeight(int height);
    int GetMaxHeight()const;
    int GetMinHeight()const;
    void SetMaxHeight(int height);
    void SetMinHeight(int height);
    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;

   protected:
    bool m_visible;         // 是否可见
    int m_x;                // 起始X坐标
    int m_y;                // 起始Y坐标
    int m_width;            // 显示宽度
    int m_height;           // 显示高度
    int m_upArrowX;         // 上箭头X坐标
    int m_upArrowY;         // 上箭头Y坐标
    int m_upArrowWidth;     // 上箭头宽度
    int m_upArrowHeight;    // 上箭头高度
    int m_downArrowX;       // 下箭头X坐标
    int m_downArrowY;       // 下箭头Y坐标
    int m_downArrowWidth;   // 下箭头宽度
    int m_downArrowHeight;  // 下箭头高度
    int m_paddingTop;       // 顶部间隙
    int m_paddingBottom;    // 底部间隙
    int m_heightManual;     // 用户指定的高度，程序不自适应
    int m_minHeight;        // 最小高度
    int m_maxHeight;        // 最大高度
}