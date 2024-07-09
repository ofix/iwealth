#include "ui/components/RichGridColumnHeaderProvider.h"
#include "ui/RichApplication.h"

RichGridColumnHeaderProvider::RichGridColumnHeaderProvider() : m_headerRenderer() {
}

const wxGridColumnHeaderRenderer& RichGridColumnHeaderProvider::GetColumnHeaderRenderer(int /*col*/) {
    return m_headerRenderer;
    // return wxGridCellAttrProvider::GetColumnHeaderRenderer(col);
}