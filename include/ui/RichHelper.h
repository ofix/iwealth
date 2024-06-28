#ifndef RICHHELPER_H
#define RICHHELPER_H
#include <wx/string.h>
#include <string>

#define CN(data) wxString::FromUTF8(data)

std::string convertDouble(double digit, int precision = 2);

wxString RichUnit(double num);

#endif  // RICHHELPER_H
