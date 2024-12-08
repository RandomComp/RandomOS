#include "../headers/mem.h"

#include "../headers/multiboot.h"

#include "../headers/types.h"

#include "../headers/std.h"

#include "../headers/error.h"

extern struct multibootInfo* bootInfo;

extern uint32 InitialPageDir[1024];

struct Size RAMSize;

struct Size heapSize;

AbsoluteSize AbsoluteRAMSizeInBytes = 0;

Address heapStart = 0;

AbsoluteSize AbsoluteHeapSizeInBytes = 0;

Address freeSpaceStart = 0;

struct Size heapSize;

bool bMEMInitialized = false;

void MEMInit(AbsoluteSize size) {
	if (bMEMInitialized) return;

	for (uint32 i = 0; i < bootInfo->mmap_length; i += sizeof(struct multibootMMapEntry)) {
		struct multibootMMapEntry* mapEntry = (struct multibootMMapEntry*)(bootInfo->mmap_addr + i);
		
		if (mapEntry->type != 1) continue;

		AbsoluteRAMSizeInBytes += mapEntry->len_low;

		AbsoluteRAMSizeInBytes += (uint64)(mapEntry->len_high) << 32;
	}

	AbsoluteHeapSizeInBytes = size;

	if (AbsoluteHeapSizeInBytes * 2 >= AbsoluteRAMSizeInBytes) {
		cause(MemoryLackError);
	}

	freeSpaceStart = heapStart;

	RAMSize.bytes = AbsoluteRAMSizeInBytes % 1024;

	RAMSize.kilobytes = (AbsoluteRAMSizeInBytes / 1024) % 1024;

	RAMSize.megabytes = (AbsoluteRAMSizeInBytes / 1024 / 1024) % 1024;

	RAMSize.gigabytes = (AbsoluteRAMSizeInBytes / 1024 / 1024 / 1024) % 1024;

	heapSize.bytes = AbsoluteHeapSizeInBytes % 1024;

	heapSize.kilobytes = (AbsoluteHeapSizeInBytes / 1024) % 1024;

	heapSize.megabytes = (AbsoluteHeapSizeInBytes / 1024 / 1024) % 1024;

	heapSize.gigabytes = (AbsoluteHeapSizeInBytes / 1024 / 1024 / 1024) % 1024;
}

void* malloc(AbsoluteSize size, enum MemoryRegionStatus status) {
	Address addr = freeSpaceStart;

	if ((addr + size + sizeof(struct MemoryRegionInformation)) >= AbsoluteRAMSizeInBytes) {

	}

	struct MemoryRegionInformation* memoryRegionInformation = (struct MemoryRegionInformation*)(addr);

	memoryRegionInformation->size = size;

	memoryRegionInformation->status = status;

	addr += sizeof(struct MemoryRegionInformation);

	freeSpaceStart = addr + size;

	return (void*)(addr);
}

void free(void* argMem, AbsoluteSize size) {
	size += sizeof(struct MemoryRegionInformation);

	Address addr = &(*argMem) - sizeof(struct MemoryRegionInformation);

	uint8* mem = (uint8*)(addr);

	for (Address i = 0; i < size; i++) {
		mem[i] = 0;
	}
}