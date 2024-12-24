///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/RichApplication.h
// Purpose:     application main entry
// Author:      songhuabiao
// Modified by:
// Created:     2024-05-13 17:30
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "ui/RichApplication.h"
#include "ui/RichMainFrame.h"

wxIMPLEMENT_APP(RichApplication);

bool RichApplication::OnInit() {
    wxInitAllImageHandlers();
    m_pFrame = new RichMainFrame(NULL, ID_MAIN_FRAME);
    m_pFrame->Show();
    SetTopWindow(m_pFrame);
    m_pFrame->LoadQuote();
    return true;
}

RichMainFrame* RichApplication::GetMainFrame() {
    return m_pFrame;
}
