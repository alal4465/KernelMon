#include "../KernelMonitor/common.h"
#include <iostream>
#include <Windows.h>
#include <unordered_map>
#include <string_view>

const std::unordered_map<MonitoredFunctions, std::string_view> monitored_functions = {
	{MonitoredFunctions::ZwCreateFile, "ZwCreateFile"},
	{MonitoredFunctions::ZwWriteFile, "ZwWriteFile"},
	{MonitoredFunctions::ZwCreateKey, "ZwCreateKey"}
};

int main() {
	// DEVICE_SYM_NAME or USERMODE_DEVICE_NAME?
	auto device_handle = CreateFile(
		DEVICE_SYM_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (device_handle == INVALID_HANDLE_VALUE) {
		std::cout << "[-] CreateFile failed with last error: " << GetLastError() << '\n';
		return -1;
	}
	DWORD returned;
	LogEntry log;
	while (true) {
		if (!DeviceIoControl(device_handle, static_cast<DWORD>(KernelMonIoctls::GetData), nullptr, 0, &log, sizeof(log), &returned, nullptr) || log.function == MonitoredFunctions::None) {
			//std::cout << "[-] DeviceIoControl failed with last error: " << GetLastError() << '\n';
			Sleep(1000);
			continue;
		}

		std::cout << "[+] Log entry from ";
		std::wcout << std::wstring_view(log.driver);
		std::cout << '\n';
		std::cout << "Function: " << monitored_functions.at(log.function) << '\n';
		std::cout << "Details: ";
		std::wcout << std::wstring_view(log.details);
		std::cout << '\n';
		std::cout << "Result: " << log.result << "\n\n";


	}

	CloseHandle(device_handle);

	return 0;
}
