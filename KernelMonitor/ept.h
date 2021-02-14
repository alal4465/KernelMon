#pragma once
#include "context.h"

namespace vmx {
    namespace ept {
        void setup_ept(VCpu* vcpu);
        void cleanup_ept(VCpu* vcpu);
    }
}