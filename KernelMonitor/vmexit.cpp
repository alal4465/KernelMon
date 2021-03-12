#include "vmexit.h"

namespace vmx {
	namespace vmexit {
		static void increment_rip();

		static void cpuid_handler(vmx::VmexitGuestContext* context, bool& increment_rip);
		static void cr_access_handler(vmx::VmexitGuestContext* context, size_t exit_qualification, bool& increment_rip);
		static void ept_violation_handler(bool& increment_rip);
		static void mtf_handler(bool& increment_rip);
		static void read_msr_handler(bool& increment_rip, vmx::VmexitGuestContext* context);
		static void write_msr_handler(bool& increment_rip, vmx::VmexitGuestContext* context);
	}
}

extern "C" void vmexit_handler(vmx::VmexitGuestContext * context) {
	size_t exit_reason{ 0 };
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_EXIT_REASON), &exit_reason);

	size_t exit_qualification{ 0 };
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_EXIT_QUALIFICATION), &exit_qualification);

	exit_reason &= 0xffff;
	size_t faulting_addr{ 0 }; // TODO: remove
	bool increment_rip{true};
	switch (static_cast<vmx::VmExitReason>(exit_reason)) {
	case vmx::VmExitReason::EXIT_REASON_CPUID:
		vmx::vmexit::cpuid_handler(context, increment_rip);
		break;

	case vmx::VmExitReason::EXIT_REASON_CR_ACCESS:
		vmx::vmexit::cr_access_handler(context, exit_qualification, increment_rip);
		break;

	case vmx::VmExitReason::EXIT_REASON_EPT_VIOLATION:
		KdPrint(("[+] Ept violation. Reason: 0x%llx\n", exit_qualification));
		__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_PHYSICAL_ADDRESS), &faulting_addr);
		KdPrint(("[+] at: 0x%llx\n", faulting_addr));
		vmx::vmexit::ept_violation_handler(increment_rip);
		break;

	case vmx::VmExitReason::EXIT_REASON_MONITOR_TRAP_FLAG:
		KdPrint(("[+] MTF\n"));
		vmx::vmexit::mtf_handler(increment_rip);
		break;

	case vmx::VmExitReason::EXIT_REASON_EPT_MISCONFIG:
		__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_PHYSICAL_ADDRESS), &faulting_addr);
		KdPrint(("[-] EPT misconfig at: 0x%llx\n", faulting_addr));
		__debugbreak();
		break;

	case vmx::VmExitReason::EXIT_REASON_MSR_READ:
		vmx::vmexit::read_msr_handler(increment_rip, context);
		break;

	case vmx::VmExitReason::EXIT_REASON_MSR_WRITE:
		vmx::vmexit::write_msr_handler(increment_rip, context);
		break;

	default:
		KdPrint(("[-] VMEXIT unknown reason: %u\n", exit_reason));
		__debugbreak();
		break;
	}

	if (increment_rip) {
		vmx::vmexit::increment_rip();
	}
}

static void vmx::vmexit::cpuid_handler(vmx::VmexitGuestContext* context, bool& increment_rip) {
	auto cpuid_eax = static_cast<unsigned __int32>(context->rax);
	auto cpuid_ecx = static_cast<unsigned __int32>(context->rcx);

	arch::cpuid_feature_info_t cpuid_info;
	__cpuidex(reinterpret_cast<int*>(&cpuid_info), cpuid_eax, cpuid_ecx);

	if (cpuid_eax == static_cast<unsigned int>(arch::CpuidCode::FEATURE_INFO)) {
		cpuid_info.ecx.features.vmx = 0;
		cpuid_info.ecx.features.hypervisor = 0;
	}

	context->rax = cpuid_info.eax.control;
	context->rbx = cpuid_info.ebx.control;
	context->rcx = cpuid_info.ecx.control;
	context->rdx = cpuid_info.edx.control;
	
	increment_rip = true;
}

static void vmx::vmexit::cr_access_handler(vmx::VmexitGuestContext* context, size_t exit_qualification, bool& increment_rip) {
	arch::CrExitQualifiction cr_data;
	cr_data.control = exit_qualification;

	unsigned __int64 rsp{ 0 };
	unsigned __int64* reg_ptr{ 0 };

	switch (cr_data.fields.mov_cr_gp_register)
	{
	case 0: reg_ptr = &context->rax; break;
	case 1: reg_ptr = &context->rcx; break;
	case 2: reg_ptr = &context->rdx; break;
	case 3: reg_ptr = &context->rbx; break;
	case 4:
		__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RSP), &rsp);
		reg_ptr = &rsp;
		break;
	case 5: reg_ptr = &context->rbp; break;
	case 6: reg_ptr = &context->rsi; break;
	case 7: reg_ptr = &context->rdi; break;
	default: KdPrint(("[-] non-supported gp register in cr handler\n")); __debugbreak(); return;
	}

	switch (static_cast<arch::CrAccessType>(cr_data.fields.access_type)) {
	case arch::CrAccessType::MOV_TO_CR:
		switch (cr_data.fields.cr_number) {
		case 0:
			__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR0), *reg_ptr);
			__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_CR0_READ_SHADOW), *reg_ptr);

			break;
		case 3:
			__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR3), (*reg_ptr) & 0xfffffffffffff);

			break;
		case 4:
			__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR4), *reg_ptr);
			__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_CR4_READ_SHADOW), *reg_ptr);

			break;
		default:
			KdPrint(("[-] non-supported control register mov to\n"));
			__debugbreak();
			return;
		}
		break;

	case arch::CrAccessType::MOV_FROM_CR:
		switch (cr_data.fields.cr_number) {
		case 0:
			__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR0), reg_ptr);

			break;
		case 3:
			__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR3), reg_ptr);

			break;
		case 4:
			__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR4), reg_ptr);

			break;
		default:
			KdPrint(("[-] non-supported control register mov from\n"));
			__debugbreak();
			return;
		}
		break;

	default:
		__debugbreak();
		break;
	}

	if (static_cast<arch::CrAccessType>(cr_data.fields.access_type) == arch::CrAccessType::MOV_FROM_CR
		&& cr_data.fields.cr_number == 4) {
		__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RSP), rsp);
	}

	increment_rip = true;
}

static void vmx::vmexit::ept_violation_handler(bool& increment_rip) {
	globals.hooking_engine->handle_ept_violation();
	increment_rip = false;
}

static void vmx::vmexit::mtf_handler(bool& increment_rip) {
	globals.hooking_engine->handle_mtf();
	increment_rip = false;
}

static void vmx::vmexit::read_msr_handler(bool& increment_rip, vmx::VmexitGuestContext* context) {
	ULARGE_INTEGER msr{0};
	msr.QuadPart = __readmsr(static_cast<ULONG>(context->rcx));

	context->rax = msr.LowPart;
	context->rdx = msr.HighPart;

	increment_rip = true;
}

static void vmx::vmexit::write_msr_handler(bool& increment_rip, vmx::VmexitGuestContext* context) {
	ULARGE_INTEGER msr = { 0 };
	msr.LowPart  = static_cast<ULONG>(context->rax);
	msr.HighPart = static_cast<ULONG>(context->rdx);

	__writemsr(static_cast<ULONG>(context->rcx), msr.QuadPart);
	increment_rip = true;
}

static void vmx::vmexit::increment_rip() {
	unsigned __int64 rip{ 0 };
	unsigned __int64 instruction_length{ 0 };

	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RIP), &rip);
	__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_VMEXIT_INSTRUCTION_LENGTH), &instruction_length);

	__vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RIP), rip + instruction_length);
}
