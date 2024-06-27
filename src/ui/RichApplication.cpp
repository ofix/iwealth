#include "ui/RichApplication.h"
#include "ui/RichMainFrame.h"

//(*AppHeaders
#include <wx/image.h>
#include "ui/RichMainFrame.h"
//*)

wxIMPLEMENT_APP(RichApplication);

bool RichApplication::OnInit() {
    m_pStockStorage = new StockDataStorage();
    if (m_pStockStorage) {
        m_pStockStorage->Init();
    }
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if (wxsOK) {
        m_pFrame = new RichMainFrame(NULL, wxID_ANY);
        m_pFrame->Show();
        SetTopWindow(m_pFrame);
    }
    //*)
    return true;
}

wxFont RichApplication::GetDefaultFont(int font_size) {
    wxFont defaultFont(font_size, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("微软雅黑"),
                       wxFONTENCODING_DEFAULT);
    return defaultFont;
}

RichMainFrame* RichApplication::GetMainFrame() {
    return m_pFrame;
}

StockDataStorage* RichApplication::GetStockDataStorage() {
    return m_pStockStorage;
}
