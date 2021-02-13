#pragma once
#include <ntddk.h>
#include <intrin.h>
#include "cr.h"
#include "msr.h"
#include "memory.h"

namespace vmx {
	constexpr unsigned __int64 VMXON_REGION_SIZE = 0x1000;
	constexpr unsigned __int64 VMCS_REGION_SIZE = 0x1000;
	constexpr unsigned __int64 VMM_STACK_SIZE = 0x1000;

	struct Vmxon {
		unsigned __int32 revision_identifier;
		unsigned char vmxon_data[VMXON_REGION_SIZE - sizeof(unsigned __int32)];
	};

	struct Vmcs {
		unsigned __int32 revision_identifier : 31;
		unsigned __int32 shadow_vmcs_indicator : 1;
		unsigned __int32 vmx_abort_indicator;
		unsigned char vmcs_data[VMCS_REGION_SIZE - (2 * sizeof(unsigned __int32))];
	};

	struct VCpu {
		Vmcs* vmcs;
		unsigned __int64 vmcs_physical;

		Vmxon* vmxon;
		unsigned __int64 vmxon_physical;

		void* msr_bitmap;
		unsigned __int64 msr_bitmap_physical;

		void* stack;
	};

	struct VmmContext {
		unsigned __int32 processor_count;
		VCpu* vcpu_table;
	};

	VmmContext* alloc_vmm_context();
	void cleanup_vmm_context(VmmContext* vmm_context);
}