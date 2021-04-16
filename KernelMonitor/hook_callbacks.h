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
	NTSTATUS zw_create_key_callback(
		PHANDLE            KeyHandle,
		ACCESS_MASK        DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		ULONG              TitleIndex,
		PUNICODE_STRING    Class,
		ULONG              CreateOptions,
		PULONG             Disposition
	);
	NTSTATUS zw_set_value_key_callback(
		HANDLE          KeyHandle,
		PUNICODE_STRING ValueName,
		ULONG           TitleIndex,
		ULONG           Type,
		PVOID           Data,
		ULONG           DataSize
	);
	NTSTATUS zw_terminate_process_callback(
		HANDLE   ProcessHandle,
		NTSTATUS ExitStatus
	);
}
