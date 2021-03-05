#pragma once
#include <ntddk.h>
#include <intrin.h>
#include "cr.h"
#include "msr.h"
#include "context.h"
#include "mtrr.h"

// this intrinsic is for some reason left out of intrin.h
extern "C" void _sgdt(void*);

namespace vmx {
    bool setup_vmcs_fields(vmx::VCpu* vcpu, PVOID guest_rsp);

    enum class VmcsField : unsigned __int64
    {
        VMCS_CTRL_VIRTUAL_PROCESSOR_IDENTIFIER = 0x0,
        VMCS_CTRL_POSTED_INTERRUPT_NOTIFICATION_VECTOR = 0x2,
        VMCS_CTRL_EPTP_INDEX = 0x4,
        VMCS_GUEST_ES_SELECTOR = 0x800,
        VMCS_GUEST_CS_SELECTOR = 0x802,
        VMCS_GUEST_SS_SELECTOR = 0x804,
        VMCS_GUEST_DS_SELECTOR = 0x806,
        VMCS_GUEST_FS_SELECTOR = 0x808,
        VMCS_GUEST_GS_SELECTOR = 0x80a,
        VMCS_GUEST_LDTR_SELECTOR = 0x80c,
        VMCS_GUEST_TR_SELECTOR = 0x80e,
        VMCS_GUEST_INTERRUPT_STATUS = 0x810,
        VMCS_GUEST_PML_INDEX = 0x812,
        VMCS_HOST_ES_SELECTOR = 0xc00,
        VMCS_HOST_CS_SELECTOR = 0xc02,
        VMCS_HOST_SS_SELECTOR = 0xc04,
        VMCS_HOST_DS_SELECTOR = 0xc06,
        VMCS_HOST_FS_SELECTOR = 0xc08,
        VMCS_HOST_GS_SELECTOR = 0xc0a,
        VMCS_HOST_TR_SELECTOR = 0xc0c,
        VMCS_CTRL_IO_BITMAP_A_ADDRESS = 0x2000,
        VMCS_CTRL_IO_BITMAP_B_ADDRESS = 0x2002,
        VMCS_CTRL_MSR_BITMAP_ADDRESS = 0x2004,
        VMCS_CTRL_VMEXIT_MSR_STORE_ADDRESS = 0x2006,
        VMCS_CTRL_VMEXIT_MSR_LOAD_ADDRESS = 0x2008,
        VMCS_CTRL_VMENTRY_MSR_LOAD_ADDRESS = 0x200a,
        VMCS_CTRL_EXECUTIVE_VMCS_POINTER = 0x200c,
        VMCS_CTRL_PML_ADDRESS = 0x200e,
        VMCS_CTRL_TSC_OFFSET = 0x2010,
        VMCS_CTRL_VIRTUAL_APIC_ADDRESS = 0x2012,
        VMCS_CTRL_APIC_ACCESS_ADDRESS = 0x2014,
        VMCS_CTRL_POSTED_INTERRUPT_DESCRIPTOR_ADDRESS = 0x2016,
        VMCS_CTRL_VMFUNC_CONTROLS = 0x2018,
        VMCS_CTRL_EPT_POINTER = 0x201a,
        VMCS_CTRL_EOI_EXIT_BITMAP_0 = 0x201c,
        VMCS_CTRL_EOI_EXIT_BITMAP_1 = 0x201e,
        VMCS_CTRL_EOI_EXIT_BITMAP_2 = 0x2020,
        VMCS_CTRL_EOI_EXIT_BITMAP_3 = 0x2022,
        VMCS_CTRL_EPT_POINTER_LIST_ADDRESS = 0x2024,
        VMCS_CTRL_VMREAD_BITMAP_ADDRESS = 0x2026,
        VMCS_CTRL_VMWRITE_BITMAP_ADDRESS = 0x2028,
        VMCS_CTRL_VIRTUALIZATION_EXCEPTION_INFORMATION_ADDRESS = 0x202a,
        VMCS_CTRL_XSS_EXITING_BITMAP = 0x202c,
        VMCS_CTRL_ENCLS_EXITING_BITMAP = 0x202e,
        VMCS_CTRL_TSC_MULTIPLIER = 0x2032,
        VMCS_GUEST_PHYSICAL_ADDRESS = 0x2400,
        VMCS_GUEST_VMCS_LINK_POINTER = 0x2800,
        VMCS_GUEST_DEBUGCTL = 0x2802,
        VMCS_GUEST_PAT = 0x2804,
        VMCS_GUEST_EFER = 0x2806,
        VMCS_GUEST_PERF_GLOBAL_CTRL = 0x2808,
        VMCS_GUEST_PDPTE0 = 0x280a,
        VMCS_GUEST_PDPTE1 = 0x280c,
        VMCS_GUEST_PDPTE2 = 0x280e,
        VMCS_GUEST_PDPTE3 = 0x2810,
        VMCS_HOST_PAT = 0x2c00,
        VMCS_HOST_EFER = 0x2c02,
        VMCS_HOST_PERF_GLOBAL_CTRL = 0x2c04,
        VMCS_CTRL_PIN_BASED_VM_EXECUTION_CONTROLS = 0x4000,
        VMCS_CTRL_PROCESSOR_BASED_VM_EXECUTION_CONTROLS = 0x4002,
        VMCS_CTRL_EXCEPTION_BITMAP = 0x4004,
        VMCS_CTRL_PAGEFAULT_ERROR_CODE_MASK = 0x4006,
        VMCS_CTRL_PAGEFAULT_ERROR_CODE_MATCH = 0x4008,
        VMCS_CTRL_CR3_TARGET_COUNT = 0x400a,
        VMCS_CTRL_VMEXIT_CONTROLS = 0x400c,
        VMCS_CTRL_VMEXIT_MSR_STORE_COUNT = 0x400e,
        VMCS_CTRL_VMEXIT_MSR_LOAD_COUNT = 0x4010,
        VMCS_CTRL_VMENTRY_CONTROLS = 0x4012,
        VMCS_CTRL_VMENTRY_MSR_LOAD_COUNT = 0x4014,
        VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD = 0x4016,
        VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE = 0x4018,
        VMCS_CTRL_VMENTRY_INSTRUCTION_LENGTH = 0x401a,
        VMCS_CTRL_TPR_THRESHOLD = 0x401c,
        VMCS_CTRL_SECONDARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS = 0x401e,
        VMCS_CTRL_PLE_GAP = 0x4020,
        VMCS_CTRL_PLE_WINDOW = 0x4022,
        VMCS_VM_INSTRUCTION_ERROR = 0x4400,
        VMCS_EXIT_REASON = 0x4402,
        VMCS_VMEXIT_INTERRUPTION_INFORMATION = 0x4404,
        VMCS_VMEXIT_INTERRUPTION_ERROR_CODE = 0x4406,
        VMCS_IDT_VECTORING_INFORMATION = 0x4408,
        VMCS_IDT_VECTORING_ERROR_CODE = 0x440a,
        VMCS_VMEXIT_INSTRUCTION_LENGTH = 0x440c,
        VMCS_VMEXIT_INSTRUCTION_INFO = 0x440e,
        VMCS_GUEST_ES_LIMIT = 0x4800,
        VMCS_GUEST_CS_LIMIT = 0x4802,
        VMCS_GUEST_SS_LIMIT = 0x4804,
        VMCS_GUEST_DS_LIMIT = 0x4806,
        VMCS_GUEST_FS_LIMIT = 0x4808,
        VMCS_GUEST_GS_LIMIT = 0x480a,
        VMCS_GUEST_LDTR_LIMIT = 0x480c,
        VMCS_GUEST_TR_LIMIT = 0x480e,
        VMCS_GUEST_GDTR_LIMIT = 0x4810,
        VMCS_GUEST_IDTR_LIMIT = 0x4812,
        VMCS_GUEST_ES_ACCESS_RIGHTS = 0x4814,
        VMCS_GUEST_CS_ACCESS_RIGHTS = 0x4816,
        VMCS_GUEST_SS_ACCESS_RIGHTS = 0x4818,
        VMCS_GUEST_DS_ACCESS_RIGHTS = 0x481a,
        VMCS_GUEST_FS_ACCESS_RIGHTS = 0x481c,
        VMCS_GUEST_GS_ACCESS_RIGHTS = 0x481e,
        VMCS_GUEST_LDTR_ACCESS_RIGHTS = 0x4820,
        VMCS_GUEST_TR_ACCESS_RIGHTS = 0x4822,
        VMCS_GUEST_INTERRUPTIBILITY_STATE = 0x4824,
        VMCS_GUEST_ACTIVITY_STATE = 0x4826,
        VMCS_GUEST_SMBASE = 0x4828,
        VMCS_GUEST_SYSENTER_CS = 0x482a,
        VMCS_GUEST_VMX_PREEMPTION_TIMER_VALUE = 0x482e,
        VMCS_SYSENTER_CS = 0x4c00,
        VMCS_CTRL_CR0_GUEST_HOST_MASK = 0x6000,
        VMCS_CTRL_CR4_GUEST_HOST_MASK = 0x6002,
        VMCS_CTRL_CR0_READ_SHADOW = 0x6004,
        VMCS_CTRL_CR4_READ_SHADOW = 0x6006,
        VMCS_CTRL_CR3_TARGET_VALUE_0 = 0x6008,
        VMCS_CTRL_CR3_TARGET_VALUE_1 = 0x600a,
        VMCS_CTRL_CR3_TARGET_VALUE_2 = 0x600c,
        VMCS_CTRL_CR3_TARGET_VALUE_3 = 0x600e,
        VMCS_EXIT_QUALIFICATION = 0x6400,
        VMCS_IO_RCX = 0x6402,
        VMCS_IO_RSX = 0x6404,
        VMCS_IO_RDI = 0x6406,
        VMCS_IO_RIP = 0x6408,
        VMCS_EXIT_GUEST_LINEAR_ADDRESS = 0x640a,
        VMCS_GUEST_CR0 = 0x6800,
        VMCS_GUEST_CR3 = 0x6802,
        VMCS_GUEST_CR4 = 0x6804,
        VMCS_GUEST_ES_BASE = 0x6806,
        VMCS_GUEST_CS_BASE = 0x6808,
        VMCS_GUEST_SS_BASE = 0x680a,
        VMCS_GUEST_DS_BASE = 0x680c,
        VMCS_GUEST_FS_BASE = 0x680e,
        VMCS_GUEST_GS_BASE = 0x6810,
        VMCS_GUEST_LDTR_BASE = 0x6812,
        VMCS_GUEST_TR_BASE = 0x6814,
        VMCS_GUEST_GDTR_BASE = 0x6816,
        VMCS_GUEST_IDTR_BASE = 0x6818,
        VMCS_GUEST_DR7 = 0x681a,
        VMCS_GUEST_RSP = 0x681c,
        VMCS_GUEST_RIP = 0x681e,
        VMCS_GUEST_RFLAGS = 0x6820,
        VMCS_GUEST_PENDING_DEBUG_EXCEPTIONS = 0x6822,
        VMCS_GUEST_SYSENTER_ESP = 0x6824,
        VMCS_GUEST_SYSENTER_EIP = 0x6826,
        VMCS_HOST_CR0 = 0x6c00,
        VMCS_HOST_CR3 = 0x6c02,
        VMCS_HOST_CR4 = 0x6c04,
        VMCS_HOST_FS_BASE = 0x6c06,
        VMCS_HOST_GS_BASE = 0x6c08,
        VMCS_HOST_TR_BASE = 0x6c0a,
        VMCS_HOST_GDTR_BASE = 0x6c0c,
        VMCS_HOST_IDTR_BASE = 0x6c0e,
        VMCS_HOST_SYSENTER_ESP = 0x6c10,
        VMCS_HOST_SYSENTER_EIP = 0x6c12,
        VMCS_HOST_RSP = 0x6c14,
        VMCS_HOST_RIP = 0x6c16
    };

