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
		wcscpy_s(log.details, ObjectAttributes->ObjectName->Buffer);
		log.details[min(ObjectAttributes->ObjectName->Length, ((sizeof(log.details) / sizeof(log.details[0])) - 1))] = L'\0';

		log.function = MonitoredFunctions::ZwCreateFile;
		log.result = result;
		
		KdPrint(("[+] Pushing log to cyclic buffer: %S %S\n", log.driver, log.details));
		globals.driver_log_buffer->push(log);
	}
	
	return result;
}