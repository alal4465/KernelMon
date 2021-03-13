#pragma once

constexpr const WCHAR DEVICE_SYM_NAME[] = L"\\??\\KernelMon";
constexpr const WCHAR USERMODE_DEVICE_NAME[] = L"\\\\.\\KernelMon";

constexpr unsigned long long MAX_MONITORED_DRIVERS = 10;

enum class KernelMonIoctls {
	GetData = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_OUT_DIRECT, FILE_ANY_ACCESS),
};

enum class MonitoredFunctions {
	None,
	ZwCreateFile,
	ZwWriteFile,
	ZwCreateKey
};

struct LogEntry {
	WCHAR driver[20];
	MonitoredFunctions function;
	NTSTATUS result;
	WCHAR details[265];
};