    union VmxPrimaryProcessorBasedControl
    {
        unsigned __int32 control;
        struct
        {
            unsigned __int64 reserved_0 : 2;
            unsigned __int64 interrupt_window_exiting : 1;
            unsigned __int64 use_tsc_offsetting : 1;
            unsigned __int64 reserved_1 : 3;
            unsigned __int64 hlt_exiting : 1;
            unsigned __int64 reserved_2 : 1;
            unsigned __int64 invldpg_exiting : 1;
            unsigned __int64 mwait_exiting : 1;
            unsigned __int64 rdpmc_exiting : 1;
            unsigned __int64 rdtsc_exiting : 1;
            unsigned __int64 reserved_3 : 2;
            unsigned __int64 cr3_load_exiting : 1;
            unsigned __int64 cr3_store_exiting : 1;
            unsigned __int64 reserved_4 : 2;
            unsigned __int64 cr8_load_exiting : 1;
            unsigned __int64 cr8_store_exiting : 1;
            unsigned __int64 use_tpr_shadow : 1;
            unsigned __int64 nmi_window_exiting : 1;
            unsigned __int64 mov_dr_exiting : 1;
            unsigned __int64 unconditional_io_exiting : 1;
            unsigned __int64 use_io_bitmaps : 1;
            unsigned __int64 reserved_5 : 1;
            unsigned __int64 monitor_trap_flag : 1;
            unsigned __int64 use_msr_bitmaps : 1;
            unsigned __int64 monitor_exiting : 1;
            unsigned __int64 pause_exiting : 1;
            unsigned __int64 active_secondary_controls : 1;
        } fields;
    };

