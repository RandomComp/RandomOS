#ifndef _IO_H
#define _IO_H

#include "types.h"

void in8(uint16 port, uint8* data);

void in16(uint16 port, uint16* data);

void in32(uint16 port, uint32* data);

void ins8(uint16 port, uint8* buf, uint32 count);

void ins16(uint16 port, uint16* buf, uint32 count);

void ins32(uint16 port, uint32* buf, uint32 count);

void insn(uint16 port, void* buf, int32 unit_size, uint32 total_size);

void out8(uint16 port, uint8 data);

void out16(uint16 port, uint16 data);

void out32(uint16 port, uint32 data);

void outs8(uint16 port, uint8* buf, uint32 count);

void outs16(uint16 port, uint16* buf, uint32 count);

void outs32(uint16 port, uint32* buf, uint32 count);

void outsn(uint16 port, void* buf, int32 unit_size, uint32 total_size);

#endif