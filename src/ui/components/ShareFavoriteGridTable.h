#ifndef SHARE_FAVORITE_GRID_TABLE_H
#define SHARE_FAVORITE_GRID_TABLE_H

#include <wx/grid.h>
#include "stock/StockDataStorage.h"
#include "stock/UserFavoriteShare.h"

class ShareFavoriteGridTable : public wxGridTableBase {
   public:
    ShareFavoriteGridTable(StockDataStorage* pStorage);
    virtual ~ShareFavoriteGridTable();
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual void SetValue(int row, int col, const wxString& value);
    virtual wxString GetValue(int row, int col);
    void SetStorage(StockDataStorage* pStorage);

   protected:
    StockDataStorage* m_pStorage;
    UserFavoriteShare* m_pUserFavoriteShare;
};

#endif