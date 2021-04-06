#pragma once
#include <string>
#include <Windows.h>
#include "../KernelMonitor/common.h"
#include <stdexcept>

class DriverConn final {
public:
	DriverConn(const std::wstring& driver_name);

	~DriverConn();

	template <typename TBuf>
	bool receive_data(TBuf& buf) {
		return device_ioctl(KernelMonIoctls::GetData, buf);
	}

private:
	template <typename TBuf>
	bool device_ioctl(KernelMonIoctls ioctl, TBuf& buf) {
		DWORD returned;
		auto ret_code = DeviceIoControl(device_handle_, static_cast<unsigned int>(KernelMonIoctls::GetData), nullptr, 0, &buf, sizeof(buf), &returned, nullptr);

		return ret_code != 0;
	}

	HANDLE device_handle_;
};