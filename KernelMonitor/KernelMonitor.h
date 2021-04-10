#pragma once
#include <ntddk.h>
#include "SpinLock.h"
#include "CyclicBuffer.h"
#include "HookInfoManager.h"
#include "HookingEngine.h"
#include "context.h"
#include "common.h"

constexpr const WCHAR DEVICE_NAME[] = L"\\Device\\KernelMon";
constexpr unsigned __int64 MAX_PERRALLEL_BUF_ENTRIES = 10;

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