#include "vmcs_fields.h"
#include "asm_stubs.h"

#pragma warning(disable: 4805 4333)

static constexpr unsigned __int32 SEGMENT_DESCRIPTOR_TYPE_TSS_AVAILABLE = 11;
static constexpr unsigned __int32 SEGMENT_DESCRIPTOR_TYPE_TSS_BUSY = 9;
static constexpr unsigned __int16 SELECTOR_MASK = 0xf8;

static unsigned __int32 read_segment_access_rights(unsigned __int16 segment_selector);
static unsigned __int64 get_segment_base(unsigned __int64 gdt_base, unsigned __int16 segment_selector);

static void adjust_controls(unsigned __int32& control, arch::Msr msr);

bool vmx::setup_vmcs_fields(vmx::VCpu* vcpu, PVOID guest_rsp) {
    auto status = false;

    if (__vmx_vmclear(&vcpu->vmcs_physical) || __vmx_vmptrld(&vcpu->vmcs_physical)) {
        KdPrint(("Couldn't setup active vmcs in processor: %u\n", KeGetCurrentProcessorNumber()));
        return status;
    }

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR0), __readcr0());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR3), __readcr3());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CR4), __readcr4());

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_DR7), __readdr(7));

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RSP), reinterpret_cast<unsigned __int64>(guest_rsp));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RIP), reinterpret_cast<unsigned __int64>(vmm_entrypoint));

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_RFLAGS), __readeflags());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_DEBUGCTL), __readmsr(static_cast<size_t>(arch::Msr::MSR_IA32_DEBUGCTL)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_SYSENTER_ESP), __readmsr(static_cast<size_t>(arch::Msr::MSR_IA32_SYSENTER_ESP)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_SYSENTER_EIP), __readmsr(static_cast<size_t>(arch::Msr::MSR_IA32_SYSENTER_EIP)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_SYSENTER_CS), __readmsr(static_cast<size_t>(arch::Msr::MSR_IA32_SYSENTER_CS)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_VMCS_LINK_POINTER), MAXUINT64);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_FS_BASE), __readmsr(static_cast<size_t>(arch::Msr::MSR_FS_BASE)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_GS_BASE), __readmsr(static_cast<size_t>(arch::Msr::MSR_GS_BASE)));

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_ES_BASE), 0ull);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CS_BASE), 0ull);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_SS_BASE), 0ull);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_DS_BASE), 0ull);


    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_RSP), reinterpret_cast<unsigned __int64>(static_cast<unsigned char*>(vcpu->stack) + VMM_STACK_SIZE - 0x10));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_RIP), reinterpret_cast<unsigned __int64>(vmexit_stub));

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_SYSENTER_ESP), __readmsr(static_cast<size_t>(arch::Msr::MSR_IA32_SYSENTER_ESP)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_SYSENTER_EIP), __readmsr(static_cast<size_t>(arch::Msr::MSR_IA32_SYSENTER_EIP)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_SYSENTER_CS), __readmsr(static_cast<size_t>(arch::Msr::MSR_IA32_SYSENTER_CS)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_FS_BASE), __readmsr(static_cast<size_t>(arch::Msr::MSR_FS_BASE)));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_GS_BASE), __readmsr(static_cast<size_t>(arch::Msr::MSR_GS_BASE)));

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_CR0_READ_SHADOW), __readcr0());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_CR4_READ_SHADOW), __readcr4());

    arch::Ia32VmxBasicMsr basic_msr;
    basic_msr.control = __readmsr(static_cast<unsigned __int64>(arch::Msr::MSR_IA32_VMX_BASIC));

    vmx::Ia32VmxPinbasedControl pin_based_control{ 0 };
    pin_based_control.control = 0;

    adjust_controls
    (
        pin_based_control.control,
        (
            basic_msr.fields.true_controls
            ? arch::Msr::MSR_IA32_VMX_TRUE_PINBASED_CTLS
            : arch::Msr::MSR_IA32_VMX_PINBASED_CTLS
            )
    );

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS), pin_based_control.control);

    vmx::VmxPrimaryProcessorBasedControl primary_processor_based_control{ 0 };
    primary_processor_based_control.control = 0;

    primary_processor_based_control.fields.cr3_load_exiting = true;
    primary_processor_based_control.fields.active_secondary_controls = true;
    primary_processor_based_control.fields.use_msr_bitmaps = true;

    adjust_controls
    (
        primary_processor_based_control.control,
        (
            basic_msr.fields.true_controls
            ? arch::Msr::MSR_IA32_VMX_TRUE_PROCBASED_CTLS
            : arch::Msr::MSR_IA32_VMX_PROCBASED_CTLS
            )
    );

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS), primary_processor_based_control.control);

    vmx::VmxExitControl vmexit_control{ 0 };
    vmexit_control.control = 0;

    vmexit_control.fields.host_address_space_size = true;

    adjust_controls
    (
        vmexit_control.control,
        (
            basic_msr.fields.true_controls
            ? arch::Msr::MSR_IA32_VMX_TRUE_EXIT_CTLS
            : arch::Msr::MSR_IA32_VMX_EXIT_CTLS
            )
    );

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_VMEXIT_CONTROLS), vmexit_control.control);

    vmx::VmxEntryControl vmentry_control{ 0 };
    vmentry_control.control = 0;

    vmentry_control.fields.ia32e_mode_guest = true;

    adjust_controls
    (
        vmentry_control.control,
        (
            basic_msr.fields.true_controls
            ? arch::Msr::MSR_IA32_VMX_TRUE_ENTRY_CTLS
            : arch::Msr::MSR_IA32_VMX_ENTRY_CTLS
            )
    );

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_VMENTRY_CONTROLS), vmentry_control.control);

    vmx::VmxSecondaryProcessorBasedControl secondary_processor_based_control{ 0 };
    secondary_processor_based_control.control = 0;

    secondary_processor_based_control.fields.enable_ept = true;
    secondary_processor_based_control.fields.enable_vpid = false;
    secondary_processor_based_control.fields.enable_rdtscp = true;
    secondary_processor_based_control.fields.enable_invpcid = true;
    secondary_processor_based_control.fields.enable_xsave_xrstor = true;


    adjust_controls(
        secondary_processor_based_control.control,
        arch::Msr::MSR_IA32_VMX_PROCBASED_CTLS2
    );

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS), secondary_processor_based_control.control);

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CS_SELECTOR), __readcs());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_SS_SELECTOR), __readss());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_DS_SELECTOR), __readds());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_ES_SELECTOR), __reades());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_FS_SELECTOR), __readfs());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_GS_SELECTOR), __readgs());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_LDTR_SELECTOR), __readldtr());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_TR_SELECTOR), __readtr());

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CS_LIMIT), __segmentlimit(__readcs()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_SS_LIMIT), __segmentlimit(__readss()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_DS_LIMIT), __segmentlimit(__readds()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_ES_LIMIT), __segmentlimit(__reades()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_FS_LIMIT), __segmentlimit(__readfs()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_GS_LIMIT), __segmentlimit(__readgs()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_LDTR_LIMIT), __segmentlimit(__readldtr()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_TR_LIMIT), __segmentlimit(__readtr()));

    arch::pseudo_descriptor_64_t gdtr;
    arch::pseudo_descriptor_64_t idtr;

    _sgdt(&gdtr);
    __sidt(&idtr);

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_GDTR_LIMIT), gdtr.limit);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_IDTR_LIMIT), idtr.limit);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_GDTR_BASE), gdtr.base_address);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_IDTR_BASE), idtr.base_address);


    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_CS_ACCESS_RIGHTS), read_segment_access_rights(__readcs()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_SS_ACCESS_RIGHTS), read_segment_access_rights(__readss()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_DS_ACCESS_RIGHTS), read_segment_access_rights(__readds()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_ES_ACCESS_RIGHTS), read_segment_access_rights(__reades()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_FS_ACCESS_RIGHTS), read_segment_access_rights(__readfs()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_GS_ACCESS_RIGHTS), read_segment_access_rights(__readgs()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_LDTR_ACCESS_RIGHTS), read_segment_access_rights(__readldtr()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_TR_ACCESS_RIGHTS), read_segment_access_rights(__readtr()));

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_LDTR_BASE), get_segment_base(gdtr.base_address, __readldtr()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_GUEST_TR_BASE), get_segment_base(gdtr.base_address, __readtr()));

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_CS_SELECTOR), __readcs()& SELECTOR_MASK);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_SS_SELECTOR), __readss()& SELECTOR_MASK);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_DS_SELECTOR), __readds()& SELECTOR_MASK);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_ES_SELECTOR), __reades()& SELECTOR_MASK);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_FS_SELECTOR), __readfs()& SELECTOR_MASK);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_GS_SELECTOR), __readgs()& SELECTOR_MASK);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_TR_SELECTOR), __readtr()& SELECTOR_MASK);

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_TR_BASE), get_segment_base(gdtr.base_address, __readtr()));
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_GDTR_BASE), gdtr.base_address);
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_IDTR_BASE), idtr.base_address);

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_CR0), __readcr0());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_CR3), __readcr3());
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_HOST_CR4), __readcr4());

    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_MSR_BITMAP_ADDRESS), vcpu->msr_bitmap_physical);

    ept::Eptp eptp;
    eptp.control = 0;
    eptp.fields.memory_type = static_cast<unsigned __int64>(arch::MtrrType::WB);
    eptp.fields.page_walk_length = 3;
    eptp.fields.pml4_physical = MmGetPhysicalAddress(vcpu->pml4t).QuadPart / PAGE_SIZE;
    
    status |= __vmx_vmwrite(static_cast<size_t>(vmx::VmcsField::VMCS_CTRL_EPT_POINTER), eptp.control);
    
    return !status;
}

