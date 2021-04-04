#include "MenuBar.h"

void Gui::MenuBar::pack(const Window& window) const {
	SetMenu(window.handle(), handle());
}

void Gui::MenuBar::add_menu_item(const MenuItem& item) {
	AppendMenuA(handle(), MF_STRING, item.id, item.name.c_str());
}

void Gui::MenuBar::add_sub_menu(const MenuBar& submenu, const std::string& menu_name) {
	AppendMenuA(handle(), MF_POPUP, reinterpret_cast<UINT_PTR>(submenu.handle()), menu_name.c_str());
}