    union VmxSecondaryProcessorBasedControl
    {
        unsigned __int32 control;
        struct
        {
            unsigned __int64 virtualize_apic_accesses : 1;
            unsigned __int64 enable_ept : 1;
            unsigned __int64 descriptor_table_exiting : 1;
            unsigned __int64 enable_rdtscp : 1;
            unsigned __int64 virtualize_x2apic : 1;
            unsigned __int64 enable_vpid : 1;
            unsigned __int64 wbinvd_exiting : 1;
            unsigned __int64 unrestricted_guest : 1;
            unsigned __int64 apic_register_virtualization : 1;
            unsigned __int64 virtual_interrupt_delivery : 1;
            unsigned __int64 pause_loop_exiting : 1;
            unsigned __int64 rdrand_exiting : 1;
            unsigned __int64 enable_invpcid : 1;
            unsigned __int64 enable_vmfunc : 1;
            unsigned __int64 vmcs_shadowing : 1;
            unsigned __int64 enable_encls_exiting : 1;
            unsigned __int64 rdseed_exiting : 1;
            unsigned __int64 enable_pml : 1;
            unsigned __int64 use_virtualization_exception : 1;
            unsigned __int64 conceal_vmx_from_pt : 1;
            unsigned __int64 enable_xsave_xrstor : 1;
            unsigned __int64 reserved_0 : 1;
            unsigned __int64 mode_based_execute_control_ept : 1;
            unsigned __int64 reserved_1 : 2;
            unsigned __int64 use_tsc_scaling : 1;
        } fields;
    };

