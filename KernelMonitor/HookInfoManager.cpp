#include "HookInfoManager.h"

void Hooking::HookInfoManager::set_jump_stub(void* function_addr, void* jmp_stub) {
	for (size_t i = 0; i < hooked_function_count_; i++) {
		if (function_jmp_stub_map_[i].first == function_addr) {
			function_jmp_stub_map_[i].second = jmp_stub;
		}
	}
}