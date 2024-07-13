///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/ui/commponents/RichDialogShareSearch.cpp
// Purpose:     iwealth global share search dialog
// Author:      songhuabiao
// Modified by:
// Created:     2024-06-13 18:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/components/RichDialogShareSearch.h"

#include <wx/intl.h>
#include <wx/string.h>
#include <chrono>
#include "stock/StockDataStorage.h"
#include "ui/RichApplication.h"
#include "ui/RichHelper.h"
#include "ui/components/RichGridCellStringRenderer.h"
#include "ui/components/ShareListGridCellStringRenderer.h"
#include "ui/components/ShareListGridTable.h"

const long RichDialogShareSearch::ID_TEXTCTRL_KEYWORD = wxNewId();
const long RichDialogShareSearch::ID_RICHGRID_SHARELIST = wxNewId();
const long RichDialogShareSearch::ID_STATICTEXT_TITLE = wxNewId();
const long RichDialogShareSearch::ID_BITMAPBUTTON_CLOSE = wxNewId();

BEGIN_EVENT_TABLE(RichDialogShareSearch, wxDialog)

END_EVENT_TABLE()

RichDialogShareSearch::RichDialogShareSearch(wxWindow* parent,
                                             wxWindowID id,
                                             const wxString& title,
                                             const wxPoint& pos,
                                             const wxSize& size,
                                             long style,
                                             const wxString& name) {
    Create(parent, id, title, pos, size, style, name);

    // 自定义标题栏标题
    m_statictext_title = new wxStaticText(this, ID_STATICTEXT_TITLE, _T("按键精灵"), wxPoint(5, 5), wxDefaultSize);
    m_statictext_title->SetFont(RichApplication::GetDefaultFont(12));
    // 自定义右侧关闭按钮
    m_bitmapbutton_close = wxBitmapButton::NewCloseButton(this, ID_BITMAPBUTTON_CLOSE);
    m_bitmapbutton_close->SetPosition(wxPoint(255, 5));
    m_bitmapbutton_close->Bind(wxEVT_BUTTON, &RichDialogShareSearch::OnExitSearchShare, this);

    // 关键字搜索文本框
    m_textCtrlKeyword =
        new wxTextCtrl(this, ID_TEXTCTRL_KEYWORD, "", wxPoint(5, 32), wxSize(200, 32), wxTE_PROCESS_ENTER);
    m_textCtrlKeyword->Bind(wxEVT_TEXT, &RichDialogShareSearch::OnSearchShare, this);
    m_textCtrlKeyword->Bind(wxEVT_TEXT_ENTER, &RichDialogShareSearch::OnExitSearchShare, this);
    m_textCtrlKeyword->Bind(wxEVT_KEY_DOWN, &RichDialogShareSearch::OnKeyDown, this);
    m_textCtrlKeyword->SetFont(RichApplication::GetDefaultFont(11));

    /////////////////////////////////////////////////
    // 股票列表组件
    m_gridShareList = new RichGrid(this, ID_RICHGRID_SHARELIST, wxPoint(5, 75), wxSize(200, 148));
    // 必须优先设置Table，否则设置wxGrid属性会失败！
    m_gridShareList->SetTable(CreateShareListGridTable());
    m_gridShareList->SetFont(RichApplication::GetDefaultFont(12));
    m_gridShareList->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
    m_gridShareList->SetDefaultCellFont(RichApplication::GetDefaultFont());
    m_gridShareList->SetDefaultRenderer(new ShareListGridCellStringRenderer());  // 采用默认的单元格渲染器
    m_gridShareList->SetDefaultRowSize(25);                                      // 设置默认行高

    // 颜色设置
    wxColor background_clr(255, 255, 255);
    m_gridShareList->SetDefaultCellBackgroundColour(background_clr);
    m_gridShareList->SetDefaultCellTextColour(wxColour(0, 0, 0));
    m_gridShareList->SetGridLineColour(background_clr);
    m_gridShareList->SetSelectionBackground(wxColor(209, 201, 240));
    m_gridShareList->SetSelectionForeground(wxColor(52, 52, 227));
    m_gridShareList->SetSelectionMode(wxGrid::wxGridSelectRows);  // 只支持行选择
    m_gridShareList->SetCellHighlightPenWidth(0);  // 单元格不高亮,必须设置，否则行选择效果不对

    m_gridShareList->DisableDragRowSize();    // 禁止拖拽改变行高
    m_gridShareList->EnableEditing(false);    // 禁止编辑
    m_gridShareList->EnableGridLines(false);  // 不显示网格线

    // m_gridShareList->SetCellHighlightColour(wxColor(52, 52, 227));
    // m_gridShareList->SetCellHighlightPenWidth(0);

    m_gridShareList->HideColLabels();  // 隐藏列标签
    m_gridShareList->HideRowLabels();  // 隐藏行标签
    m_gridShareList->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &RichDialogShareSearch::OnGridCellLeftClick, this);

    // 设置列宽
    std::vector<int> grid_column_widths = {60, 160, 120};
    for (size_t i = 0; i < grid_column_widths.size(); i++) {
        m_gridShareList->SetColSize(i, grid_column_widths[i]);
    }
    /////////////////////////////////////////////////

    m_oldTimeKeywordInput = std::chrono::steady_clock::now();
    m_nowTimeKeywordInput = std::chrono::steady_clock::now();
}

