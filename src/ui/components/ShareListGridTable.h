#ifndef SHARELISTGRIDTABLE_H
#define SHARELISTGRIDTABLE_H

#include <wx/grid.h>
#include "stock/StockDataStorage.h"

class ShareListGridTable : public wxGridTableBase {
   public:
    ShareListGridTable(StockDataStorage* pStorage);
    virtual ~ShareListGridTable();
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual void SetValue(int row, int col, const wxString& value);
    virtual wxString GetValue(int row, int col);
    void SetStorage(StockDataStorage* pStorage);
    void SetSearchKeyword(const std::string& keyword);

   protected:
    StockDataStorage* m_pStorage;
    std::vector<Share*> m_shareList;
};

#endif