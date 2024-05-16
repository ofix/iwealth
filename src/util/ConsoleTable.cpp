#include "util/ConsoleTable.h"

ConsoleTable::ConsoleTable(std::vector<std::vector<std::string>>& data)
    : m_data(data),
      m_padding(1),
      m_max_col_width(60),
      m_align(ConsoleTable::Align::left),
      m_valign(ConsoleTable::Valign::middle),
      m_layout(ConsoleTable::Layout::adaptive) {}
ConsoleTable& ConsoleTable::setMaxColumnWidth(size_t max_column_width) {
    m_max_col_width = max_column_width;
    return *this;
}
ConsoleTable& ConsoleTable::setPadding(size_t padding) {
    m_padding = padding;
    return *this;
}

size_t ConsoleTable::getMaxColumnWidth() const {
    return m_max_col_width;
}

size_t ConsoleTable::getPadding() const {
    return m_padding;
}

ConsoleTable::Align ConsoleTable::getAlign() const {
    return m_align;
}

ConsoleTable::Valign ConsoleTable::getValign() const {
    return m_valign;
}

ConsoleTable::Layout ConsoleTable::getLayout() const {
    return m_layout;
}

ConsoleTable& ConsoleTable::setAlign(ConsoleTable::Align align) {
    m_align = align;
    return *this;
}

ConsoleTable& ConsoleTable::setValign(ConsoleTable::Valign valign) {
    m_valign = valign;
    return *this;
}

ConsoleTable& ConsoleTable::setLayout(ConsoleTable::Layout layout) {
    m_layout = layout;
    return *this;
}

ConsoleTable& ConsoleTable::setColumnWidth(uint16_t column_index, size_t width) {
    m_fixed_col_width.insert({column_index, width});
    m_layout = ConsoleTable::Layout::fixed;
    return *this;
}

size_t ConsoleTable::getColumnWidth(uint16_t column_index) {
    auto it = m_fixed_col_width.find(column_index);
    if (it != m_fixed_col_width.end()) {
        return it->second;
    }
    return m_max_col_width;
}

ConsoleTable& ConsoleTable::setColumnColor(uint16_t column_index,
                                           ConsoleTable::Color color) {
    m_column_colors.insert({column_index, color});
    return *this;
}

ConsoleTable::Color ConsoleTable::getColumnColor(uint16_t column_index) {
    auto it = m_column_colors.find(column_index);
    if (it != m_column_colors.end()) {
        return it->second;
    }
    return ConsoleTable::Color::white;
}

std::string ConsoleTable::getCellColorText(std::string text, uint16_t column_index) {
    ConsoleTable::Color clr = getColumnColor(column_index);
    if (clr == ConsoleTable::Color::white) {
        return FWHITE + text + RST;
    } else if (clr == ConsoleTable::Color::green) {
        return FGREEN + text + RST;
    } else if (clr == ConsoleTable::Color::red) {
        return FRED + text + RST;
    } else if (clr == ConsoleTable::Color::yellow) {
        return FYELLOW + text + RST;
    } else if (clr == ConsoleTable::Color::cyan) {
        return FCYAN + text + RST;
    } else if (clr == ConsoleTable::Color::magenta) {
        return FMAGENTA + text + RST;
    }
    return text;
}

void ConsoleTable::print() {
    if (m_data.size() == 0) {
        return;
    }
    layoutTable();
    printTable();
}

void ConsoleTable::print(std::vector<std::vector<std::string>>& data) {
    ConsoleTable table{data};
    table.setColumnWidth(0, 10);
    table.setColumnWidth(1, 60);
    table.setColumnColor(0, Color::yellow);
    table.setColumnColor(1, Color::cyan);
    table.print();
}

void ConsoleTable::printTable() {
    printTableBorder();
    for (size_t i = 0; i < m_table.cells.size(); i++) {
        for (size_t j = 0; j < m_table.cells[i].size(); j++) {
            std::string space = repeat_chars(' ', m_padding);
            std::string cell_text = space + m_table.cells[i][j] + space;
            std::cout << '|' << getCellColorText(cell_text, j);
        }
        std::cout << "|\n";
        if (needPrintTableBoder(i)) {
            printTableBorder();
        }
    }
}

bool ConsoleTable::needPrintTableBoder(size_t row_index) {
    size_t i = 0;

    for (; i < m_table.expanded_heights.size() &&
           (row_index + 1) > m_table.expanded_heights[i];
         i++)
        ;
    if (m_table.expanded_heights[i] == (row_index + 1)) {
        return true;
    }
    return false;
}

/**
 * @todo 将每行文本转换成多行表格,行数会增加，列宽会相应变化
 * @code 代码算法步骤
 *       步骤1: 根据最大列宽设置，将每个单元格的文本进行换行处理
 *       步骤2: 统计一行中所有单元格换行处理后实际占用的行数，行数有可能增加
 *       步骤3: 对每个单元格进行上下左右对齐和空格填充
 *       步骤4: 记录实际每行的分割位置(前缀和统计)，方便打印输出的时候快速判断
 *       步骤5: 输出表格内容
 * @author songhuabiao@greatwall.com.cn
 */
