#include "ui/components/RichGridColumnHeaderProvider.h"

RichGridColumnHeaderProvider::RichGridColumnHeaderProvider() : m_headerRenderer() {
}

const wxGridColumnHeaderRenderer& RichGridColumnHeaderProvider::GetColumnHeaderRenderer(int /*col*/) {
    return m_headerRenderer;
    // return wxGridCellAttrProvider::GetColumnHeaderRenderer(col);
}