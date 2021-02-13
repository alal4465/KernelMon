#pragma once
#include <ntddk.h>
#include <intrin.h>
#include "cpuid.h"
#include "vmcs_fields.h"

namespace arch {
    union CrExitQualifiction
    {
        unsigned __int64 control;

        struct
        {
            unsigned __int64 cr_number : 4;
            unsigned __int64 access_type : 2;
            unsigned __int64 lmsw_operand_type : 1;
            unsigned __int64 reserved0 : 1;
            unsigned __int64 mov_cr_gp_register : 4;
            unsigned __int64 reserved1 : 4;
            unsigned __int64 lmsw_source_data : 16;
        }fields;
    };

    struct xmm_t {
        unsigned __int64 low;
        unsigned __int64 high;
    };

    enum class CrAccessType : unsigned __int32 {
        MOV_TO_CR,
        MOV_FROM_CR,
        CLTS,
        LMSW
    };
}
namespace vmx {

    struct VmexitGuestContext
    {
        unsigned __int64 rax;
        unsigned __int64 rcx;
        unsigned __int64 rdx;
        unsigned __int64 rbx;
        unsigned __int64 rbp;
        unsigned __int64 rsi;
        unsigned __int64 rdi;
        unsigned __int64 r8;
        unsigned __int64 r9;
        unsigned __int64 r10;
        unsigned __int64 r11;
        unsigned __int64 r12;
        unsigned __int64 r13;
        unsigned __int64 r14;
        unsigned __int64 r15;
        //arch::xmm_t xmm5;
        //arch::xmm_t xmm4;
        //arch::xmm_t xmm3;
        //arch::xmm_t xmm2;
        //arch::xmm_t xmm1;
        //arch::xmm_t xmm0;
    };


    enum class VmExitReason : unsigned __int64 {
        EXIT_REASON_EXCEPTION_NMI = 0,
        EXIT_REASON_EXTERNAL_INTERRUPT = 1,
        EXIT_REASON_TRIPLE_FAULT = 2,
        EXIT_REASON_INIT = 3,
        EXIT_REASON_SIPI = 4,
        EXIT_REASON_IO_SMI = 5,
        EXIT_REASON_OTHER_SMI = 6,
        EXIT_REASON_PENDING_VIRT_INTR = 7,
        EXIT_REASON_PENDING_VIRT_NMI = 8,
        EXIT_REASON_TASK_SWITCH = 9,
        EXIT_REASON_CPUID = 10,
        EXIT_REASON_GETSEC = 11,
        EXIT_REASON_HLT = 12,
        EXIT_REASON_INVD = 13,
        EXIT_REASON_INVLPG = 14,
        EXIT_REASON_RDPMC = 15,
        EXIT_REASON_RDTSC = 16,
        EXIT_REASON_RSM = 17,
        EXIT_REASON_VMCALL = 18,
        EXIT_REASON_VMCLEAR = 19,
        EXIT_REASON_VMLAUNCH = 20,
        EXIT_REASON_VMPTRLD = 21,
        EXIT_REASON_VMPTRST = 22,
        EXIT_REASON_VMREAD = 23,
        EXIT_REASON_VMRESUME = 24,
        EXIT_REASON_VMWRITE = 25,
        EXIT_REASON_VMXOFF = 26,
        EXIT_REASON_VMXON = 27,
        EXIT_REASON_CR_ACCESS = 28,
        EXIT_REASON_DR_ACCESS = 29,
        EXIT_REASON_IO_INSTRUCTION = 30,
        EXIT_REASON_MSR_READ = 31,
        EXIT_REASON_MSR_WRITE = 32,
        EXIT_REASON_INVALID_GUEST_STATE = 33,
        EXIT_REASON_MSR_LOADING = 34,
        EXIT_REASON_MWAIT_INSTRUCTION = 36,
        EXIT_REASON_MONITOR_TRAP_FLAG = 37,
        EXIT_REASON_MONITOR_INSTRUCTION = 39,
        EXIT_REASON_PAUSE_INSTRUCTION = 40,
        EXIT_REASON_MCE_DURING_VMENTRY = 41,
        EXIT_REASON_TPR_BELOW_THRESHOLD = 43,
        EXIT_REASON_APIC_ACCESS = 44,
        EXIT_REASON_ACCESS_GDTR_OR_IDTR = 46,
        EXIT_REASON_ACCESS_LDTR_OR_TR = 47,
        EXIT_REASON_EPT_VIOLATION = 48,
        EXIT_REASON_EPT_MISCONFIG = 49,
        EXIT_REASON_INVEPT = 50,
        EXIT_REASON_RDTSCP = 51,
        EXIT_REASON_VMX_PREEMPTION_TIMER_EXPIRED = 52,
        EXIT_REASON_INVVPID = 53,
        EXIT_REASON_WBINVD = 54,
        EXIT_REASON_XSETBV = 55,
        EXIT_REASON_APIC_WRITE = 56,
        EXIT_REASON_RDRAND = 57,
        EXIT_REASON_INVPCID = 58,
        EXIT_REASON_RDSEED = 61,
        EXIT_REASON_PML_FULL = 62,
        EXIT_REASON_XSAVES = 63,
        EXIT_REASON_XRSTORS = 64,
        EXIT_REASON_PCOMMIT = 65
    };

}
