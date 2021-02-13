#pragma once
#include <ntddk.h>
#include <intrin.h>

namespace arch {
    enum class CpuidCode {
        VENDOR_ID,
        FEATURE_INFO
    };

    struct cpuid_feature_info_t {
        union
        {
            unsigned __int32 control;
        }eax;

        union
        {
            unsigned __int32 control;
        }ebx;

        union
        {
            unsigned __int32 control;

            struct
            {
                unsigned __int64 sse3 : 1;
                unsigned __int64 pclmul : 1;
                unsigned __int64 dtes64 : 1;
                unsigned __int64 monitor : 1;
                unsigned __int64 ds_cpl : 1;
                unsigned __int64 vmx : 1;
                unsigned __int64 smx : 1;
                unsigned __int64 est : 1;
                unsigned __int64 tm2 : 1;
                unsigned __int64 ssse3 : 1;
                unsigned __int64 cid : 1;
                unsigned __int64 sdbg : 1;
                unsigned __int64 fma : 1;
                unsigned __int64 cx16 : 1;
                unsigned __int64 xtpr : 1;
                unsigned __int64 pdcm : 1;
                unsigned __int64 reserved : 1;
                unsigned __int64 pcid : 1;
                unsigned __int64 dca : 1;
                unsigned __int64 sse4_1 : 1;
                unsigned __int64 sse4_2 : 1;
                unsigned __int64 x2apic : 1;
                unsigned __int64 movbe : 1;
                unsigned __int64 popcnt : 1;
                unsigned __int64 tsc_deadline : 1;
                unsigned __int64 aes : 1;
                unsigned __int64 xsave : 1;
                unsigned __int64 osxsave : 1;
                unsigned __int64 avx : 1;
                unsigned __int64 f16c : 1;
                unsigned __int64 rdrnd : 1;
                unsigned __int64 hypervisor : 1;
            } features;
        }ecx;

        union
        {
            unsigned __int32 control;
        }edx;
    };

    struct cpuid_vendor_name_t {
        unsigned __int32 highest_calling_param;
        char vendor_name_start[4];
        char vendor_name_end[4];
        char vendor_name_middle[4];
    };
}

namespace cpuid {
    constexpr const char* vendor_name = "GenuineIntel";

    bool is_vendor_intel();
    bool is_vmx_supported();
}