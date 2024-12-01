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

static uint32 pageFrameMin;

static uint32 pageFrameMax;

static uint32 totalAlloc;

#define NUM_PAGES_DIRS 256

#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)

uint8 physicalMemoryBitmap[NUM_PAGE_FRAMES / 8];

static uint32 pageDirs[NUM_PAGES_DIRS][1024] ALIGNED4096;

static uint8 pageDirUsed[NUM_PAGES_DIRS];

void PMMInit(uint32 memLow, uint32 memHigh) {
	pageFrameMin = ((memLow + 0x1000) - 1) / 0x1000;

	pageFrameMax = memHigh / 0x1000;

	totalAlloc = 0;

	memset(physicalMemoryBitmap, 0, sizeof(physicalMemoryBitmap));
}

void MEMInit(AbsoluteSize size) {
	if (bMEMInitialized) return;

	for (uint32 i = 0; i < bootInfo->mmap_length; i += sizeof(struct multibootMMapEntry)) {
		struct multibootMMapEntry* mapEntry = (struct multibootMMapEntry*)(bootInfo->mmap_addr + i);
		
		if (mapEntry->type != 1) continue;

		AbsoluteRAMSizeInBytes += mapEntry->len_low;

		AbsoluteRAMSizeInBytes += (uint64)(mapEntry->len_high) << 32;
	}

	AbsoluteHeapSizeInBytes = size;

	if (AbsoluteHeapSizeInBytes > AbsoluteRAMSizeInBytes) {
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

void invalidate(uint32 vaddr) {
	asm volatile("invlpg %0" :: "m"(vaddr));
}