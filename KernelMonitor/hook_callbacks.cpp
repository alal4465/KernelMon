#include "hook_callbacks.h"

typedef struct _KLDR_DATA_TABLE_ENTRY
{
	struct _LIST_ENTRY InLoadOrderLinks;                  
	VOID* ExceptionTable;                                 
	ULONG ExceptionTableSize;                             
	VOID* GpValue;                                        
	struct _NON_PAGED_DEBUG_INFO* NonPagedDebugInfo;      
	VOID* DllBase;                                        
	VOID* EntryPoint;                                     
	ULONG SizeOfImage;                                    
	struct _UNICODE_STRING FullDllName;                   
	struct _UNICODE_STRING BaseDllName;                   
	ULONG Flags;                                          
	USHORT LoadCount;                                     
	union
	{
		USHORT SignatureLevel : 4;                        
		USHORT SignatureType : 3;                         
		USHORT Unused : 9;                                
		USHORT EntireField;                               
	} u1;                                                 
	VOID* SectionPointer;                                 
	ULONG CheckSum;                                       
	ULONG CoverageSectionSize;                            
	VOID* CoverageSection;                                
	VOID* LoadedImports;                                  
	VOID* Spare;                                          
	ULONG SizeOfImageNotRounded;                          
	ULONG TimeDateStamp;                                  
}KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;

static bool find_driver_by_address(void* addr_in_range, PUNICODE_STRING& driver_name) {
	auto PsLoadedModuleList = &reinterpret_cast<PKLDR_DATA_TABLE_ENTRY>(globals.driver_obj->DriverSection)->InLoadOrderLinks;

	for (PLIST_ENTRY pListEntry = PsLoadedModuleList->Flink; pListEntry != PsLoadedModuleList; pListEntry = pListEntry->Flink)
	{
		PKLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (addr_in_range >= pEntry->DllBase && addr_in_range < (reinterpret_cast<unsigned char*>(pEntry->DllBase) + pEntry->SizeOfImage)) {
			driver_name = &pEntry->BaseDllName;
			return true;
		}
	}

	return false;
}

static bool is_driver_monitored(PUNICODE_STRING driver_name) {

	for (auto monitored_driver : globals.monitored_drivers) {
		if (!monitored_driver)
			continue;

		if (!wcscmp(monitored_driver, driver_name->Buffer))
			return true;
	}

	return false;
}

NTSTATUS Hooking::zw_create_file_callback(
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
) {
	auto jump_stub = globals.hook_info_manager->lookup_jump_stub(ZwCreateFile);
	auto result = jump_stub(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength
	);

	PUNICODE_STRING driver_name;
	if (find_driver_by_address(_ReturnAddress(), driver_name) && is_driver_monitored(driver_name)) {
		LogEntry log;

		wcscpy_s(log.driver, driver_name->Buffer);
		log.driver[min(driver_name->Length, (sizeof(log.driver)/sizeof(log.driver[0])) - 1)] = L'\0';
		wcscpy_s(log.path, ObjectAttributes->ObjectName->Buffer);
		log.path[min(ObjectAttributes->ObjectName->Length, ((sizeof(log.path) / sizeof(log.path[0])) - 1))] = L'\0';
		log.details[0] = L'\0';
		log.function = MonitoredFunctions::ZwCreateFile;
		log.result = result;
		
		KdPrint(("[+] ZwCreateFile Pushing log to cyclic buffer: %S %S\n", log.driver, log.path));
		globals.driver_log_buffer->push(log);
	}
	
	return result;
}

NTSTATUS Hooking::zw_write_file_callback(
	HANDLE             FileHandle,
	HANDLE             Event,
	PIO_APC_ROUTINE    ApcRoutine,
	PVOID              ApcContext,
	PIO_STATUS_BLOCK   IoStatusBlock,
	PVOID              Buffer,
	ULONG              Length,
	PLARGE_INTEGER     ByteOffset,
	PULONG             Key
) {
	auto jump_stub = globals.hook_info_manager->lookup_jump_stub(ZwWriteFile);
	auto result = jump_stub(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		ByteOffset,
		Key
	);

	PUNICODE_STRING driver_name;
	if (find_driver_by_address(_ReturnAddress(), driver_name) && is_driver_monitored(driver_name)) {
		LogEntry log;

		wcscpy_s(log.driver, driver_name->Buffer);
		log.driver[min(driver_name->Length, (sizeof(log.driver) / sizeof(log.driver[0])) - 1)] = L'\0';
		log.path[0] = L'\0';
		memcpy(log.details, Buffer, min(Length, sizeof(log.details)));
		log.function = MonitoredFunctions::ZwWriteFile;
		log.result = result;

		KdPrint(("[+] ZwWriteFile Pushing log to cyclic buffer: %S\n", log.driver));
		globals.driver_log_buffer->push(log);
	}

	return result;
}

