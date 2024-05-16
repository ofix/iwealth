#include "ui/RichApplication.h"
#include "ui/RichMainFrame.h"

//(*AppHeaders
#include <wx/image.h>
#include "ui/RichMainFrame.h"
//*)

wxIMPLEMENT_APP(RichApplication);

bool RichApplication::OnInit() {
    m_pStockStorage = new StockDataStorage();
    m_pStockStorage->Init();  // 进行必要的数据加载
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if (wxsOK) {
        RichMainFrame* Frame = new RichMainFrame(NULL, wxID_ANY);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return true;
}

StockDataStorage* RichApplication::GetStockDataStorage() {
    return m_pStockStorage;
}
