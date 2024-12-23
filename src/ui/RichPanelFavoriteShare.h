#ifndef RICH_PANEL_FAVORITE_SHARE_H
#define RICH_PANEL_FAVORITE_SHARE_H

#include <wx/grid.h>
#include <wx/wx.h>
#include "ui/components/RichPanel.h"
#include "ui/components/RichGrid.h"

class RichPanelFavoriteShare : public RichPanel {
   public:
    RichPanelFavoriteShare(StockDataStorage* pStorage,
                           wxWindow* parent,
                           wxWindowID id = wxID_ANY,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize);
    virtual ~RichPanelFavoriteShare();

   protected:
    RichGrid* m_gridFavorite;
};

#endif