    union Ia32VmxPinbasedControl
    {
        unsigned __int32 control;
        struct
        {
            unsigned __int64 external_interrupt_exiting : 1;
            unsigned __int64 reserved_0 : 2;
            unsigned __int64 nmi_exiting : 1;
            unsigned __int64 reserved_1 : 1;
            unsigned __int64 virtual_nmis : 1;
            unsigned __int64 vmx_preemption_timer : 1;
            unsigned __int64 process_posted_interrupts : 1;
        } fields;
    };

    union VmxExitControl
    {
        unsigned __int32 control;
        struct
        {
            unsigned __int64 reserved_0 : 2;
            unsigned __int64 save_dbg_controls : 1;
            unsigned __int64 reserved_1 : 6;
            unsigned __int64 host_address_space_size : 1;
            unsigned __int64 reserved_2 : 2;
            unsigned __int64 load_ia32_perf_global_control : 1;
            unsigned __int64 reserved_3 : 2;
            unsigned __int64 ack_interrupt_on_exit : 1;
            unsigned __int64 reserved_4 : 2;
            unsigned __int64 save_ia32_pat : 1;
            unsigned __int64 load_ia32_pat : 1;
            unsigned __int64 save_ia32_efer : 1;
            unsigned __int64 load_ia32_efer : 1;
            unsigned __int64 save_vmx_preemption_timer_value : 1;
            unsigned __int64 clear_ia32_bndcfgs : 1;
            unsigned __int64 conceal_vmx_from_pt : 1;
        } fields;
    };

