#ifndef RICH_TOP_MENU_H
#define RICH_TOP_MENU_H

#include <wx/wx.h>
#include <string>
#include <vector>
#include "ui/components/RichTopMenuItem.h"

class RichTopMenu {
   public:
    RichTopMenu(const wxString& name, wxWindow* parent);
    virtual ~RichTopMenu();
    void Append(const long menu_id, const std::string menu_name);
    void DisableMenuItem(const std::string& menu_item_name);
    void EnableMenuItem(const std::string& menu_item_name);
    int FindMenuItem(const std::string& menu_item_name);
    wxSize DoGetBestClientSize() const;
    void OnPaint(wxPaintEvent& event);

   private:
    wxString m_menuName;
    wxWindow* m_parent;
    std::vector<RichTopMenuItem> m_menuItems;
};

#endif