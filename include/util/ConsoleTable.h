#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Color.h"
#include "Global.h"

class ConsoleTable {
   public:
    typedef enum _align {
        left,   // 自适应模式
        right,  // 固定模式
    } Align;
    typedef enum _valign {
        top,     // 靠上对齐，
        middle,  // 垂直居中
        bottom   // 底部对齐
    } Valign;
    typedef enum _layout {
        adaptive,  // 自适应模式
        fixed,     // 固定模式
    } Layout;
    typedef enum _color { red, green, blue, white, yellow, cyan, magenta } Color;
    ConsoleTable(std::vector<std::vector<std::string>>& data);
    ConsoleTable() = default;
    ~ConsoleTable() = default;
    ConsoleTable& setMaxColumnWidth(size_t max_column_width);
    ConsoleTable& setPadding(size_t padding);
    ConsoleTable& setAlign(ConsoleTable::Align align);
    ConsoleTable& setValign(ConsoleTable::Valign valign);
    ConsoleTable& setLayout(ConsoleTable::Layout layout);
    ConsoleTable& setColumnWidth(uint16_t column_index, size_t width);
    ConsoleTable& setColumnColor(uint16_t column_index, ConsoleTable::Color color);
    ConsoleTable::Color getColumnColor(uint16_t column_index);
    size_t getColumnWidth(uint16_t column_index);
    ConsoleTable::Align getAlign() const;
    ConsoleTable::Valign getValign() const;
    ConsoleTable::Layout getLayout() const;
    size_t getMaxColumnWidth() const;
    size_t getPadding() const;
    void print();
    static void print(std::vector<std::vector<std::string>>& data);

   protected:
    typedef struct _cell {
        std::vector<std::string> lines;  // 每个单元格显示的多行文本
        size_t col_width;                // 显示的列宽(字符数)
    } Cell;

    typedef struct table {
        std::vector<std::vector<std::string>> cells;
        size_t nrows = 0;                      // 总行数
        size_t ncols = 0;                      // 总列数
        size_t padding = 0;                    // 表格内间距
        size_t col_total_width = 0;            // 所有列总宽度
        std::vector<size_t> col_widths;        // 每列显示宽度
        std::vector<size_t> expanded_heights;  // 膨胀后的行高度数组，前缀和数组

    } Table;

    void layoutTable();
    void printTable();
    void printTableBorder();
    bool needPrintTableBoder(size_t row_index);
    std::string getCellColorText(std::string text, uint16_t column_index);
    ConsoleTable::Cell layoutCell(std::string& str, uint16_t column_index);
    std::vector<std::string> fillCell(ConsoleTable::Cell& cell,
                                      size_t width,
                                      size_t nRows);

    std::vector<std::vector<std::string>> m_data;  // 用户传递过来的表格数据
    Table m_table;  // 已经格式化好的表格内容，可以直接控制台打印输出
    size_t m_padding;        // 表格内间距
    size_t m_max_col_width;  // 自适应布局，每列最大列宽
    std::unordered_map<uint16_t, size_t>
        m_fixed_col_width;  // 固定列布局，每列最大显示列宽
    std::unordered_map<uint16_t, ConsoleTable::Color>
        m_column_colors;  // 每列对应输出文字颜色
    Align m_align;        // 文字左右对齐
    Valign m_valign;      // 文字垂直对齐
    Layout m_layout;      // 表格布局方式
};