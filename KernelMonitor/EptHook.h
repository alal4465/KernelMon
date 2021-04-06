#pragma once
#include <ntddk.h>
#include "ept.h"
#include "context.h"
#include "ldisasm.h"
#include "asm_stubs.h"
#include "HookInfoManager.h"

namespace Hooking {
	class EptHook final {
	public:
		explicit EptHook(void* hooked_function_virtual, void* hook_callback_function);

		EptHook(const EptHook& rhs) = delete;

		EptHook& operator=(const EptHook& rhs) = delete;

		void handle_ept_violation() volatile;

		void handle_mtf() volatile;

		void add_function_hook(void* hooked_function_virtual, void* hook_callback_function);

		unsigned __int64 guest_physical_page() { return MmGetPhysicalAddress(read_write_page_).QuadPart & 0xfffffffffffff000; }

		//~EptHook();
	private:
		void* execute_page_;
		void* read_write_page_;
		vmx::ept::Pte** ptes_;
	};
}