ShareListGridTable* RichDialogShareSearch::CreateShareListGridTable() {
    StockDataStorage* pStorage = static_cast<RichApplication*>(wxTheApp)->GetStockDataStorage();
    return new ShareListGridTable(pStorage);
}

void RichDialogShareSearch::OnGridCellLeftClick(wxGridEvent& event) {
    int iSelectedRow = event.GetRow();
    m_gridShareList->SelectRow(iSelectedRow);
    m_gridShareList->MakeCellVisible(iSelectedRow, 0);
    m_gridShareList->Refresh();
}

void RichDialogShareSearch::ReLayout(const wxSize& size) {
    this->SetSize(size);
    m_bitmapbutton_close->SetPosition(wxPoint(size.x - 20, 5));
    wxSize size_textctrl;
    size_textctrl.SetWidth(size.GetWidth() - 10);
    size_textctrl.SetHeight(32);
    m_textCtrlKeyword->SetSize(size_textctrl);

    wxSize size_gridctrl;
    size_gridctrl.SetWidth(size.GetWidth() - 10);
    size_gridctrl.SetHeight(300);
    m_gridShareList->SetSize(size_gridctrl);
}

void RichDialogShareSearch::OnKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_UP) {  // 向上方向键
        m_gridShareList->MoveSelectedListItem(-1);
    } else if (event.GetKeyCode() == WXK_DOWN) {  // 向下方向键
        m_gridShareList->MoveSelectedListItem(1);
    } else {
        event.Skip();  // 其他按键按默认方式处理
    }
}

void RichDialogShareSearch::SetKeyword(const std::string& keyword) {
    // 通过判断调用时间来决定是否要拼接keyword,优化中文输入法导致的无法完整获取用户输入的中文字符串的问题
    m_nowTimeKeywordInput = std::chrono::steady_clock::now();
    std::chrono::milliseconds time_elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(m_nowTimeKeywordInput - m_oldTimeKeywordInput);
    static std::string cache_keyword = "";
    if (time_elapsed.count() < 200) {  // 不能设置太小，否则无法识别到
        cache_keyword += keyword;
    } else {
        cache_keyword = keyword;
    }
    m_textCtrlKeyword->SetValue(CN(cache_keyword));  // 需要转换成UTF8字符串才能工作
    m_textCtrlKeyword->SetFocus();
    m_textCtrlKeyword->SetInsertionPointEnd();  // 移动光标到输入字符串末尾
    Raise();                                    // 将搜索窗口置于最顶部
    m_oldTimeKeywordInput = m_nowTimeKeywordInput;
}

void RichDialogShareSearch::OnSearchShare(wxCommandEvent& event) {
    wxString input = m_textCtrlKeyword->GetValue();
    std::string keyword = input.utf8_string();  // Trie只支持utf8字符串
    // 必须先设置 Keyword，然后调用SetTable，否则 表格行数为0，无法显示搜索结果
    ShareListGridTable* pGridTable = CreateShareListGridTable();
    pGridTable->SetSearchKeyword(keyword);
    m_gridShareList->SetTable(pGridTable);
    m_gridShareList->SelectRow(0);  // 选中第一行
    m_gridShareList->Refresh();
    event.Skip();
}

void RichDialogShareSearch::OnExitSearchShare(wxCommandEvent& /*event*/) {
    m_textCtrlKeyword->Clear();
    this->Show(false);
}

RichDialogShareSearch::~RichDialogShareSearch() {
    //(*Destroy(RichDialogShareSearch)
    //*)
}
