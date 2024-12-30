///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/ui/RichPanelTest.cpp
// Purpose:     test panel for debuggging
// Author:      songhuabiao
// Modified by:
// Created:     2024-12-30 20:00
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "RichPanelTest.h"
#include "util/FileTool.h"
#include "util/Global.h"

BEGIN_EVENT_TABLE(RichPanelTest, wxPanel)
EVT_PAINT(RichPanelTest::OnPaint)
EVT_ERASE_BACKGROUND(RichPanelTest::OnBackground)
END_EVENT_TABLE()

RichPanelTest::RichPanelTest(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, id, pos, size) {
    // 使用自动双缓冲
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

RichPanelTest::~RichPanelTest() {
}

void RichPanelTest::TestLoadSvgFile(wxDC* pDC) {
    // 加载SVG文件
    wxSVGDocument* svgDoc = new wxSVGDocument;
    if (svgDoc->Load(FileTool::CurrentPath() + "assets" + DIR_SEPARATOR + "market.svg")) {
        wxImage img = svgDoc->Render(32,32);
        pDC->DrawBitmap(img, 0, 0,true);
    }
    if (svgDoc) {
        delete svgDoc;
    }
}

void RichPanelTest::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    TestLoadSvgFile(&dc);
}

void RichPanelTest::OnBackground(wxEraseEvent& event) {
    wxDC* pDC = event.GetDC();
    pDC->SetBrush(wxBrush(wxColour(255, 255, 255)));
}