NTSTATUS Hooking::zw_create_key_callback(
	PHANDLE            KeyHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	ULONG              TitleIndex,
	PUNICODE_STRING    Class,
	ULONG              CreateOptions,
	PULONG             Disposition
) {
	auto jump_stub = globals.hook_info_manager->lookup_jump_stub(ZwCreateKey);
	auto result = jump_stub(
		KeyHandle,
		DesiredAccess,
		ObjectAttributes,
		TitleIndex,
		Class,
		CreateOptions,
		Disposition
	);

	PUNICODE_STRING driver_name;
	if (find_driver_by_address(_ReturnAddress(), driver_name) && is_driver_monitored(driver_name)) {
		LogEntry log;

		wcscpy_s(log.driver, driver_name->Buffer);
		log.driver[min(driver_name->Length, (sizeof(log.driver) / sizeof(log.driver[0])) - 1)] = L'\0';
		wcscpy_s(log.path, ObjectAttributes->ObjectName->Buffer);
		log.path[min(ObjectAttributes->ObjectName->Length, ((sizeof(log.path) / sizeof(log.path[0])) - 1))] = L'\0';
		log.details[0] = L'\0';
		log.function = MonitoredFunctions::ZwCreateKey;
		log.result = result;

		KdPrint(("[+] ZwCreateKey Pushing log to cyclic buffer: %S %S\n", log.driver, log.path));
		globals.driver_log_buffer->push(log);
	}

	return result;
}

NTSTATUS Hooking::zw_set_value_key_callback(
	HANDLE          KeyHandle,
	PUNICODE_STRING ValueName,
	ULONG           TitleIndex,
	ULONG           Type,
	PVOID           Data,
	ULONG           DataSize
) {
	auto jump_stub = globals.hook_info_manager->lookup_jump_stub(ZwSetValueKey);
	auto result = jump_stub(
		KeyHandle,
		ValueName,
		TitleIndex,
		Type,
		Data,
		DataSize
	);

	PUNICODE_STRING driver_name;
	if (find_driver_by_address(_ReturnAddress(), driver_name) && is_driver_monitored(driver_name)) {
		LogEntry log;

		wcscpy_s(log.driver, driver_name->Buffer);
		log.driver[min(driver_name->Length, (sizeof(log.driver) / sizeof(log.driver[0])) - 1)] = L'\0';
		wcscpy_s(log.path, ValueName->Buffer);
		log.path[min(ValueName->Length, ((sizeof(log.path) / sizeof(log.path[0])) - 1))] = L'\0';
		log.details[0] = L'\0';
		log.function = MonitoredFunctions::ZwSetValueKey;
		log.result = result;

		KdPrint(("[+] ZwSetValueKey Pushing log to cyclic buffer: %S %S\n", log.driver, log.path));
		globals.driver_log_buffer->push(log);
	}

	return result;
}

NTSTATUS Hooking::zw_terminate_process_callback(
	HANDLE   ProcessHandle,
	NTSTATUS ExitStatus
) {
	auto jump_stub = globals.hook_info_manager->lookup_jump_stub(ZwTerminateProcess);
	auto result = jump_stub(
		ProcessHandle,
		ExitStatus
	);

	PUNICODE_STRING driver_name;
	if (find_driver_by_address(_ReturnAddress(), driver_name) && is_driver_monitored(driver_name)) {
		LogEntry log;

		wcscpy_s(log.driver, driver_name->Buffer);
		log.driver[min(driver_name->Length, (sizeof(log.driver) / sizeof(log.driver[0])) - 1)] = L'\0';
		log.details[0] = L'\0';
		log.path[0] = L'\0';
		log.function = MonitoredFunctions::ZwTerminateProcess;
		log.result = result;

		KdPrint(("[+] ZwTerminateProcess Pushing log to cyclic buffer\n"));
		globals.driver_log_buffer->push(log);
	}

	return result;
}