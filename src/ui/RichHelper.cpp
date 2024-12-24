///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/RichHelper.h
// Purpose:     help functions for GUI
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-19 18:07
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/RichHelper.h"
#include "util/Global.h"

wxString RichUnit(double num) {
    if (num == 0) {
        return wxString("---");  // 股票停牌没有成交量和成交额
    }
    std::string result;
    if (num >= 100000000) {
        num /= 100000000;
        result = convert_double(num, 2) + "亿";
    } else if (num >= 10000) {
        num /= 10000;
        result = convert_double(num, 2) + "万";
    } else {
        result = std::to_string(static_cast<int>(num));
    }
    return CN(result);
}

wxFont RichHelper::GetDefaultFont(int font_size) {
    wxFont defaultFont(font_size, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("微软雅黑"),
                       wxFONTENCODING_DEFAULT);
    return defaultFont;
}

wxFont RichHelper::GetBoldFont(int font_size){
    wxFont boldFont(font_size, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_EXTRABOLD, false, _T("微软雅黑"),
                       wxFONTENCODING_DEFAULT);
    return boldFont;
}
