#pragma once
#include <ntddk.h>
#include "context.h"
#include "mtrr.h"
#include "asm_stubs.h"

namespace vmx {
    namespace ept {
        constexpr unsigned __int64 EPT_TABLE_ENTRIES = 512;
        constexpr unsigned __int64 LARGE_PAGE_SIZE = 0x200000;
        constexpr unsigned __int64 SPLIT_LARGE_PDE_PAGES = LARGE_PAGE_SIZE / PAGE_SIZE;

        bool setup_ept(VCpu* vcpu);
        void cleanup_ept(VCpu* vcpu);
        void split_large_pdte(LargePdte* pde);
    }
}