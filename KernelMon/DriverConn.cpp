#include "DriverConn.h"

DriverConn::DriverConn(const std::wstring& driver_name) : device_handle_(
	CreateFile(driver_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr)
)
{
	if (device_handle_ == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Opening driver handle failed with err code: " + GetLastError());
	}
}

DriverConn::~DriverConn() {
	try {
		CloseHandle(device_handle_);
	}
	catch (...) {}
}