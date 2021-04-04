#include "Window.h"
#include "MenuBar.h"

Gui::Window::Window(const std::string& class_name, const window_params& params, WindowCallbackFunction callback) :
	GuiBase(init_window(class_name, params, callback))
{

}

Gui::Window::~Window() {

}

void Gui::Window::set_visibility(bool is_visible) {
	ShowWindow(handle(), is_visible ? SW_SHOWDEFAULT : SW_HIDE);
}

void Gui::Window::main_loop() {
	MSG msg = { };

	while (GetMessageA(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

HWND Gui::Window::init_window(const std::string& class_name, const window_params& params, WindowCallbackFunction callback) {
	WNDCLASSA window_data = {};

	window_data.lpfnWndProc = callback;
	window_data.hInstance = 0;
	window_data.lpszClassName = class_name.data();
	window_data.hbrBackground = CreateSolidBrush(COLOR_WINDOW + 1);

	ATOM class_atom = 0;
	if (callback) {
		class_atom = RegisterClassA(&window_data);
	}

	return CreateWindowExA(
		0,
		class_name.data(),
		params.name.data(),
		static_cast<DWORD>(params.window_style),
		params.window_location.first,
		params.window_location.second,
		params.window_size.first,
		params.window_size.second,
		params.parent_window,
		0,
		0,
		0
	);
}

Gui::Dimensions Gui::Window::get_size() const {
	RECT rect;
	if (!GetWindowRect(handle(), &rect)) {
		throw std::runtime_error("Failed to get window size. last error: " + GetLastError());
	}

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	return { width, height };
}

void Gui::Window::set_timer(unsigned int timeout_ms, TIMERPROC callback) const {
	SetTimer(
		handle(),
		static_cast<UINT_PTR>(Idm::TimerId),
		timeout_ms,
		callback
	);
}