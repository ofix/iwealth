#ifndef RICH_TOP_MENU_ITEM_H
#define RICH_TOP_MENU_ITEM_H

#include <wx/wx.h>
#include <string>

enum class MenuItemState { Normal = 0, Disabled = 1, Hilight = 2 };

class RichTopMenuItem {
   public:
    RichTopMenuItem(const std::string name);
    ~RichTopMenuItem();
    void Disable();
    void Hilight();

   private:
    std::string m_menu_item_name;
    MenuItemState m_state;
};

#endif