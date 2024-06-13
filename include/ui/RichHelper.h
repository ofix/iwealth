#ifndef RICHHELPER_H
#define RICHHELPER_H

#define CN(data) wxString::FromUTF8(data)

std::string convertDouble(double digit, int precision=2);

#endif // RICHHELPER_H
