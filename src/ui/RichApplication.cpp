#include "ui/RichApplication.h"
#include "ui/RichMainFrame.h"

//(*AppHeaders
#include "ui/RichMainFrame.h"
#include <wx/image.h>
//*)

wxIMPLEMENT_APP(RichApplication);

bool RichApplication::OnInit() {
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
