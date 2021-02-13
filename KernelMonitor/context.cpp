#include "context.h"

static vmx::Vmxon* alloc_vmxon();
static vmx::Vmcs* alloc_vmcs();
static unsigned __int64 virtual_to_physical(PVOID vaddr);

vmx::VmmContext* vmx::alloc_vmm_context() {
	PHYSICAL_ADDRESS physical_max;
	physical_max.QuadPart = ~0ULL;

	auto vmm_context = new(PagedPool) VmmContext{ 0 };

	RtlSecureZeroMemory(vmm_context, sizeof(VmmContext));

	if (!vmm_context)
		return nullptr;

	vmm_context->processor_count = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
	vmm_context->vcpu_table = new(NonPagedPool) VCpu[vmm_context->processor_count]{ 0 };

	if (!vmm_context->vcpu_table) {
		delete vmm_context;
		return nullptr;
	}
	RtlSecureZeroMemory(vmm_context->vcpu_table, sizeof(VCpu) * vmm_context->processor_count);

	auto msr_bitmap = MmAllocateContiguousMemory(PAGE_SIZE, physical_max);

	if (!msr_bitmap) {
		delete[] vmm_context->vcpu_table;
		delete vmm_context;
		return nullptr;
	}

	RtlSecureZeroMemory(msr_bitmap, PAGE_SIZE);

	for (unsigned __int32 i = 0; i < vmm_context->processor_count; i++) {
		vmm_context->vcpu_table[i].vmxon = alloc_vmxon();
		vmm_context->vcpu_table[i].vmcs = alloc_vmcs();
		vmm_context->vcpu_table[i].msr_bitmap = msr_bitmap;
		vmm_context->vcpu_table[i].stack = MmAllocateNonCachedMemory(VMM_STACK_SIZE);

		if (vmm_context->vcpu_table[i].vmcs == nullptr || vmm_context->vcpu_table[i].vmxon == nullptr || vmm_context->vcpu_table[i].stack == nullptr) {
			cleanup_vmm_context(vmm_context);
			return nullptr;
		}

		vmm_context->vcpu_table[i].vmxon_physical = virtual_to_physical(vmm_context->vcpu_table[i].vmxon);
		vmm_context->vcpu_table[i].vmcs_physical = virtual_to_physical(vmm_context->vcpu_table[i].vmcs);
		vmm_context->vcpu_table[i].msr_bitmap_physical = virtual_to_physical(vmm_context->vcpu_table[i].msr_bitmap);
	}

	return vmm_context;
}

static vmx::Vmxon* alloc_vmxon() {
	arch::Ia32VmxBasicMsr vmx_basic_msr{ 0 };
	vmx::Vmxon* vmxon;
	PHYSICAL_ADDRESS physical_max;
	physical_max.QuadPart = ~0ULL;

	vmx_basic_msr.control = __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_VMX_BASIC));

	if (vmx_basic_msr.fields.vmxon_region_size > PAGE_SIZE)
		vmxon = static_cast<vmx::Vmxon*>(MmAllocateContiguousMemory(PAGE_SIZE, physical_max));
	else
		vmxon = static_cast<vmx::Vmxon*>(MmAllocateContiguousMemory(vmx_basic_msr.fields.vmxon_region_size, physical_max));

	if (!vmxon)
		return nullptr;

	RtlSecureZeroMemory(vmxon, vmx::VMXON_REGION_SIZE);

	vmxon->revision_identifier = vmx_basic_msr.fields.vmcs_revision_identifier;

	return vmxon;
}

static vmx::Vmcs* alloc_vmcs() {
	arch::Ia32VmxBasicMsr vmx_basic_msr{ 0 };
	vmx_basic_msr.control = __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_VMX_BASIC));
	
	PHYSICAL_ADDRESS physical_max;
	physical_max.QuadPart = ~0ULL;

	auto vmcs = static_cast<vmx::Vmcs*>(MmAllocateContiguousMemory(PAGE_SIZE, physical_max));
	RtlSecureZeroMemory(vmcs, vmx::VMCS_REGION_SIZE);

	vmcs->revision_identifier = vmx_basic_msr.fields.vmcs_revision_identifier;
	vmcs->shadow_vmcs_indicator = 0;

	return vmcs;
}

static unsigned __int64 virtual_to_physical(PVOID vaddr) {
	return MmGetPhysicalAddress(vaddr).QuadPart;
}

void vmx::cleanup_vmm_context(vmx::VmmContext* vmm_context) {
	if (!vmm_context)
		return;

	if (vmm_context->vcpu_table) {
		MmFreeContiguousMemory(vmm_context->vcpu_table[0].msr_bitmap);
		for (unsigned __int32 i = 0; i < vmm_context->processor_count; i++) {
			if (vmm_context->vcpu_table[i].vmxon)
				MmFreeContiguousMemory(vmm_context->vcpu_table[i].vmxon);

			if (vmm_context->vcpu_table[i].vmcs)
				MmFreeContiguousMemory(vmm_context->vcpu_table[i].vmcs);

			if (vmm_context->vcpu_table[i].stack)
				MmFreeNonCachedMemory(vmm_context->vcpu_table[i].stack, VMM_STACK_SIZE);
		}
	}

	delete[] vmm_context->vcpu_table;
	delete vmm_context;
}