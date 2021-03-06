#pragma once

constexpr const wchar_t DEVICE_SYM_NAME[] = L"\\??\\KernelMon";
constexpr const wchar_t USERMODE_DEVICE_NAME[] = L"\\\\.\\KernelMon";

constexpr unsigned long long MAX_MONITORED_DRIVERS = 5;

enum class KernelMonIoctls {
	GetData        = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_OUT_DIRECT, FILE_ANY_ACCESS),
	AddDriver      = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS),
	RemoveDriver   = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x822, METHOD_BUFFERED, FILE_ANY_ACCESS),
	ResetState     = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x823, METHOD_BUFFERED, FILE_ANY_ACCESS)
};

enum class MonitoredFunctions {
	None,
	ZwCreateFile,
	ZwWriteFile,
	ZwCreateKey,
	ZwSetValueKey,
	ZwTerminateProcess,
};

struct LogEntry {
	wchar_t driver[20];
	MonitoredFunctions function;
	long result;
	wchar_t path[265];
	wchar_t details[150];
};
