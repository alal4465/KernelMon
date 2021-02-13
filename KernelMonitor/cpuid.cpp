#pragma once
#include "cpuid.h"

bool cpuid::is_vendor_intel() {
	char str[13]{ 0 };
	arch::cpuid_vendor_name_t cpuid_info;

	__cpuid(reinterpret_cast<int*>(&cpuid_info), static_cast<int>(arch::CpuidCode::VENDOR_ID));
	strncat(str, cpuid_info.vendor_name_start, 4);
	strncat(str, cpuid_info.vendor_name_middle, 4);
	strncat(str, cpuid_info.vendor_name_end, 4);
	KdPrint(("[+] Vendor name is : %s\n", str));

	return strcmp(str, vendor_name) == 0;
}

bool cpuid::is_vmx_supported() {
	arch::cpuid_feature_info_t cpuid_info;

	__cpuid(reinterpret_cast<int*>(&cpuid_info), static_cast<int>(arch::CpuidCode::FEATURE_INFO));
	KdPrint(("[+] CPUID RESULT (pre-hypervisor) : VMX=%u, HYPERVISOR=%u\n", cpuid_info.ecx.features.vmx, cpuid_info.ecx.features.hypervisor));
	return cpuid_info.ecx.features.vmx;
}