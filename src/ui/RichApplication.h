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
#include "ui/RichMainFrame.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "stock/StockDataStorage.h"

class RichMainFrame;
class RichApplication : public wxApp {
   public:
    virtual bool OnInit();
    StockDataStorage* GetStockDataStorage();
    RichMainFrame* GetMainFrame();
    static wxFont GetDefaultFont(int font_size = 12);

   private:
    StockDataStorage* m_pStockStorage;
    RichMainFrame* m_pFrame;
};
