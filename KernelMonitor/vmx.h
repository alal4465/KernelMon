#pragma once
#include <ntddk.h>
#include <intrin.h>
#include "cr.h"
#include "msr.h"
#include "context.h"
#include "vmcs_fields.h"
#include "cpuid.h"
#include "asm_stubs.h"
#include "EptHook.h"
#include "HookInfoManager.h"

extern vmx::VmmContext* g_vmm_context;

namespace vmx {
	bool initialize_vmx();

	void terminate_vmx();
}