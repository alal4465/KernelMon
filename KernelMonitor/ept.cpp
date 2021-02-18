#include "ept.h"

namespace vmx {
	namespace ept {
		static bool alloc_ept(VCpu* vcpu);
	}
}

bool vmx::ept::setup_ept(VCpu* vcpu) {
	if (!alloc_ept(vcpu))
		return false;

	vcpu->pml4t->fields.read = true;
	vcpu->pml4t->fields.write = true;
	vcpu->pml4t->fields.exec = true;
	vcpu->pml4t->fields.user_mode_exec = true;
	vcpu->pml4t->fields.pdpt_physical = MmGetPhysicalAddress(vcpu->pdpt).QuadPart / PAGE_SIZE;

	for (int i = 0; i < EPT_TABLE_ENTRIES; i++) {
		vcpu->pdpt[i].fields.read = true;
		vcpu->pdpt[i].fields.write = true;
		vcpu->pdpt[i].fields.execute = true;
		vcpu->pdpt[i].fields.user_mode_exec = true;
		vcpu->pdpt[i].fields.pdt_physical = MmGetPhysicalAddress(vcpu->large_pdt[i]).QuadPart / PAGE_SIZE;
	}

	for (int i = 0; i < EPT_TABLE_ENTRIES; i++) {
		for (int j = 0; j < EPT_TABLE_ENTRIES; j++) {
			vcpu->large_pdt[i][j].fields.read = true;
			vcpu->large_pdt[i][j].fields.write = true;
			vcpu->large_pdt[i][j].fields.exec = true;
			vcpu->large_pdt[i][j].fields.large = true;
			vcpu->large_pdt[i][j].fields.user_mode_exec = true;
			vcpu->large_pdt[i][j].fields.type = static_cast<unsigned __int64>(arch::MtrrType::WB);
			vcpu->large_pdt[i][j].fields.pfn = i * EPT_TABLE_ENTRIES + j;
		}
	}

	return true;
}

void vmx::ept::cleanup_ept(VCpu* vcpu) {
	delete[] vcpu->pml4t;
	delete[] vcpu->pdpt;
	for (int i = 0; i < EPT_TABLE_ENTRIES; i++) {
		delete[] vcpu->large_pdt[i];
	}
	delete[] vcpu->large_pdt;
}

static bool vmx::ept::alloc_ept(VCpu* vcpu) {
	vcpu->pml4t = new(NonPagedPool) Pml4e[EPT_TABLE_ENTRIES]{0};
	vcpu->pdpt = new(NonPagedPool) Pdpte[EPT_TABLE_ENTRIES]{0};
	vcpu->large_pdt = new(NonPagedPool) LargePdte * [EPT_TABLE_ENTRIES] {0};
	if (!vcpu->pml4t || !vcpu->pdpt || !vcpu->large_pdt)
		return false;

	RtlSecureZeroMemory(vcpu->pml4t, sizeof(Pml4e) * EPT_TABLE_ENTRIES);
	RtlSecureZeroMemory(vcpu->pdpt, sizeof(Pdpte) * EPT_TABLE_ENTRIES);


	for (int i = 0; i < EPT_TABLE_ENTRIES; i++) {
		vcpu->large_pdt[i] = new(NonPagedPool) LargePdte[EPT_TABLE_ENTRIES]{0};
		if (!vcpu->large_pdt[i])
			return false;

		RtlSecureZeroMemory(vcpu->large_pdt[i], sizeof(LargePdte) * EPT_TABLE_ENTRIES);
	}

	KdPrint(("[+] pml4t allocated in: 0x%llx, physical: 0x%llx\n", reinterpret_cast<unsigned __int64>(vcpu->pml4t), MmGetPhysicalAddress(vcpu->pml4t).QuadPart));
	KdPrint(("[+] pdpt allocated in: 0x%llx, physical: 0x%llx\n", reinterpret_cast<unsigned __int64>(vcpu->pdpt), MmGetPhysicalAddress(vcpu->pdpt).QuadPart));
	KdPrint(("[+] large_pdt allocated in: 0x%llx, physical: 0x%llx\n", reinterpret_cast<unsigned __int64>(vcpu->large_pdt), MmGetPhysicalAddress(vcpu->large_pdt).QuadPart));
	
	NT_ASSERT(reinterpret_cast<unsigned __int64>(vcpu->pml4t) % PAGE_SIZE == 0);
	NT_ASSERT(reinterpret_cast<unsigned __int64>(vcpu->pdpt) % PAGE_SIZE == 0);
	NT_ASSERT(reinterpret_cast<unsigned __int64>(vcpu->large_pdt) % PAGE_SIZE == 0);

	NT_ASSERT(MmGetPhysicalAddress(vcpu->pml4t).QuadPart % PAGE_SIZE == 0);
	NT_ASSERT(MmGetPhysicalAddress(vcpu->pdpt).QuadPart % PAGE_SIZE == 0);
	NT_ASSERT(MmGetPhysicalAddress(vcpu->large_pdt).QuadPart % PAGE_SIZE == 0);

	return true;
}