#include "vmx.h"
vmx::VmmContext* g_vmm_context{ nullptr };
extern "C" void processor_initialize_vmx(vmx::VCpu * vcpu, PVOID guest_rsp);
NTSTATUS zw_create_file_callback(
	PHANDLE            FileHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK   IoStatusBlock,
	PLARGE_INTEGER     AllocationSize,
	ULONG              FileAttributes,
	ULONG              ShareAccess,
	ULONG              CreateDisposition,
	ULONG              CreateOptions,
	PVOID              EaBuffer,
	ULONG              EaLength
) {
	KdPrint(("ZwCreateFile called with: %wZ\n", ObjectAttributes->ObjectName));
	//__debugbreak();

	auto jump_stub = g_hook_info_manager->lookup_jump_stub(ZwCreateFile);
	return jump_stub(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength
	);
}

namespace vmx{
	static bool enable_vmx_operation();
	static void adjust_control_registers();
	static void terminate_cpu(ULONG_PTR);
}

bool vmx::initialize_vmx() {
	g_vmm_context = alloc_vmm_context();
	if (!g_vmm_context)
		return false;
	__debugbreak();
	g_hook_info_manager = new(NonPagedPool) Hooking::HookInfoManager();
	g_test_hook = new(NonPagedPool) Hooking::EptHook(ZwCreateFile, zw_create_file_callback);
	//hooking::init();
	KeIpiGenericCall(reinterpret_cast<PKIPI_BROADCAST_WORKER>(vmm_setup_stub), reinterpret_cast<ULONG_PTR>(g_vmm_context->vcpu_table));

	return true;
}

void vmx::terminate_vmx() {
	if (!g_vmm_context)
		return;

	KeIpiGenericCall(reinterpret_cast<PKIPI_BROADCAST_WORKER>(terminate_cpu), 0);

	cleanup_vmm_context(g_vmm_context);
	g_vmm_context = nullptr;
}


extern "C" void processor_initialize_vmx(vmx::VCpu * vcpu_table, PVOID guest_rsp) {
	auto processor_number = KeGetCurrentProcessorNumber();
	auto vcpu = &vcpu_table[processor_number];

	KdPrint(("[+] Initializing VMX on processor : %u\n", processor_number));

	vmx::adjust_control_registers();

	KdPrint(("[+] Adjusted control registers on processor : %u\n", processor_number));

	if (!vmx::enable_vmx_operation()) {
		KdPrint(("[-] Failed to enable vm operation on processor: %u\n", processor_number));
		return;
	}
	KdPrint(("[+] Enabled vmx operation : %u\n", processor_number));

	if (__vmx_on(&vcpu->vmxon_physical)) {
		KdPrint(("Failed to execute vmxon on processor: %u\n", processor_number));
		return;
	}
	KdPrint(("[+] Processor %u executed vmxon\n", processor_number));

	if (!vmx::setup_vmcs_fields(vcpu, guest_rsp)) {
		KdPrint(("[-] Vmcs setup failed on processor: %u\n", KeGetCurrentProcessorNumber()));
		return;
	}

	KdPrint(("[+] Processor %u vmcs setup complete\n", KeGetCurrentProcessorNumber()));

	auto status = __vmx_vmlaunch();

	if (status) {
		size_t vmx_error;
		__vmx_vmread(static_cast<size_t>(vmx::VmcsField::VMCS_VM_INSTRUCTION_ERROR), &vmx_error);
		KdPrint(("[-] Vmlaunch failed on processor: %u, status: %u, error: 0x%llx\n", KeGetCurrentProcessorNumber(), status, vmx_error));
	}

}

static bool vmx::enable_vmx_operation() {
	arch::Cr4 cr4;

	cr4.control = __readcr4();
	cr4.fields.vmx_enable |= 1;
	__writecr4(cr4.control);

	arch::Ia32FeatureControlMsr feature_control_msr;

	feature_control_msr.control = __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_FEATURE_CONTROL));

	if (feature_control_msr.fields.lock)
		return true;

	feature_control_msr.fields.vmxon_outside_smx |= 1;
	feature_control_msr.fields.lock |= 1;

	__writemsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_FEATURE_CONTROL), feature_control_msr.control);

	return true;
}

static void vmx::adjust_control_registers() {
	arch::Cr0 cr0;
	cr0.control = __readcr0();

	cr0.control |= __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_VMX_CR0_FIXED0));
	cr0.control &= __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_VMX_CR0_FIXED1));

	__writecr0(cr0.control);

	arch::Cr4 cr4;
	cr4.control = __readcr4();

	cr4.control |= __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_VMX_CR4_FIXED0));
	cr4.control &= __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_VMX_CR4_FIXED1));

	__writecr4(cr4.control);
}

static void vmx::terminate_cpu(ULONG_PTR) {
	__vmx_off();
}