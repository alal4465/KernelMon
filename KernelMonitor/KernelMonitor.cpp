#include <ntddk.h>
#include "memory.h"
#include "cpuid.h"
#include "vmx.h"

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

#pragma warning( disable : 4065 )
NTSTATUS DriverDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto status = STATUS_SUCCESS;

	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

void DriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	KdPrint(("[+] Driver Unload\n"));

	vmx::terminate_vmx();
}