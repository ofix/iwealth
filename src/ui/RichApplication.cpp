#include "ui/RichApplication.h"
#include "ui/RichMainFrame.h"

//(*AppHeaders
#include <wx/image.h>
#include "ui/RichMainFrame.h"
#include "util/RichResult.h"
//*)

wxIMPLEMENT_APP(RichApplication);

bool RichApplication::OnInit() {
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if (wxsOK) {
        m_pFrame = new RichMainFrame(NULL, wxID_ANY);
        m_pFrame->Show();
        SetTopWindow(m_pFrame);
        m_pStockStorage = new StockDataStorage(m_pFrame);
        if (m_pStockStorage) {
            RichResult result = m_pStockStorage->Init();
            if (!result.Ok()) {
                wxMessageBox(result.What());
            } else {
                if (m_pStockStorage->IsQuoteDataReady()) {  //本地行情数据已经是最新的，立即刷新窗口显示
                    m_pFrame->Refresh();
                }
            }
        }
    }
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
