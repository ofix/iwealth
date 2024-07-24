#ifndef RICH_APPLICATION_H
#define RICH_APPLICATION_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class RichMainFrame;
class RichApplication : public wxApp {
   public:
    virtual bool OnInit();
    RichMainFrame* GetMainFrame();

   private:
    RichMainFrame* m_pFrame;
};

#endif