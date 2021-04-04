#pragma once
#include <Windows.h>
#include <utility>

namespace Gui {
	constexpr unsigned int USE_DEFAULT_VALUES = CW_USEDEFAULT;
	constexpr std::pair<unsigned int, unsigned int> DEFAULT_PARAMS_PAIR = { USE_DEFAULT_VALUES, USE_DEFAULT_VALUES };
	using Point = std::pair<unsigned int, unsigned int>;
	using Dimensions = std::pair<unsigned int, unsigned int>;

	enum class WindowStyle : unsigned long {
		Child = WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_BORDER,
		Scroll = WS_VSCROLL,
		Disabled = WS_DISABLED,
		Overlapped = WS_OVERLAPPED,
		Visible = WS_VISIBLE | WS_OVERLAPPED,
		App = WS_OVERLAPPED | WS_BORDER,
		ListBox = LBS_DISABLENOSCROLL | WS_VSCROLL | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_BORDER,
		Popup = WS_POPUPWINDOW | WS_VISIBLE | WS_OVERLAPPED
	};

	enum class DialogBoxStyle : unsigned long {

	};

	struct window_params {
		window_params(WindowStyle style = WindowStyle::Visible, HWND parent_handle = 0, Point location = DEFAULT_PARAMS_PAIR, Dimensions size = DEFAULT_PARAMS_PAIR, const std::string& window_name = "") :
			window_style(style),
			window_location(location),
			window_size(size),
			parent_window(parent_handle),
			name(window_name) {}

		WindowStyle window_style;
		Point window_location;
		Dimensions window_size;
		HWND parent_window;
		std::string name;
	};
}