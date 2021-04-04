#include "ListBox.h"

const std::string Gui::ListBox::WINAPI_CLASS_NAME("ListBox");

Gui::ListBox::ListBox(const Window& parent, const Dimensions& size, const Point& top_left) :
	Window(WINAPI_CLASS_NAME, window_params(WindowStyle::ListBox, parent.handle(), top_left, size, WINAPI_CLASS_NAME))
{

}

void Gui::ListBox::add_item(const std::wstring& log) {
	SendMessageW(handle(), LB_ADDSTRING, NULL, (LPARAM)log.c_str());
}