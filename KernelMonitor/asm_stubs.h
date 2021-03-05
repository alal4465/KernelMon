#pragma once
#include "vmcs_fields.h"

extern "C" {
	enum class InveptType : unsigned __int32 {
		SINGLE_CONTEXT = 1,
		GLOBAL_CONTEXT
	};

	unsigned short __readcs();
	unsigned short __readds();
	unsigned short __reades();
	unsigned short __readss();
	unsigned short __readfs();
	unsigned short __readgs();
	unsigned short __readldtr();
	unsigned short __readtr();

	unsigned __int64 __readrsp();
	unsigned __int64 __readrip();

	unsigned long __read_segment_access_rights(unsigned long segment_selector);

	void vmm_entrypoint();
	void vmm_setup_stub(void* vcpu);

	void vmexit_stub();
	void vmm_entrypoint();

	void __invept(unsigned __int32 type, void* descriptors);
	inline void invept_global() {
		unsigned __int64 descriptors[2];

		__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_EPT_POINTER), descriptors);
		descriptors[1] = 0;

		__invept(static_cast<unsigned __int32>(InveptType::GLOBAL_CONTEXT), descriptors);
	}
}