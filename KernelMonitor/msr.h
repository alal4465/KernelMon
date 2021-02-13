#pragma once

namespace arch {

	enum class Msr : unsigned __int64 {
		MSR_APIC_BASE = 0x01B,
		MSR_IA32_FEATURE_CONTROL = 0x03A,
		MSR_IA32_VMX_BASIC = 0x480,
		MSR_IA32_VMX_PINBASED_CTLS = 0x481,
		MSR_IA32_VMX_PROCBASED_CTLS = 0x482,
		MSR_IA32_VMX_EXIT_CTLS = 0x483,
		MSR_IA32_VMX_ENTRY_CTLS = 0x484,
		MSR_IA32_VMX_MISC = 0x485,
		MSR_IA32_VMX_CR0_FIXED0 = 0x486,
		MSR_IA32_VMX_CR0_FIXED1 = 0x487,
		MSR_IA32_VMX_CR4_FIXED0 = 0x488,
		MSR_IA32_VMX_CR4_FIXED1 = 0x489,
		MSR_IA32_VMX_VMCS_ENUM = 0x48A,
		MSR_IA32_VMX_PROCBASED_CTLS2 = 0x48B,
		MSR_IA32_VMX_EPT_VPID_CAP = 0x48C,
		MSR_IA32_VMX_TRUE_PINBASED_CTLS = 0x48D,
		MSR_IA32_VMX_TRUE_PROCBASED_CTLS = 0x48E,
		MSR_IA32_VMX_TRUE_EXIT_CTLS = 0x48F,
		MSR_IA32_VMX_TRUE_ENTRY_CTLS = 0x490,
		MSR_IA32_VMX_VMFUNC = 0x491,
		MSR_IA32_SYSENTER_CS = 0x174,
		MSR_IA32_SYSENTER_ESP = 0x175,
		MSR_IA32_SYSENTER_EIP = 0x176,
		MSR_IA32_DEBUGCTL = 0x1D9,
		MSR_LSTAR = 0xC0000082,
		MSR_FS_BASE = 0xC0000100,
		MSR_GS_BASE = 0xC0000101,
		MSR_SHADOW_GS_BASE = 0xC0000102,
		MSR_IA32_EFER = 0xc0000080
	};

	union Ia32EferMsr
	{
		unsigned __int64 control;
		struct
		{
			unsigned __int64 syscall_enable : 1;
			unsigned __int64 reserved_0 : 7;
			unsigned __int64 long_mode_enable : 1;
			unsigned __int64 reserved_1 : 1;
			unsigned __int64 long_mode_active : 1;
			unsigned __int64 execute_disable : 1;
			unsigned __int64 reserved_2 : 52;
		}fields;
	};

	union Ia32FeatureControlMsr
	{
		unsigned __int64 control;
		struct
		{
			unsigned __int64 lock : 1;
			unsigned __int64 vmxon_inside_smx : 1;
			unsigned __int64 vmxon_outside_smx : 1;
			unsigned __int64 reserved_0 : 5;
			unsigned __int64 senter_local : 6;
			unsigned __int64 senter_global : 1;
			unsigned __int64 reserved_1 : 1;
			unsigned __int64 sgx_launch_control_enable : 1;
			unsigned __int64 sgx_global_enable : 1;
			unsigned __int64 reserved_2 : 1;
			unsigned __int64 lmce : 1;
			unsigned __int64 system_reserved : 42;
		}fields;
	};

	union Ia32VmxMiscMsr
	{
		unsigned __int64 control;
		struct
		{
			unsigned __int64 vmx_preemption_tsc_rate : 5;
			unsigned __int64 store_lma_in_vmentry_control : 1;
			unsigned __int64 activate_state_bitmap : 3;
			unsigned __int64 reserved_0 : 5;
			unsigned __int64 pt_in_vmx : 1;
			unsigned __int64 rdmsr_in_smm : 1;
			unsigned __int64 cr3_target_value_count : 9;
			unsigned __int64 max_msr_vmexit : 3;
			unsigned __int64 allow_smi_blocking : 1;
			unsigned __int64 vmwrite_to_any : 1;
			unsigned __int64 interrupt_mod : 1;
			unsigned __int64 reserved_1 : 1;
			unsigned __int64 mseg_revision_identifier : 32;
		}fields;
	};

	union Ia32VmxBasicMsr
	{
		unsigned __int64 control;
		struct
		{
			unsigned __int64 vmcs_revision_identifier : 31;
			unsigned __int64 always_0 : 1;
			unsigned __int64 vmxon_region_size : 13;
			unsigned __int64 reserved_1 : 3;
			unsigned __int64 vmxon_physical_address_width : 1;
			unsigned __int64 dual_monitor_smi : 1;
			unsigned __int64 memory_type : 4;
			unsigned __int64 io_instruction_reporting : 1;
			unsigned __int64 true_controls : 1;
		}fields;
	};

	union Ia32VmxPrimaryProcessorBasedCtlMsr
	{
		unsigned __int64 control;
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
		}fields;
	};

	union Ia32VmxSecondaryProcessorBasedCtlMsr
	{
		unsigned __int64 control;
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
		}fields;
	};

	union Ia32VmxCtlMsr
	{
		unsigned __int64 control;
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
		}fields;
	};

	union Ia32VmxEntryCtlMsr
	{
		unsigned __int64 control;
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
		}fields;
	};

#pragma pack(push, 1)
	union VmxTrueControlSettings
	{
		unsigned __int64 control;
		struct
		{
			unsigned __int32 allowed_0_settings;
			unsigned __int32 allowed_1_settings;
		} fields;
	};
#pragma pack(pop)

}