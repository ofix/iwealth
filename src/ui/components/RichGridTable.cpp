#include "ui/components/RichGridTable.h"
#include "ui/RichHelper.h"

RichGridTable::RichGridTable(RichGridTableDataType type, StockDataStorage* pStorage)
    : wxGridTableBase(), m_dataType(type), m_pStorage(pStorage) {
    m_clrGreen = wxColor(57, 227, 101);
    m_clrRed = wxColor(255, 0, 0);
}

RichGridTable::~RichGridTable() {
}

int RichGridTable::GetNumberRows() {
    if (m_dataType == RichGridTableDataType::Stock) {
        return 30;
        // return static_cast<int>(m_pStorage->GetStockMarketShareCount());
    }
    return 0;
}

int RichGridTable::GetNumberCols() {
    if (m_dataType == RichGridTableDataType::Stock) {
        return 15;
    }
    return 0;
}

// 获取指定单元格的值
wxString RichGridTable::GetValue(int row, int col) {
    if (m_dataType == RichGridTableDataType::Stock) {
        int nTotalRows = static_cast<int>(m_pStorage->GetStockMarketShareCount());
        if (row < nTotalRows && col < 15) {
            Share share = m_pStorage->GetStockAllShares()->at(row);
            switch (col) {
                case 0:
                    return wxString(std::to_string(row + 1));  // 序号
                case 1:
                    return CN(share.code);  // 股票代码
                case 2:
                    return CN(share.name);  // 股票名称
                case 3:
                    return CN(convertDouble(share.change_rate) + '%');  // 涨幅
                case 4:
                    return CN(convertDouble(share.price_now) + '%');  // 当前价
                case 5:
                    return CN(convertDouble(share.price_yesterday_close) + '%');  // 昨天收盘价
                case 6:
                    return CN(convertDouble(share.price_open) + '%');  // 开盘价
                case 7:
                    return CN(convertDouble(share.price_max) + '%');  // 最高价
                case 8:
                    return CN(convertDouble(share.price_min) + '%');  // 最低价
                case 9:
                    return CN(std::to_string(share.amount));  // 成交额
                case 10:
                    return CN(std::to_string(share.volume));  // 成交量
                case 11:
                    return CN(convertDouble(share.turnover_rate) + '%');  // 换手率
                case 12:
                    return CN(convertDouble(share.qrr));  // 量比
                case 13:
                    return CN(share.industry_name);  // 行业
                case 14:
                    return CN(share.province);  // 区域
                default:
                    return "";
            }
        } else {
            return wxEmptyString;
        }
    }
    return wxEmptyString;
}

void RichGridTable::SetValue(int /*row*/, int /*col*/, const wxString& /*value*/) {
    /**
     * 不支持设置
     */
}

void RichGridTable::SetStorage(StockDataStorage* pStorage) {
    m_pStorage = pStorage;
}