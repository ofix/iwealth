#include "ui/components/RichGridTable.h"
#include <algorithm>
#include "ui/RichHelper.h"
#include "util/Global.h"

RichGridTable::RichGridTable(RichGridTableDataType type, StockDataStorage* pStorage)
    : wxGridTableBase(), m_dataType(type), m_pStorage(pStorage) {
    m_clrGreen = wxColor(57, 227, 101);
    m_clrRed = wxColor(255, 0, 0);
    m_colSortOrders = {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    m_colSortFixed = -1;
    m_iColSorting = 3;
}

RichGridTable::~RichGridTable() {
}

int RichGridTable::GetNumberRows() {
    if (m_dataType == RichGridTableDataType::Stock) {
        int rows = static_cast<int>(m_pStorage->GetStockMarketShareCount());
        return rows;
    }
    return 0;
}

bool RichGridTable::InsertRows(size_t pos, size_t numRows) {
    if (pos >= m_pStorage->GetStockMarketShareCount()) {
        return AppendRows(numRows);
    }
    if (GetView()) {
        wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_INSERTED, pos, numRows);

        GetView()->ProcessTableMessage(msg);
    }
    return true;
}

bool RichGridTable::AppendRows(size_t numRows) {
    wxGrid* pGrid = GetView();
    if (pGrid != NULL) {
        wxGridTableMessage changed(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, numRows);
        pGrid->ProcessTableMessage(changed);
        return true;
    }
    return false;
}

bool RichGridTable::DeleteRows(size_t pos, size_t numRows) {
    size_t curNumRows = m_pStorage->GetStockMarketShareCount();
    if (pos >= curNumRows) {
        wxFAIL_MSG(wxString::Format(wxT("Called wxGridStringTable::DeleteRows(pos=%lu, N=%lu)\nPos value is invalid "
                                        "for present table with %lu rows"),
                                    (unsigned long)pos, (unsigned long)numRows, (unsigned long)curNumRows));

        return false;
    }
    if (numRows > curNumRows - pos) {
        numRows = curNumRows - pos;
    }
    if (numRows >= curNumRows) {
        m_pStorage->ClearShares();
    } else {
        m_pStorage->DeleteShares(pos, numRows);
    }
    if (GetView()) {
        wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, numRows);
        GetView()->ProcessTableMessage(msg);
    }
    return true;
}

bool RichGridTable::SetColumnOrder(int iCol, int order) {
    if (m_dataType == RichGridTableDataType::Stock) {
        if ((size_t)iCol < m_colSortOrders.size()) {
            if (order == wxDirection::wxUP) {
                m_colSortOrders[iCol] = 0;
            } else {
                m_colSortOrders[iCol] = 1;
            }
            return true;
        }
        return false;
    }
    return false;
}

int RichGridTable::GetColumnOrder(int iCol) {
    if (m_dataType == RichGridTableDataType::Stock) {
        return m_colSortOrders[iCol];
    }
    return 0;
}

bool RichGridTable::IsSortingColumn(int iCol) {
    return m_iColSorting == iCol;
}

// 支持多列排序
bool RichGridTable::IsFixedSortColumn(int iCol) {
    return m_colSortFixed == iCol;
}

int RichGridTable::GetFixedSortColumn() const {
    return m_colSortFixed;
}

bool RichGridTable::SetSortColumn(int iCol) {
    if (m_dataType == RichGridTableDataType::Stock) {
        if (iCol < 16) {
            m_iColSorting = iCol;
            m_colSortOrders[iCol] = ((~m_colSortOrders[iCol]) & 0x01);
            return true;
        }
        return false;
    }
    return false;
}

bool RichGridTable::SetFixedSortColumn(int iCol) {
    if (m_dataType == RichGridTableDataType::Stock) {
        if (iCol < 16) {
            if (m_colSortFixed == iCol) {
                m_colSortFixed = -1;  // 允许取消固定列，双击同一个固定列取消
            } else {
                m_colSortFixed = iCol;  // 如果设置的固定列没有设置，或者设置的不一样
            }
            m_colSortOrders[iCol] = ((~m_colSortOrders[iCol]) & 0x01);
            return true;
        }
        return false;
    }
    return false;
}

