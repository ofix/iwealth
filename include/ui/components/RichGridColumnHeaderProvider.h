#ifndef RICHCOLUMNHEADERPROVIDER_H
#define RICHCOLUMNHEADERPROVIDER_H

#include <wx/grid.h>
#include "ui/components/RichGridColumnHeaderRenderer.h"

class RichGridColumnHeaderProvider : public wxGridCellAttrProvider {
   public:
    RichGridColumnHeaderProvider();

   protected:
    virtual const wxGridColumnHeaderRenderer& GetColumnHeaderRenderer(int col);

   private:
    RichGridColumnHeaderRenderer m_headerRenderer;
    wxFont m_font;
    wxDECLARE_NO_COPY_CLASS(RichGridColumnHeaderProvider);
};

#endif