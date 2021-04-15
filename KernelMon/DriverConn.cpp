#include "DriverConn.h"

DriverConn::DriverConn(const std::wstring& driver_name) : device_handle_(
	CreateFileW(driver_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr)
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

bool DriverConn::reset_state() {
	return device_ioctl(KernelMonIoctls::ResetState, nullptr, 0);
}

bool DriverConn::receive_data(LogEntry& buf) {
	return device_ioctl(KernelMonIoctls::GetData, &buf, sizeof(buf));
}

bool DriverConn::add_driver(std::wstring& driver_name) {
	return device_ioctl(KernelMonIoctls::AddDriver, &(driver_name[0]), driver_name.size() * 2 + 2);
}

bool DriverConn::remove_driver(std::wstring& driver_name) {
	return device_ioctl(KernelMonIoctls::RemoveDriver, &(driver_name[0]), driver_name.size() * 2 + 2);
}

bool DriverConn::device_ioctl(KernelMonIoctls ioctl, void* buf, unsigned int size) {
	DWORD returned;
	auto ret_code = DeviceIoControl(device_handle_, static_cast<unsigned int>(ioctl), buf, size, buf, size, &returned, nullptr);

	return ret_code != 0;
}