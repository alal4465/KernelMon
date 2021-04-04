#include <iostream>
#include "Application.h"
#include <iostream>
#include <unordered_map>
#include <string_view>
#include <Windows.h>
#include "../KernelMonitor/common.h"
#include <string>
#include "ListBox.h"
#include "DriverConn.h"

Gui::Application app;
const std::unordered_map<MonitoredFunctions, std::wstring> monitored_functions = {
	{MonitoredFunctions::ZwCreateFile, L"ZwCreateFile"},
	{MonitoredFunctions::ZwWriteFile, L"ZwWriteFile"},
	{MonitoredFunctions::ZwCreateKey, L"ZwCreateKey"}
};


void request_driver_log(HWND, UINT, UINT_PTR, DWORD) {
	std::cout << "timeout\n";

	static DriverConn driver_conn(DEVICE_SYM_NAME);
	LogEntry log;

	if (!driver_conn.receive_data(log) || log.function == MonitoredFunctions::None) {
		return;
	}

	std::wstring str = std::wstring(L"Driver: ") + log.driver + L" | ";
	str += std::wstring(L"Function: ") + monitored_functions.at(log.function) + L" | ";
	str += std::wstring(L"Path: ") + log.path + L" | ";
	str += std::wstring(L"Details: ") + log.details + L" | ";
	str += std::wstring(L"Result: ") + std::to_wstring(log.result);

	std::wcout << str;
	app.add_log(str);
}


int main()
{
	app.set_visibility(true);
	app.set_timer(1000, request_driver_log);
	app.main_loop();

	return 0;
}