// 支持多字段分组排序
int RichGridTable::SortComparator(const Share& a, const Share& b, int iCol, int order) {
    if (iCol == 1) {
        if (order == 0) {  // 股票代码升序
            if (a.code < b.code) {
                return 1;
            } else if (a.code == b.code) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票代码降序
            if (a.code > b.code) {
                return 1;
            } else if (a.code == b.code) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 2) {
        if (order == 0) {  // 股票名称升序
            if (a.name < b.name) {
                return 1;
            } else if (a.name == b.name) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票名称降序
            if (a.name > b.name) {
                return 1;
            } else if (a.name == b.name) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 3) {
        if (order == 0) {  // 股票涨幅升序
            if (a.change_rate < b.change_rate) {
                return 1;
            } else if (a.change_rate == b.change_rate) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票涨幅降序
            if (a.change_rate > b.change_rate) {
                return 1;
            } else if (a.change_rate == b.change_rate) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 4) {
        if (order == 0) {  // 股票现价升序
            if (a.price_now < b.price_now) {
                return 1;
            } else if (a.price_now == b.price_now) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票现价降序
            if (a.price_now > b.price_now) {
                return 1;
            } else if (a.price_now == b.price_now) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 5) {
        if (order == 0) {  // 股票昨天收盘价升序
            if (a.price_yesterday_close < b.price_yesterday_close) {
                return 1;
            } else if (a.price_yesterday_close == b.price_yesterday_close) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票收盘价降序
            if (a.price_yesterday_close > b.price_yesterday_close) {
                return 1;
            } else if (a.price_yesterday_close == b.price_yesterday_close) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 6) {
        if (order == 0) {  // 股票开盘价升序
            if (a.price_open < b.price_open) {
                return 1;
            } else if (a.price_open == b.price_open) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票开盘价降序
            if (a.price_open > b.price_open) {
                return 1;
            } else if (a.price_open == b.price_open) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 7) {
        if (order == 0) {  // 股票最高价升序
            if (a.price_max < b.price_max) {
                return 1;
            } else if (a.price_max == b.price_max) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票最高价降序
            if (a.price_max > b.price_max) {
                return 1;
            } else if (a.price_max == b.price_max) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 8) {
        if (order == 0) {  // 股票最低价升序
            if (a.price_min < b.price_min) {
                return 1;
            } else if (a.price_min == b.price_min) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票最低价降序
            if (a.price_min > b.price_min) {
                return 1;
            } else if (a.price_min == b.price_min) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 9) {
        if (order == 0) {  // 股票成交额升序
            if (a.amount < b.amount) {
                return 1;
            } else if (a.amount == b.amount) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票成交额降序
            if (a.amount > b.amount) {
                return 1;
            } else if (a.amount == b.amount) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 10) {
        if (order == 0) {  // 股票成交量升序
            if (a.volume < b.volume) {
                return 1;
            } else if (a.volume == b.volume) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票成交量降序
            if (a.volume > b.volume) {
                return 1;
            } else if (a.volume == b.volume) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 11) {
        if (order == 0) {  // 股票换手率升序
            if (a.turnover_rate < b.turnover_rate) {
                return 1;
            } else if (a.turnover_rate == b.turnover_rate) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票换手率降序
            if (a.turnover_rate > b.turnover_rate) {
                return 1;
            } else if (a.turnover_rate == b.turnover_rate) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 13) {
        if (order == 0) {  // 股票量比升序
            if (a.qrr < b.qrr) {
                return 1;
            } else if (a.qrr == b.qrr) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票量比降序
            if (a.qrr > b.qrr) {
                return 1;
            } else if (a.qrr == b.qrr) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 14) {
        if (order == 0) {  // 股票行业升序
            if (a.industry_name < b.industry_name) {
                return 1;
            } else if (a.industry_name == b.industry_name) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票行业降序
            if (a.industry_name > b.industry_name) {
                return 1;
            } else if (a.industry_name == b.industry_name) {
                return 0;
            }
            return -1;
        }
    } else if (iCol == 15) {
        if (order == 0) {  // 股票省份升序
            if (a.province < b.province) {
                return 1;
            } else if (a.province == b.province) {
                return 0;
            }
            return -1;
        } else if (order == 1) {  // 股票省份降序
            if (a.province > b.province) {
                return 1;
            } else if (a.province == b.province) {
                return 0;
            }
            return -1;
        }
    }
    return -1;
}

bool RichGridTable::SortMultiColumns() {
    if (m_dataType == RichGridTableDataType::Stock) {
        std::vector<Share>* pShares = m_pStorage->GetStockAllShares();
        int order = m_colSortOrders[m_iColSorting];
        std::stable_sort(pShares->begin(), pShares->end(), [this, order](const Share& a, const Share& b) {
            if (m_colSortFixed != -1) {
                int orderFixed = m_colSortOrders[m_colSortFixed];
                if (SortComparator(a, b, m_colSortFixed, orderFixed) == 1) {
                    return true;
                } else if (SortComparator(a, b, m_colSortFixed, orderFixed) == 0) {
                    if (SortComparator(a, b, m_iColSorting, order) == 1) {
                        return true;
                    }
                    return false;
                }
            } else {
                if (SortComparator(a, b, m_iColSorting, order) == 1) {
                    return true;
                }
                return false;
            }
            return false;
        });
        return true;
    }
    return false;
}

int RichGridTable::GetNumberCols() {
    if (m_dataType == RichGridTableDataType::Stock) {
        return 16;
    }
    return 0;
}

wxString RichGridTable::GetColLabelValue(int col) {
    bool bShow = false;
    if (bShow && m_dataType == RichGridTableDataType::Stock) {
        static std::vector<wxString> columns = {
            CN("序号"), CN("代码"),   CN("名称"),   CN("涨幅"), CN("现价"), CN("昨收"), CN("今开"), CN("最高"),
            CN("最低"), CN("成交额"), CN("成交量"), CN("换手"), CN("振幅"), CN("量比"), CN("行业"), CN("省份"),
        };
        if ((size_t)col < columns.size()) {
            return columns[col];
        }
    }
    return "";
}

// 获取指定单元格的值
wxString RichGridTable::GetValue(int row, int col) {
    if (m_dataType == RichGridTableDataType::Stock) {
        int nTotalRows = static_cast<int>(m_pStorage->GetStockMarketShareCount());
        if (row < nTotalRows && col < 16) {
            Share share = m_pStorage->GetStockAllShares()->at(row);
            switch (col) {
                case 0:
                    return wxString(std::to_string(row + 1));  // 序号
                case 1:
                    return CN(share.code);  // 股票代码
                case 2:
                    return CN(share.name);  // 股票名称
                case 3:
                    return CN(convert_double(share.change_rate) + '%');  // 涨幅
                case 4:
                    return CN(convert_double(share.price_now));  // 当前价
                case 5:
                    return CN(convert_double(share.price_yesterday_close));  // 昨天收盘价
                case 6:
                    return CN(convert_double(share.price_open));  // 开盘价
                case 7:
                    return CN(convert_double(share.price_max));  // 最高价
                case 8:
                    return CN(convert_double(share.price_min));  // 最低价
                case 9:
                    return RichUnit(share.amount);  // 成交额
                case 10:
                    return RichUnit(share.volume);  // 成交量
                case 11:
                    return CN(convert_double(share.turnover_rate) + '%');  // 换手率
                case 12:
                    return CN(convert_double(share.price_amplitude) + '%');  // 振幅
                case 13:
                    return CN(convert_double(share.qrr));  // 量比
                case 14:
                    return CN(share.industry_name);  // 行业
                case 15:
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