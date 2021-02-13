#include "memory.h"

void* operator new(size_t size, POOL_TYPE pool_type, ULONG tag) {
	return ExAllocatePoolWithTag(pool_type, size, tag);
}

void* operator new[](size_t size, POOL_TYPE pool_type, ULONG tag) {
	return ExAllocatePoolWithTag(pool_type, size, tag);
}

void operator delete(void* addr, size_t size)
{
	UNREFERENCED_PARAMETER(size);

	if (addr != nullptr)
		ExFreePool(addr);
}

void operator delete[](void* addr, size_t size)
{
	UNREFERENCED_PARAMETER(size);

	if (addr != nullptr)
		ExFreePool(addr);
}

void operator delete(void* addr) {
	if (addr != nullptr)
		ExFreePool(addr);
}

void operator delete[](void* addr) {
	if (addr != nullptr)
		ExFreePool(addr);
}