#pragma once
#include <ntddk.h>

void* operator new(size_t size, POOL_TYPE pool_type = PagedPool, ULONG tag = 0);

void* operator new[](size_t size, POOL_TYPE pool_type = PagedPool, ULONG tag = 0);

void operator delete(void* addr, size_t size);

void operator delete[](void* addr, size_t size);

void operator delete(void* addr);

void operator delete[](void* addr);