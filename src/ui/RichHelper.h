#ifndef RICHHELPER_H
#define RICHHELPER_H
#include <wx/string.h>
#include <string>

#define CN(data) wxString::FromUTF8(data)

wxString RichUnit(double num);

#endif  // RICHHELPER_H
