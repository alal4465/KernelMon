#pragma once
#include <Windows.h>
#include <string_view>
#include "GuiBase.h"
#include <iostream>
#include "WindowOptions.h"
#include "resource.h"

namespace Gui {
	using WindowCallbackFunction = LRESULT CALLBACK(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	class Window : public GuiBase<HWND> {
	public:
		Window(const std::string& class_name, const window_params& params = {}, WindowCallbackFunction callback = nullptr);

		virtual ~Window();

		void set_visibility(bool is_visible);

		void main_loop();

		Dimensions get_size() const;

		void set_timer(unsigned int timeout_ms, TIMERPROC callback) const;
	private:
		static HWND init_window(const std::string& class_name, const window_params& params, WindowCallbackFunction callback);
	};

}