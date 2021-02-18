#pragma once
#include <ntddk.h>
#include "context.h"
#include "mtrr.h"

namespace vmx {
    namespace ept {
        constexpr unsigned __int64 EPT_TABLE_ENTRIES = 512;

        bool setup_ept(VCpu* vcpu);
        void cleanup_ept(VCpu* vcpu);
    }
}