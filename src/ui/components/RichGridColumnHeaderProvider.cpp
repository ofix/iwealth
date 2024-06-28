#include "ui/components/RichGridColumnHeaderProvider.h"
#include "ui/RichApplication.h"

RichGridColumnHeaderProvider::RichGridColumnHeaderProvider() : m_headerRenderer() {
    m_headerRenderer.SetFont(RichApplication::GetDefaultFont(12));
}

const wxGridColumnHeaderRenderer& RichGridColumnHeaderProvider::GetColumnHeaderRenderer(int col) {
    return m_headerRenderer;
    // return wxGridCellAttrProvider::GetColumnHeaderRenderer(col);
}