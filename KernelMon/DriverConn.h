#pragma once
#include <string>
#include <Windows.h>
#include "../KernelMonitor/common.h"
#include <stdexcept>

class DriverConn final {
public:
	DriverConn(const std::wstring& driver_name);

	~DriverConn();

	bool reset_state();

	bool receive_data(LogEntry& buf);

	bool add_driver(std::wstring& driver_name);

	bool remove_driver(std::wstring& driver_name);
private:
	bool device_ioctl(KernelMonIoctls ioctl, void* buf, unsigned int size);

	HANDLE device_handle_;
};