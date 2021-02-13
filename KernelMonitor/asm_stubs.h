#pragma once

extern "C" {
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
}