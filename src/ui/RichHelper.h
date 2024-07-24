#ifndef RICH_HELPER_H
#define RICH_HELPER_H

#include <wx/font.h>
#include <wx/string.h>
#include <string>

#define CN(data) wxString::FromUTF8(data)

wxString RichUnit(double num);

class RichHelper {
   public:
    static wxFont GetDefaultFont(int font_size = 12);
};

#endif  // RICH_HELPER_H
