#pragma once
#include "HookInfoManager.h"
#include "EptHook.h"

namespace Hooking {

	class HookingEngine final {
	public:
		template <typename TArr>
		HookingEngine(const TArr& hooked_functions):
			hooked_function_count_(sizeof(hooked_functions) / sizeof(hooked_functions[0])),
			function_hooks_(new (NonPagedPool) EptHook*[hooked_function_count_]),
			expected_mtf_function_index_(-1)
		{
			__debugbreak();
			globals.hook_info_manager = new (NonPagedPool) HookInfoManager(hooked_functions);

			for (unsigned __int32 i = 0; i < hooked_function_count_; i++) {
				function_hooks_[i] = new(NonPagedPool) EptHook(hooked_functions[i].first, hooked_functions[i].second);
			}
		}

		HookingEngine& operator=(const HookingEngine& rhs) = delete;

		~HookingEngine();

		void handle_ept_violation();

		void handle_mtf();
	private:
		unsigned __int64 hooked_function_count_;
		EptHook** function_hooks_;
		__int32 expected_mtf_function_index_;
	};

}