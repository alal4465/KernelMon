#include <ntddk.h>
#include "memory.h"
#include "cpuid.h"
#include "vmx.h"
#include "KernelMonitor.h"
KernelMonGlobals globals;

void DriverUnload(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS DriverCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DriverDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

extern "C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);
	KdPrint(("[+] Driver Entry\n"));

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDeviceControl;

	memset(globals.monitored_drivers, 0, sizeof(globals.monitored_drivers));

	globals.driver_log_buffer = new(NonPagedPool) kstd::CyclicBuffer<LogEntry, kstd::SpinLock>(MAX_PERRALLEL_BUF_ENTRIES, NonPagedPool);
	globals.driver_obj = DriverObject;

	UNICODE_STRING devName = RTL_CONSTANT_STRING(DEVICE_NAME);
	NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &globals.device_obj);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to create device (0x%08X)\n", status));
		return status;
	}

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(DEVICE_SYM_NAME);
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status)) {
		KdPrint(("Failed to create symbolic link (0x%08X)\n", status));
		IoDeleteDevice(globals.device_obj);
		return status;
	}
	
	if (!cpuid::is_vendor_intel()) {
		KdPrint(("[-] Vendor is not intel!\n"));
		return STATUS_SUCCESS;
	}

	if (!cpuid::is_vmx_supported()) {
		KdPrint(("[-] vmx is not supported!\n"));
		return STATUS_SUCCESS;
	}

	if (!vmx::initialize_vmx()) {
		KdPrint(("[-] Couldn't initialize vmx!\n"));
		return STATUS_SUCCESS;
	}

	return STATUS_SUCCESS;
}

NTSTATUS DriverCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DriverDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto status = STATUS_SUCCESS;
	ULONG_PTR information = 0;
	auto inputLen = stack->Parameters.DeviceIoControl.InputBufferLength;
	auto outputLen = stack->Parameters.DeviceIoControl.OutputBufferLength;

	switch (static_cast<KernelMonIoctls>(stack->Parameters.DeviceIoControl.IoControlCode)) {
	case KernelMonIoctls::GetData: {
		if (outputLen < sizeof(LogEntry)) {
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		auto result = static_cast<LogEntry*>(MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority));
		if (result == nullptr) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}
		result->function = MonitoredFunctions::None;

		if (globals.driver_log_buffer->pop(*result)) {
			KdPrint(("[+] Returning to UM %S %S\n", result->driver, result->details));
		}
		information = sizeof(*result);
		break;
	}
	case KernelMonIoctls::AddDriver: {
		information = 0;
		PCWSTR driverName = static_cast<PCWSTR>(Irp->AssociatedIrp.SystemBuffer);
		
		if (driverName[inputLen / sizeof(WCHAR) - 1] != L'\0') {
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		for (unsigned int i = 0; i < MAX_MONITORED_DRIVERS; i++) {
			if (globals.monitored_drivers[i]) {
				continue;
			}

			globals.monitored_drivers[i] = new(NonPagedPool) WCHAR[inputLen / sizeof(WCHAR)];
			wcscpy(globals.monitored_drivers[i], driverName);

			status = STATUS_SUCCESS;
			break;
		}

		break;
	}

	case KernelMonIoctls::RemoveDriver: {
		information = 0;
		PCWSTR driverName = static_cast<PCWSTR>(Irp->AssociatedIrp.SystemBuffer);

		if (driverName[inputLen / sizeof(WCHAR) - 1] != L'\0') {
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		for (auto& driver : globals.monitored_drivers) {
			if (driver != nullptr && wcscmp(driver, driverName) == 0) {
				delete driver;
				driver = nullptr;
			}
		}

		status = STATUS_SUCCESS;
		break;
	}

	case KernelMonIoctls::ResetState:
		information = 0;
		for (auto& driver : globals.monitored_drivers) {
			delete driver;
			driver = nullptr;
		}

		status = STATUS_SUCCESS;
		break;
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = information;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

void DriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	KdPrint(("[+] Driver Unload\n"));

	vmx::terminate_vmx();
}