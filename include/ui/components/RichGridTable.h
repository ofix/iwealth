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
    bool SortMultiColumns();
    virtual int GetColumnOrder(int iCol);
    bool SetColumnOrder(int iCol, int order);

    bool SetSortColumn(int iCol);
    bool IsSortingColumn(int iCol);
    // 固定列
    bool IsFixedSortColumn(int iCol);
    int GetFixedSortColumn() const;
    bool SetFixedSortColumn(int iCol);
    int SortComparator(const Share& a, const Share& b, int iCol, int order);

   protected:
    RichGridTableDataType m_dataType;
    StockDataStorage* m_pStorage;
    wxColor m_clrGreen;
    wxColor m_clrRed;
    std::vector<int> m_colSortOrders;
    int m_colSortFixed;
    int m_iColSorting;  // 当前正在排序的列
};

#endif