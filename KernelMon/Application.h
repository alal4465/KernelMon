#pragma once
#include "imgui/imgui.h"
#include <vector>
#include <string>
#include <array>
#include <Windows.h>
#include "../KernelMonitor/common.h"
#include "util.h"
#include "DriverConn.h"

namespace Gui {
	struct AppLog final {
		std::string driver;
		MonitoredFunctions function;
		long result;
		std::string path;
		std::string details;
	};

	class Application final {
	public:
		Application();

		void render_tick();

		void show_drivers_window();

		void show_log_window();

		void add_log(AppLog&& log);

		int console_loop(int argc, char** argv);

		DriverConn& driver() { return driver_conn_; }
	private:
		std::vector<AppLog> logs_;
		std::vector<std::string> monitored_drivers_;
		DriverConn driver_conn_;
	};

	extern Application app;
}