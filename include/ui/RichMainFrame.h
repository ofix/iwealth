///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/ui/WealthMainFrame.h
// Purpose:     GUI main frame
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

#include <wx/listctrl.h>
#include "ui/components/PanelStockQuote.h"

enum { ID_Hello = 1 };

class RichMainFrame : public wxFrame {
   public:
    RichMainFrame(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize);
    virtual ~RichMainFrame();

   protected:
    //(*Identifiers(RichMainFrame)
    static const long ID_PANEL_STOCK_QUOTE;
    //*)

   private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

   private:
    PanelStockQuote* m_panelStockQuota;
};