static unsigned __int32 read_segment_access_rights(unsigned __int16 segment_selector)
{
    arch::segment_selector_t selector;
    arch::segment_access_rights_t vmx_access_rights;

    selector.control = segment_selector;


    if (selector.fields.table == 0
        && selector.fields.index == 0)
    {
        vmx_access_rights.control = 0;
        vmx_access_rights.fields.unusable = true;
        return vmx_access_rights.control;
    }

    vmx_access_rights.control = (__read_segment_access_rights(segment_selector) >> 8);
    vmx_access_rights.fields.unusable = 0;
    vmx_access_rights.fields.reserved0 = 0;
    vmx_access_rights.fields.reserved1 = 0;

    return vmx_access_rights.control;
}

static unsigned __int64 get_segment_base(unsigned __int64 gdt_base, unsigned __int16 segment_selector)
{
    if (segment_selector == 0 || (segment_selector >> 3) == 0)
    {
        return 0;
    }

    auto descriptor = reinterpret_cast<arch::SystemSegmentDescriptor*>(
        gdt_base + (segment_selector >> 3) * sizeof(arch::SegmentDescriptor));

    return (descriptor->fields.base_0 | (descriptor->fields.base_1 << 24) | (descriptor->fields.base_2 << 32));
}

static void adjust_controls(unsigned __int32& control, arch::Msr msr) {
    arch::VmxTrueControlSettings hint;
    hint.control = __readmsr(static_cast<unsigned __int32>(msr));

    control |= hint.fields.allowed_0_settings;
    control &= hint.fields.allowed_1_settings;
}