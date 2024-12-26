#ifndef RICH_SVG_IMAGE_H
#define RICH_SVG_IMAGE_H

#include <wx/wx.h>

class RichSvgImage {
   public:
    RichSvgImage(const wxString& svg_path);
    virtual ~RichSvgImage();
    void Draw(wxDC* pDC);

   private:
    wxString m_svgPath;
};

#endif