#include "ui/rich_app.h"
#include "ui/rich_main_frame.h"

wxIMPLEMENT_APP(RichApplication);

bool RichApplication::OnInit() {
    RichMainFrame* frame = new RichMainFrame();
    frame->Show(true);
    return true;
}