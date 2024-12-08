#ifndef _GDT_H
#define _GDT_H

#include "types.h"

struct gdt_entry_struct {
	uint16 limit;

	uint16 base_low;

	uint8 base_middle;

	uint8 access;

	uint8 flags;

	uint8 base_high;
} PACKED;

struct gdt_ptr_struct {
	uint16 limit;

	uint32 base;
} PACKED;

void GDTInit(void);

void GDTSetGate(uint32 num, uint32 base, uint32 limit, uint8 access, uint8 gran);

#endif