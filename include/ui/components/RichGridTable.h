#ifndef RICHGRIDTABLE_H
#define RICHGRIDTABLE_H

#include <wx/grid.h>
#include "stock/StockDataStorage.h"

enum class RichGridTableDataType { Stock = 0 };

class RichGridTable : public wxGridTableBase {
   public:
    RichGridTable(RichGridTableDataType type, StockDataStorage* pStorage);
    virtual ~RichGridTable();
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual wxString GetValue(int row, int col);
    virtual wxString GetColLabelValue(int col);
    virtual void SetValue(int row, int col, const wxString& value);
    void SetStorage(StockDataStorage* pStorage);
    void SortColumn(int iCol);

   protected:
    RichGridTableDataType m_dataType;
    StockDataStorage* m_pStorage;
    wxColor m_clrGreen;
    wxColor m_clrRed;
};

#endif