#pragma once
#include <ntddk.h>
#include "SpinLock.h"
#include "CyclicBuffer.h"
#include "HookInfoManager.h"
#include "HookingEngine.h"
#include "context.h"

constexpr const WCHAR DEVICE_SYM_NAME[] = L"\\??\\KernelMon";
constexpr const WCHAR DEVICE_NAME[] = L"\\Device\\KernelMon";
constexpr unsigned __int64 MAX_MONITORED_DRIVERS = 10;
constexpr unsigned __int64 MAX_PERRALLEL_BUF_ENTRIES = 10;

struct LogEntry {
	WCHAR driver[20];
	char function[20];
	char result[20];
	char details[100];
};

struct KernelMonGlobals {
	PWCHAR monitored_drivers[MAX_MONITORED_DRIVERS];
	kstd::CyclicBuffer<LogEntry, kstd::SpinLock>* driver_log_buffer;
	Hooking::HookInfoManager* hook_info_manager;
	Hooking::HookingEngine* hooking_engine;
	vmx::VmmContext* vmm_context;
	PDEVICE_OBJECT device_obj;
	PDRIVER_OBJECT driver_obj;
};

extern KernelMonGlobals globals;