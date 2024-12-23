///////////////////////////////////////////////////////////////////////////////
// Name:        src/ui/commponents/ShareFavoriteGridTable.cpp
// Purpose:     favorite shares' grid table
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-23 18:40
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/ShareFavoriteGridTable.h"
#include <algorithm>
#include "ui/RichHelper.h"

ShareFavoriteGridTable::ShareFavoriteGridTable(StockDataStorage* pStorage) : wxGridTableBase(), m_pStorage(pStorage) {
    m_pUserFavoriteShare = new UserFavoriteShare(pStorage);
    m_pUserFavoriteShare->Load();  // 加载本地自选股数据
}

ShareFavoriteGridTable::~ShareFavoriteGridTable() {
}

int ShareFavoriteGridTable::GetNumberRows() {
    if (m_pUserFavoriteShare) {
        return m_pUserFavoriteShare->GetCurrentGroupSize();
    }
    return 0;
}

int ShareFavoriteGridTable::GetNumberCols() {
    return 5;  // 股票代码,股票名称，当前涨幅，自选日期，自选涨幅
}

void ShareFavoriteGridTable::SetValue(int /*row*/, int /*col*/, const wxString& /*value*/) {
    /**
     * 不支持设置
     */
}

wxString ShareFavoriteGridTable::GetValue(int row, int col) {
    if ((size_t)row < m_pUserFavoriteShare->GetCurrentGroupSize()) {
        std::vector<FavoriteShare> shares = m_pUserFavoriteShare->GetCurrentGroupShares();
        FavoriteShare favorite_share = shares[row];
        if (col == 0) {  //自选股代号
            return CN(favorite_share.pShare->code);
        } else if (col == 1) {  // 自选股名称
            return CN(favorite_share.pShare->name);
        } else if (col == 2) {  // 当前涨幅
            return CN(convert_double(favorite_share.pShare->change_rate) + "%");
        } else if (col == 3) {  // 自选日期
            return favorite_share.favoriate_date;
        } else if (col == 4) {  // 自选涨幅
            return CN(convert_double(favorite_share.total_change_rate) + "%");
        }
    }
    return "";
}

void ShareFavoriteGridTable::SetStorage(StockDataStorage* pStorage) {
    m_pStorage = pStorage;
}