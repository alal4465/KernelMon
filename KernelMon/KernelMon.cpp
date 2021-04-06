#include "GuiImpl.h"
#include "Application.h"
#include <iostream>

Gui::Application Gui::app;

int main(int argc, void** argv) {
	auto ret_code = Gui::main_loop();
	std::cerr << "[+] Program exited with return code: " << ret_code;
}