///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/ui/WealthApplication.h
// Purpose:     GUI main entry
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-13
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class RichApplication : public wxApp {
   public:
    virtual bool OnInit();
};