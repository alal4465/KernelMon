#pragma once
#include <Windows.h>
#include <functional>
#include <vector>
#include <algorithm>
#include <iterator>
#include "Window.h"
#include "GuiBase.h"

namespace Gui {
	struct MenuItem {
		const std::string name;
		const UINT_PTR id;
	};


	class MenuBar : public GuiBase<HMENU> {
	public:
		MenuBar() : GuiBase<HMENU>(CreateMenu()) {}

		virtual ~MenuBar() = default;

		void pack(const Window& window) const;

		void add_menu_item(const MenuItem& item);

		void add_sub_menu(const MenuBar& submenu, const std::string& menu_name);
	};

}