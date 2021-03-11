#include "EptHook.h"
#include "vmx.h"

Hooking::EptHook* g_test_hook = nullptr;

namespace Hooking {
	/*
	* push rdi
	* movabs rdi, 0xffffffffffffffff
	* xchg QWORD PTR [rsp], rdi
	* ret
	*/
	static constexpr unsigned char HOOK_PATCH_TEMPLATE[] = { 0x57, 0x48, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x48, 0x87, 0x3c, 0x24, 0xc3 };
	static constexpr unsigned __int64 HOOK_ADDRESS_OFFSET = 3;
}

Hooking::EptHook::EptHook(void* hooked_function_virtual, void* hook_callback_function):
	hook_callback_function_(hook_callback_function),
	hooked_function_(hooked_function_virtual),
	read_write_page_(reinterpret_cast<void*>(reinterpret_cast<unsigned __int64>(hooked_function_virtual) & 0xfffffffffffff000)),
	execute_page_(new (NonPagedPool) unsigned char[PAGE_SIZE]),
	ptes_(new(NonPagedPool) vmx::ept::Pte*[g_vmm_context->processor_count])
{
	RtlCopyMemory(execute_page_, read_write_page_, PAGE_SIZE);
	auto exec_page_function = static_cast<unsigned char*>(execute_page_) + (reinterpret_cast<unsigned __int64>(hooked_function_virtual) % PAGE_SIZE);

	size_t overwritten_length{ 0 };
	while (overwritten_length < sizeof(HOOK_PATCH_TEMPLATE))
		overwritten_length += ldisasm::get_instruction_length(exec_page_function + overwritten_length);

	unsigned char* jump_stub = new(NonPagedPool) unsigned char[overwritten_length + sizeof(HOOK_PATCH_TEMPLATE)];
	RtlCopyMemory(jump_stub, exec_page_function, overwritten_length);
	RtlCopyMemory(jump_stub + overwritten_length, HOOK_PATCH_TEMPLATE, sizeof(HOOK_PATCH_TEMPLATE));

	void* post_hook_jmp_dest = static_cast<unsigned char*>(hooked_function_virtual) + overwritten_length;
	RtlCopyMemory(jump_stub + overwritten_length + HOOK_ADDRESS_OFFSET, &post_hook_jmp_dest, sizeof(void*));

	g_hook_info_manager->set_jump_stub(hooked_function_, jump_stub);

	RtlCopyMemory(exec_page_function, HOOK_PATCH_TEMPLATE, sizeof(HOOK_PATCH_TEMPLATE));
	RtlCopyMemory(exec_page_function + HOOK_ADDRESS_OFFSET, &hook_callback_function_, sizeof(void*));

	for (size_t i = 0; i < g_vmm_context->processor_count; i++) {
		auto vcpu = &g_vmm_context->vcpu_table[i];
		unsigned __int64 guest_physical_page = MmGetPhysicalAddress(read_write_page_).QuadPart;

		vmx::ept::LargePdte* large_pdte = &vcpu->large_pdt[(guest_physical_page >> 21) / vmx::ept::EPT_TABLE_ENTRIES][(guest_physical_page >> 21) % vmx::ept::EPT_TABLE_ENTRIES];
		vmx::ept::split_large_pdte(large_pdte);

		PHYSICAL_ADDRESS physical_addr{ 0 };
		physical_addr.QuadPart = reinterpret_cast<vmx::ept::Pdte*>(large_pdte)->fields.pt_physical * PAGE_SIZE;
		vmx::ept::Pte* pte = reinterpret_cast<vmx::ept::Pte*>(MmGetVirtualForPhysical(physical_addr));
		pte = &pte[(guest_physical_page >> 12) % vmx::ept::EPT_TABLE_ENTRIES];

		vmx::ept::Pte pte_template;
		pte_template.control = 0;
		pte_template.execute = true;
		pte_template.pfn = MmGetPhysicalAddress(execute_page_).QuadPart >> 12;

		pte->control = pte_template.control;
		ptes_[i] = pte;
	}
}

// TODO: will verify that the violation is relevent
void Hooking::EptHook::handle_ept_violation() volatile{
	__debugbreak();
	auto pte = ptes_[KeGetCurrentProcessorNumber()];

	pte->pfn = MmGetPhysicalAddress(read_write_page_).QuadPart >> 12;
	pte->read = true;
	pte->write = true;
	pte->execute = false;

	unsigned __int64 rip{ 0 };
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RIP), &rip);
	KdPrint(("Guest RIP: 0x%llx\n", rip));

	invept_global();

	unsigned __int64 primary_processor_based_ctrl{ 0 };
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS), &primary_processor_based_ctrl);
	reinterpret_cast<vmx::VmxPrimaryProcessorBasedControl*>(&primary_processor_based_ctrl)->fields.monitor_trap_flag = true;
	__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS), primary_processor_based_ctrl);	
}

void Hooking::EptHook::handle_mtf() volatile {
	__debugbreak();
	auto pte = ptes_[KeGetCurrentProcessorNumber()];

	pte->pfn = MmGetPhysicalAddress(execute_page_).QuadPart >> 12;
	pte->read = false;
	pte->write = false;
	pte->execute = true;

	unsigned __int64 rip{ 0 };
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RIP), &rip);
	KdPrint(("Guest RIP: 0x%llx\n", rip));

	invept_global();

	unsigned __int64 primary_processor_based_ctrl{ 0 };
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS), &primary_processor_based_ctrl);
	reinterpret_cast<vmx::VmxPrimaryProcessorBasedControl*>(&primary_processor_based_ctrl)->fields.monitor_trap_flag = false;
	__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS), primary_processor_based_ctrl);
}