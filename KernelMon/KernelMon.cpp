#include "GuiImpl.h"
#include "Application.h"
#include <iostream>
#include <cstdint>

Gui::Application Gui::app;

int main(int argc, char** argv) {
	if (argc > 1 && argv[1] == std::string_view("--console")) {
		return Gui::app.console_loop(argc, argv);
	}

	return Gui::main_loop();
}