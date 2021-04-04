#pragma once
#include "Window.h"
#include "MenuBar.h"
#include <Windows.h>
#include "resource.h"
#include "ListBox.h"

namespace Gui {

	class Application : public Window {
	public:
		Application();

		void add_log(const std::wstring& log);

	private:
		void add_widgets();

		ListBox log_list_;

		static const std::string WINAPI_CLASS_NAME;

		static LRESULT CALLBACK handle_window_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}