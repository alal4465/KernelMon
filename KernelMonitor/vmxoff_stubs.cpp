#include "vmcs_fields.h"

extern "C" {
	UINT64 g_guest_rip;
	UINT64 g_guest_rsp;

	void prepare_vmxoff_globals() {
		__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RSP), &g_guest_rsp);
		__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RIP), &g_guest_rip);
	}
}