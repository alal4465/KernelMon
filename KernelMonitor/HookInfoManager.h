#pragma once
#include <ntddk.h>

namespace Hooking {
	constexpr unsigned __int64 HOOKED_FUNCTION_COUNT = 1;
	
	class HookInfoManager {
	public:
		HookInfoManager();

		template <typename T>
		T lookup_jump_stub(T function_ptr) {
			for (const auto& data : hook_info_) {
				if (data.original_function == function_ptr) {
					return reinterpret_cast<T>(data.jmp_stub);
				}
			}
			return reinterpret_cast<T>(nullptr);
		}

		void set_jump_stub(void* function_addr, void* jmp_stub);
	private:
		struct FunctionJmpStubPair {
			void* original_function;
			void* jmp_stub;
		};

		FunctionJmpStubPair hook_info_[HOOKED_FUNCTION_COUNT];
	};
}

extern Hooking::HookInfoManager* g_hook_info_manager;