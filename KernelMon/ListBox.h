#pragma once
#include "Window.h"
#include <Windows.h>

namespace Gui {
	class ListBox : public Window {
	public:
		ListBox(const Window& parent, const Dimensions& size = DEFAULT_PARAMS_PAIR, const Point& top_left = DEFAULT_PARAMS_PAIR);

		void add_item(const std::wstring& log);

	private:
		static const std::string WINAPI_CLASS_NAME;
	};
}