void ConsoleTable::layoutTable() {
    std::vector<std::vector<Cell>> table;
    for (std::vector<std::string> row : m_data) {
        std::vector<Cell> table_row = {};
        for (size_t i = 0; i < row.size(); i++) {
            std::string cell_text = row[i];
            Cell cell = layoutCell(cell_text, i);
            table_row.push_back(cell);
        }
        table.push_back(table_row);
    }

    // 计算每列显示最大列宽(字符数)
    for (size_t i = 0; i < m_data[0].size(); i++) {
        m_table.col_widths.push_back(0);
    }

    for (size_t i = 0; i < table.size(); i++) {
        for (size_t j = 0; j < table[i].size(); j++) {
            Cell cell = table[i][j];
            if (cell.col_width > m_table.col_widths[j]) {
                m_table.col_widths[j] = cell.col_width;
            }
        }
    }
    m_table.padding = m_padding;
    m_table.col_total_width = 0;
    for (size_t i = 0; i < m_table.col_widths.size(); i++) {
        m_table.col_total_width += m_table.col_widths[i] + m_padding;
    }

    // 计算表格每一行显示的最大高度(行数)
    std::vector<size_t> row_heights = {};
    for (size_t i = 0; i < table.size(); i++) {
        row_heights.push_back(0);
    }

    for (size_t i = 0; i < table.size(); i++) {
        for (size_t j = 0; j < table[i].size(); j++) {
            Cell cell = table[i][j];
            if (cell.lines.size() > row_heights[i]) {
                row_heights[i] = cell.lines.size();
            }
        }
    }

    // 根据总行数初始化表格，行数会膨胀，列数保持不变
    for (size_t i = 0; i < row_heights.size(); i++) {
        m_table.nrows += row_heights[i];
    }
    m_table.ncols = table[0].size();
    for (size_t i = 0; i < m_table.nrows; i++) {
        std::vector<std::string> row = {};
        m_table.cells.push_back(row);
        for (size_t j = 0; j < m_table.ncols; j++) {
            m_table.cells[i].push_back("");
        }
    }

    size_t rows_count = 0;
    for (size_t i = 0; i < table.size(); i++) {
        size_t expanded_height = 0;
        for (size_t j = 0; j < table[i].size(); j++) {
            Cell cell = table[i][j];
            std::vector<std::string> format_cells =
                fillCell(cell, m_table.col_widths[j], row_heights[i]);
            for (size_t k = 0; k < format_cells.size(); k++) {
                m_table.cells[rows_count + k][j] = format_cells[k];
            }
            expanded_height = row_heights[i];
        }
        rows_count += expanded_height;
        m_table.expanded_heights.push_back(rows_count);
        expanded_height = 0;
    }
}

/**
 * @todo 打印表格下边框
 * @author songhuabiao@greatwall.com.cn
 */
void ConsoleTable::printTableBorder() {
    std::cout << repeat_chars('-', m_table.col_total_width +
                                       m_table.col_widths.size() * m_table.padding + 3)
              << "\n";
}

/**
 * @todo 将字符串分割成多行文本，并限制每行最大字符数 不超过 max_col_size
 * @param str 原始字符串，可能包含换行符
 * @param column_index 列序号
 * @author songhuabiao@greatwall.com.cn
 */
ConsoleTable::Cell ConsoleTable::layoutCell(std::string& str, uint16_t column_index) {
    Cell cell = {};
    std::string line = "";
    size_t ncount = 0;
    size_t col_width = getColumnWidth(column_index);
    for (size_t i = 0; i < str.length(); i++) {
        if ((ncount < col_width) && (str[i] != '\n') && (str[i] != '\r')) {
            line += str[i];
            ncount += 1;
        } else {
            if (str[i] == '\r') {  // 跳过\r\n
                i++;
            }
            cell.lines.push_back(line);
            if (ncount > cell.col_width) {
                cell.col_width = ncount;
            }
            line = "";
            ncount = 0;
        }
    }
    if (ncount != 0 && line != "") {
        cell.lines.push_back(line);
        if (ncount > cell.col_width) {
            cell.col_width = ncount;
        }
    }
    if (m_layout == ConsoleTable::Layout::fixed) {  // 覆盖默认配置
        auto it = m_fixed_col_width.find(column_index);
        if (it != m_fixed_col_width.end()) {
            cell.col_width = it->second;
        }
    }
    return cell;
}

/**
 * @todo 将每个单元格填充到指定列宽和高度
 * ------       -----------
 * |ABC |  ---> |ABC      |
 * |DEF |       |DEF      |
 * ------       |         |
 *              -----------
 * @author songhuabiao@greatwall.com.cn
 */
std::vector<std::string> ConsoleTable::fillCell(ConsoleTable::Cell& cell,
                                                size_t width,
                                                size_t nrows) {
    std::vector<std::string> result = {};
    size_t lines = cell.lines.size();
    if (m_valign == ConsoleTable::Valign::top) {
        for (size_t i = 0; i < nrows; i++) {
            std::string str = i < lines ? cell.lines[i] : "";
            if (m_align == ConsoleTable::Align::left) {
                padding_right(str, width, ' ');
            } else {
                padding_left(str, width, ' ');
            }
            result.push_back(str);
        }
    } else if (m_valign == ConsoleTable::Valign::middle) {
        size_t top = (nrows - lines) / 2;
        size_t bottom = top + lines;
        for (size_t i = 0; i < nrows; i++) {
            std::string str = "";
            if (i >= top && i <= bottom && lines > 0 && (i - top) < lines) {
                str = cell.lines[i - top];
            }
            if (m_align == ConsoleTable::Align::left) {
                padding_right(str, width, ' ');
            } else {
                padding_left(str, width, ' ');
            }
            result.push_back(str);
        }
    } else {
        size_t top = (nrows - lines);
        for (size_t i = 0; i < nrows; i++) {
            std::string str = (i >= top) ? cell.lines[i] : "";
            if (m_align == ConsoleTable::Align::left) {
                padding_right(str, width, ' ');
            } else {
                padding_left(str, width, ' ');
            }
            result.push_back(str);
        }
    }

    return result;
}
