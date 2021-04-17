#pragma once
#include <ntddk.h>
#include "Pair.h"

namespace Hooking {
	class HookInfoManager final{
	public:
		template <typename TArr>
		HookInfoManager(const TArr& hooked_functions) :
			hooked_function_count_(sizeof(hooked_functions)/sizeof(hooked_functions[0])),
			function_jmp_stub_map_(new (NonPagedPool)kstd::Pair<void*, void*>[hooked_function_count_] {nullptr, nullptr})
		{
			for (size_t i = 0; i < hooked_function_count_; i++) {
				function_jmp_stub_map_[i] = { hooked_functions[i].first, nullptr };
			}
		}

		template <typename TFunc>
		TFunc lookup_jump_stub(TFunc function_ptr) {
			for (size_t i = 0; i < hooked_function_count_; i++) {
				if (function_jmp_stub_map_[i].first == function_ptr) {
					return reinterpret_cast<TFunc>(function_jmp_stub_map_[i].second);
				}
			}
			return reinterpret_cast<TFunc>(nullptr);
		}

		void set_jump_stub(void* function_addr, void* jmp_stub);

		~HookInfoManager() { delete function_jmp_stub_map_; }
	private:
		unsigned __int64 hooked_function_count_;
		kstd::Pair<void*, void*>* function_jmp_stub_map_;
	};
}