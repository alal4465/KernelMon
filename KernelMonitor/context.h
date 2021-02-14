#pragma once
#include <ntddk.h>
#include <intrin.h>
#include "cr.h"
#include "msr.h"
#include "memory.h"

namespace vmx {
	constexpr unsigned __int64 VMXON_REGION_SIZE = 0x1000;
	constexpr unsigned __int64 VMCS_REGION_SIZE = 0x1000;
	constexpr unsigned __int64 VMM_STACK_SIZE = 0x1000;

	struct Vmxon {
		unsigned __int32 revision_identifier;
		unsigned char vmxon_data[VMXON_REGION_SIZE - sizeof(unsigned __int32)];
	};

	struct Vmcs {
		unsigned __int32 revision_identifier : 31;
		unsigned __int32 shadow_vmcs_indicator : 1;
		unsigned __int32 vmx_abort_indicator;
		unsigned char vmcs_data[VMCS_REGION_SIZE - (2 * sizeof(unsigned __int32))];
	};

    namespace ept {

        union Eptp {
            unsigned __int64 control;
            struct {
                unsigned __int64 memory_type : 3;
                unsigned __int64 page_walk_length : 3;
                unsigned __int64 enable_dirty_access_bits : 1;
                unsigned __int64 reserved1 : 5;
                unsigned __int64 pml4_physical : 36;
                unsigned __int64 reserved2 : 16;
            }fields;
        };

        union Pml4e {
            unsigned __int64 control;
            struct {
                unsigned __int64 read : 1;
                unsigned __int64 write : 1;
                unsigned __int64 exec : 1;
                unsigned __int64 reserved1 : 5;
                unsigned __int64 accessed : 1;
                unsigned __int64 ignored1 : 1;
                unsigned __int64 exec_for_usermode : 1;
                unsigned __int64 ignored2 : 1;
                unsigned __int64 pdpt_physical : 36;
                unsigned __int64 reserved2 : 4;
                unsigned __int64 ignored3 : 12;
            }fields;
        };

        union Pdpte {
            unsigned __int64 control;
            struct {
                unsigned __int64 read : 1;
                unsigned __int64 write : 1;
                unsigned __int64 execute : 1;
                unsigned __int64 reserved1 : 5;
                unsigned __int64 accessed : 1;
                unsigned __int64 ignored1 : 1;
                unsigned __int64 exec_for_usermode : 1;
                unsigned __int64 ignored2 : 1;
                unsigned __int64 pdt_physical : 36;
                unsigned __int64 reserved2 : 4;
                unsigned __int64 ignored3 : 12;
            }fields;
        };

        union Pdte {
            unsigned __int64 control;
            struct {
                unsigned __int64 read : 1;
                unsigned __int64 write : 1;
                unsigned __int64 execute : 1;
                unsigned __int64 reserved1 : 5;
                unsigned __int64 accessed : 1;
                unsigned __int64 ignored1 : 1;
                unsigned __int64 exec_for_usermode : 1;
                unsigned __int64 ignored2 : 1;
                unsigned __int64 pt_physical : 36;
                unsigned __int64 reserved2 : 4;
                unsigned __int64 ignored3 : 12;
            }fields;
        };

        union Pte {
            unsigned __int64 control;
            struct {
                unsigned __int64 read : 1;
                unsigned __int64 write : 1;
                unsigned __int64 execute : 1;
                unsigned __int64 mem_type : 3;
                unsigned __int64 ignore_pat : 1;
                unsigned __int64 ignored1 : 1;
                unsigned __int64 accessed_flag : 1;
                unsigned __int64 dirty_flag : 1;
                unsigned __int64 exec_for_usermode : 1;
                unsigned __int64 ignored2 : 1;
                unsigned __int64 page_physical : 36;
                unsigned __int64 reserved : 4;
                unsigned __int64 ignored3 : 11;
                unsigned __int64 suppress_ve : 1;
            }fields;
        };
    }

	struct VCpu {
		Vmcs* vmcs;
		unsigned __int64 vmcs_physical;

		Vmxon* vmxon;
		unsigned __int64 vmxon_physical;

		void* msr_bitmap;
		unsigned __int64 msr_bitmap_physical;

        ept::Eptp eptp;
        ept::Pml4e* pml4t;
        ept::Pdpte* pdpt;
        ept::Pdte* pdt;

		void* stack;
	};

	struct VmmContext {
		unsigned __int32 processor_count;
		VCpu* vcpu_table;
	};

	VmmContext* alloc_vmm_context();
	void cleanup_vmm_context(VmmContext* vmm_context);
}