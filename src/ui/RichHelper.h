#ifndef RICH_HELPER_H
#define RICH_HELPER_H

#include <wx/colour.h>
#include <wx/font.h>
#include <wx/string.h>
#include <string>

#define CN(data) wxString::FromUTF8(data)

wxString RichUnit(double num);

class RichHelper {
   public:
    static wxFont GetDefaultFont(int font_size = 12);
    static wxFont GetBoldFont(int font_size = 12);
};

struct RichColorText {
    wxString text;
    wxColor color;
};

#endif  // RICH_HELPER_H
