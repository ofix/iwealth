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
#include "ui/components/DialogShareSearch.h"
#include "ui/components/PanelKline.h"
#include "ui/components/PanelStockQuote.h"

class RichApplication;
class RichMainFrame : public wxFrame {
   public:
    RichMainFrame(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize);
    virtual ~RichMainFrame();
    void OnClose(wxCloseEvent& event);
    void OnIconize(wxIconizeEvent& event);
    void OnMaximize(wxMaximizeEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnGridCellLeftClick(wxGridEvent& event);
    void OnGridCellLeftDblClick(wxGridEvent& event);
    void AdjustDlgShareSearchPostion();
    void AddPanelToStack(wxPanel* panel);
    wxPanel* PopPanelFromStack();

   protected:
    //(*Identifiers(RichMainFrame)
    //*)
    static const long ID_PANEL_STOCK_QUOTE;
    static const long ID_PANEL_KLINE;
    static const long ID_DIALOG_SHARE_SEARCH;

   private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    // this is called when the event from the thread is received
    void OnStorageDataReady(wxThreadEvent& event);

   private:
    std::vector<wxPanel*> m_panelStack;  // 视图栈
    wxPanel* m_panelCurrent;             // 当前视图
    DialogShareSearch* m_dlgShareSearch;
    DECLARE_EVENT_TABLE()
};
