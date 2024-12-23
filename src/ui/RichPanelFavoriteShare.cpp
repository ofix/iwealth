///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/RichPanelFavoriteShare.cpp
// Purpose:     user favorite share
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-23 20:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichPanelFavoriteShare.h"
#include <wx/dcbuffer.h>
#include "ui/RichHelper.h"
#include "ui/components/ShareListGridCellStringRenderer.h"
#include "ui/components/ShareFavoriteGridTable.h"

RichPanelFavoriteShare::RichPanelFavoriteShare(StockDataStorage* pStorage,
                                               wxWindow* parent,
                                               wxWindowID id,
                                               const wxPoint& pos,
                                               const wxSize& size)
    : RichPanel(PanelType::Favorite, pStorage, parent, id, pos, size) {
    // 股票列表组件
    m_gridFavorite = new RichGrid(this, wxID_ANY, wxPoint(0, 0), size);
    // 必须优先设置Table，否则设置wxGrid属性会失败！
    m_gridFavorite->SetTable(new ShareFavoriteGridTable(pStorage));
    m_gridFavorite->SetFont(RichHelper::GetDefaultFont(12));
    m_gridFavorite->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
    m_gridFavorite->SetDefaultCellFont(RichHelper::GetDefaultFont());
    m_gridFavorite->SetDefaultRenderer(new ShareListGridCellStringRenderer());  // 采用默认的单元格渲染器
    m_gridFavorite->SetDefaultRowSize(25);                                      // 设置默认行高

    // 颜色设置
    wxColor background_clr(255, 255, 255);
    m_gridFavorite->SetDefaultCellBackgroundColour(background_clr);
    m_gridFavorite->SetDefaultCellTextColour(wxColour(0, 0, 0));
    m_gridFavorite->SetGridLineColour(background_clr);
    m_gridFavorite->SetSelectionBackground(wxColor(209, 201, 240));
    m_gridFavorite->SetSelectionForeground(wxColor(52, 52, 227));
    m_gridFavorite->SetSelectionMode(wxGrid::wxGridSelectRows);  // 只支持行选择
    m_gridFavorite->SetCellHighlightPenWidth(0);  // 单元格不高亮,必须设置，否则行选择效果不对

    m_gridFavorite->DisableDragRowSize();    // 禁止拖拽改变行高
    m_gridFavorite->EnableEditing(false);    // 禁止编辑
    m_gridFavorite->EnableGridLines(false);  // 不显示网格线

    // m_gridFavorite->SetCellHighlightColour(wxColor(52, 52, 227));
    // m_gridFavorite->SetCellHighlightPenWidth(0);

    m_gridFavorite->HideColLabels();  // 隐藏列标签
    m_gridFavorite->HideRowLabels();  // 隐藏行标签
    // m_gridFavorite->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &RichDialogShareSearch::OnGridCellLeftClick, this);
}

RichPanelFavoriteShare::~RichPanelFavoriteShare() {
}