#include "ui/RichApplication.h"
#include "ui/RichMainFrame.h"

//(*AppHeaders
#include "ui/RichMainFrame.h"
#include <wx/image.h>
//*)

wxIMPLEMENT_APP(RichApplication);

bool RichApplication::OnInit() {
    m_pDataCenter = new ShareDataCenter();
    m_pDataCenter->Init(); // 进行必要的数据加载
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK ) {
        RichMainFrame* Frame = new RichMainFrame(NULL,wxID_ANY);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return true;
}

ShareDataCenter* RichApplication::GetDataCenter(){
   return m_pDataCenter;
}
