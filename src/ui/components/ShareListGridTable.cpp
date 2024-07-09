#include "ui/components/ShareListGridTable.h"
#include <algorithm>
#include "ui/RichHelper.h"

ShareListGridTable::ShareListGridTable(StockDataStorage* pStorage) : wxGridTableBase(), m_pStorage(pStorage) {
}
ShareListGridTable::~ShareListGridTable() {
}
int ShareListGridTable::GetNumberRows() {
    size_t size = m_shareList.size();
    return size;
}
int ShareListGridTable::GetNumberCols() {
    return 3;  // 股票代码,股票名称，股票市场
}

void ShareListGridTable::SetValue(int /*row*/, int /*col*/, const wxString& /*value*/) {
    /**
     * 不支持设置
     */
}

void ShareListGridTable::SetSearchKeyword(const std::string& keyword) {
    m_shareList = m_pStorage->SearchShares(keyword);
}

wxString ShareListGridTable::GetValue(int row, int col) {
    if ((size_t)row < m_shareList.size()) {
        if (col == 0) {
            return CN(m_shareList[row]->code);
        } else if (col == 1) {
            return CN(m_shareList[row]->name);
        } else if (col == 2) {
            return CN(MARKET_NAME(m_shareList[row]->market));
        }
    }
    return "";
}

void ShareListGridTable::SetStorage(StockDataStorage* pStorage) {
    m_pStorage = pStorage;
}