#pragma once
#include "HookInfoManager.h"
#include "EptHook.h"

namespace Hooking {

	class HookingEngine final {
	public:
		template <typename T>
		HookingEngine(const T& hooked_functions);

		~HookingEngine();
	private:
		EptHook* hook_array;
		HookInfoManager info_manager;
	};

}