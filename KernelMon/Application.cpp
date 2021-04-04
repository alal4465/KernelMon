#include "Application.h"

const std::string Gui::Application::WINAPI_CLASS_NAME("KernelMon");

Gui::Application::Application() :
    Window(WINAPI_CLASS_NAME, window_params(WindowStyle::App, 0, DEFAULT_PARAMS_PAIR, { 1920, 1028 }, WINAPI_CLASS_NAME), handle_window_message),
    log_list_(*this, { 1900, 978 })
{
    add_widgets();
}


void Gui::Application::add_log(const std::wstring& log) {
    log_list_.add_item(log);
}

void Gui::Application::add_widgets() {
    MenuBar menu;
    menu.add_menu_item({ "&Filter", static_cast<unsigned long long>(Idm::MenuFilter) });
    menu.add_menu_item({ "&Add driver", static_cast<unsigned long long>(Idm::MenuAddDriver) });
    menu.add_menu_item({ "&Remove driver", static_cast<unsigned long long>(Idm::MenuAddDriver) });
    menu.add_menu_item({ "&Quit", static_cast<unsigned long long>(Idm::MenuQuit) });
    menu.pack(*this);
}


LRESULT CALLBACK Gui::Application::handle_window_message(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)));
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_COMMAND:
        switch (static_cast<Idm>(LOWORD(wParam))) {
        case Idm::MenuQuit:
            PostQuitMessage(0);
            break;

        case Idm::MenuFilter:
            break;
        }
        break;
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}