#include "HookingEngine.h"
#include "KernelMonitor.h"
#include "vmcs_fields.h"
Hooking::HookingEngine::~HookingEngine() {
	// for ? in ? delete function_hooks[i]
	delete[] function_hooks_;
}

void Hooking::HookingEngine::handle_ept_violation() {
	size_t guest_physical_addr = 0;
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_PHYSICAL_ADDRESS), &guest_physical_addr);
	guest_physical_addr &= 0xfffffffffffff000;

	for (unsigned __int32 i = 0; i < hooked_function_count_; i++) {
		if (function_hooks_[i]->guest_physical_page() == guest_physical_addr) {
			function_hooks_[i]->handle_ept_violation();
			expected_mtf_function_index_ = i;
			return;
		}
	}
}

void Hooking::HookingEngine::handle_mtf() {
	if (expected_mtf_function_index_ >= 0)
		function_hooks_[expected_mtf_function_index_]->handle_mtf();

	expected_mtf_function_index_ = -1;
}