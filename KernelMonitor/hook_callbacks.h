#pragma once
#include <ntddk.h>
#include "KernelMonitor.h"

namespace Hooking {
	NTSTATUS zw_create_file_callback(
		PHANDLE            FileHandle,
		ACCESS_MASK        DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		PIO_STATUS_BLOCK   IoStatusBlock,
		PLARGE_INTEGER     AllocationSize,
		ULONG              FileAttributes,
		ULONG              ShareAccess,
		ULONG              CreateDisposition,
		ULONG              CreateOptions,
		PVOID              EaBuffer,
		ULONG              EaLength
	);
	NTSTATUS zw_write_file_callback(
		HANDLE             FileHandle,
		HANDLE             Event,
		PIO_APC_ROUTINE    ApcRoutine,
		PVOID              ApcContext,
		PIO_STATUS_BLOCK   IoStatusBlock,
		PVOID              Buffer,
		ULONG              Length,
		PLARGE_INTEGER     ByteOffset,
		PULONG             Key
	);
}
