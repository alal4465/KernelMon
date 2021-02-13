#pragma once

namespace arch {

	union Cr0 {
		unsigned __int64 control;

		struct {
			unsigned __int64 protected_mode_enable : 1;
			unsigned __int64 monitor_co_processor : 1;
			unsigned __int64 emulation : 1;
			unsigned __int64 task_switched : 1;
			unsigned __int64 extension_type : 1;
			unsigned __int64 numeric_error : 1;
			unsigned __int64 reserved1 : 10;
			unsigned __int64 write_protect : 1;
			unsigned __int64 reserved2 : 1;
			unsigned __int64 alignment_mask : 1;
			unsigned __int64 reserved3 : 10;
			unsigned __int64 not_write_through : 1;
			unsigned __int64 cache_disable : 1;
			unsigned __int64 paging : 1;
			unsigned __int64 reserved4 : 32;
		}fields;
	};

	union Cr4 {
		unsigned __int64 control;

		struct {
			unsigned __int64 virtual_mode_extentions : 1;
			unsigned __int64 pm_virtual_interrupts : 1;
			unsigned __int64 time_stamp_disable : 1;
			unsigned __int64 dbg_extensions : 1;
			unsigned __int64 page_size_extension : 1;
			unsigned __int64 phys_addr_extension : 1;
			unsigned __int64 machine_check_exception : 1;
			unsigned __int64 page_global_enable : 1;
			unsigned __int64 pmc_enable : 1;
			unsigned __int64 osfxsr_support : 1;
			unsigned __int64 osxmmexcpt_support : 1;
			unsigned __int64 um_instruction_prevention : 1;
			unsigned __int64 reserved1 : 1;
			unsigned __int64 vmx_enable : 1;
			unsigned __int64 smx_enable : 1;
			unsigned __int64 reserved2 : 2;
			unsigned __int64 pcid_enable : 1;
			unsigned __int64 osxsave : 1;
			unsigned __int64 reserved3 : 1;
			unsigned __int64 smep_enable : 1;
			unsigned __int64 smap_enable : 1;
			unsigned __int64 reserved4 : 42;
		}fields;
	};

	union InterruptCommandRegister
	{
		unsigned __int64 control;
		struct
		{
			unsigned __int64 vector : 8;
			unsigned __int64 delivery_mode : 3;
			unsigned __int64 destination_mode : 1;
			unsigned __int64 delivery_status : 1;
			unsigned __int64 reserved_0 : 1;
			unsigned __int64 level : 1;
			unsigned __int64 trigger_mode : 1;
			unsigned __int64 reserved_1 : 2;
			unsigned __int64 destination_short : 2;
			unsigned __int64 reserved_3 : 35;
			unsigned __int64 destination : 8;
		} fields;
	};

}