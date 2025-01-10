#ifndef RICH_DIRECT_UI_CTRL_H
#define RICH_DIRECT_UI_CTRL_H

#include <wx/string.h>
#include <wx/wx.h>
#include <wx/gdicmn.h>
#include <vector>

class RichDirectUICtrl {
   public:
    RichDirectUICtrl(RichDirectUICtrl* parent,
                     wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     const wxString& name = wxPanelNameStr);
    virtual ~RichDirectUICtrl();
    void AddChild(RichDirectUICtrl* child);
    void SetParent(RichDirectUICtrl* parent);
    void SetPosition(int x, int y);
    void SetSize(int width, int height);
    virtual void Layout();
    virtual void Draw(wxDC* pDC);
    virtual bool onMouseEvent(wxMouseEvent& event);

   protected:
    std::vector<RichDirectUICtrl*> m_children;
    RichDirectUICtrl* m_parent;
    wxWindowID m_id;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    wxString m_name;
};

#endif