    union VmxEntryControl
    {
        unsigned __int32 control;
        struct
        {
            unsigned __int64 reserved_0 : 2;
            unsigned __int64 load_dbg_controls : 1;
            unsigned __int64 reserved_1 : 6;
            unsigned __int64 ia32e_mode_guest : 1;
            unsigned __int64 entry_to_smm : 1;
            unsigned __int64 deactivate_dual_monitor_treament : 1;
            unsigned __int64 reserved_3 : 1;
            unsigned __int64 load_ia32_perf_global_control : 1;
            unsigned __int64 load_ia32_pat : 1;
            unsigned __int64 load_ia32_efer : 1;
            unsigned __int64 load_ia32_bndcfgs : 1;
            unsigned __int64 conceal_vmx_from_pt : 1;
        } fields;
    };
}

namespace arch {
#pragma pack(push, 1)
    struct pseudo_descriptor_64_t
    {
        unsigned __int16 limit;
        unsigned __int64 base_address;
    };
#pragma pack(pop)

    struct segment_descriptor_64_t
    {
        unsigned __int16 segment_limit_low;
        unsigned __int16 base_low;
        union
        {
            struct
            {
                unsigned __int32 base_middle : 8;
                unsigned __int32 type : 4;
                unsigned __int32 descriptor_type : 1;
                unsigned __int32 dpl : 2;
                unsigned __int32 present : 1;
                unsigned __int32 segment_limit_high : 4;
                unsigned __int32 system : 1;
                unsigned __int32 long_mode : 1;
                unsigned __int32 default_big : 1;
                unsigned __int32 granularity : 1;
                unsigned __int32 base_high : 8;
            } fields;
            unsigned __int32 flags;
        };
        unsigned __int32 base_upper;
        unsigned __int32 reserved;
    };

    struct segment_descriptor_32_t
    {
        unsigned __int16 segment_limit_low;
        unsigned __int16 base_low;
        union
        {
            struct
            {
                unsigned __int32 base_middle : 8;
                unsigned __int32 type : 4;
                unsigned __int32 descriptor_type : 1;
                unsigned __int32 dpl : 2;
                unsigned __int32 present : 1;
                unsigned __int32 segment_limit_high : 4;
                unsigned __int32 system : 1;
                unsigned __int32 long_mode : 1;
                unsigned __int32 default_big : 1;
                unsigned __int32 granularity : 1;
                unsigned __int32 base_high : 8;
            } fields;
            unsigned __int32 flags;
        };
    };

    union segment_selector_t
    {
        struct
        {
            unsigned __int16 rpl : 2;
            unsigned __int16 table : 1;
            unsigned __int16 index : 13;
        }fields;
        unsigned __int16 control;
    };

    union segment_access_rights_t
    {
        struct
        {
            unsigned __int32 type : 4;
            unsigned __int32 descriptor_type : 1;
            unsigned __int32 dpl : 2;
            unsigned __int32 present : 1;
            unsigned __int32 reserved0 : 4;
            unsigned __int32 available : 1;
            unsigned __int32 long_mode : 1;
            unsigned __int32 default_big : 1;
            unsigned __int32 granularity : 1;
            unsigned __int32 unusable : 1;
            unsigned __int32 reserved1 : 15;
        }fields;
        unsigned __int32 control;
    };

    union SegmentDescriptor
    {
        unsigned long long raw;

        struct
        {
            unsigned long long limit_0 : 16;
            unsigned long long base_0 : 24;
            unsigned long long attr_0 : 8;
            unsigned long long limit_1 : 4;
            unsigned long long attr_1 : 4;
            unsigned long long base_1 : 8;
        }fields;
    };

    union SystemSegmentDescriptor
    {
        struct
        {
            unsigned long long raw_1;
            unsigned long long raw_2;
        }control;

        struct
        {
            unsigned long long limit_0 : 16;
            unsigned long long base_0 : 24;
            unsigned long long attr_0 : 8;
            unsigned long long limit_1 : 4;
            unsigned long long attr_1 : 4;
            unsigned long long base_1 : 8;
            unsigned long long base_2 : 32;
            unsigned long long : 32;
        }fields;
    };
}