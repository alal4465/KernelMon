#include "HookInfoManager.h"
Hooking::HookInfoManager* g_hook_info_manager = nullptr;

Hooking::HookInfoManager::HookInfoManager() : hook_info_{ 0 } {
	void* hooked_functions[HOOKED_FUNCTION_COUNT] = {
		ZwCreateFile
	};

	for (size_t i = 0; i < HOOKED_FUNCTION_COUNT; i++) {
		hook_info_[i] = { hooked_functions[i], nullptr };
	}
}

void Hooking::HookInfoManager::set_jump_stub(void* function_addr, void* jmp_stub) {
	for (auto& data : hook_info_) {
		if (data.original_function == function_addr) {
			data.jmp_stub = jmp_stub;
		}
	}
}