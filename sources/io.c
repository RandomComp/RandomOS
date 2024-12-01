#include "../headers/types.h"

void in8(uint16 port, uint8* data) {
	asm volatile ("inb %%dx, %%al" : "=a" (*data) : "d" (port));
}

void in16(uint16 port, uint16* data) { 
	asm volatile ("inw %%dx, %%ax" : "=a" (*data) : "d" (port));
}

void in32(uint16 port, uint32* data) { 
	asm volatile ("inl %%dx, %%eax" : "=a" (*data) : "d" (port));
}

void ins8(uint16 port, uint8* buf, uint32 count) { 
	asm volatile ("cld; rep insb" : "=c" (count), "=D" (buf)
				  : "c" (count), "d" (port), "D" (buf));
}

void ins16(uint16 port, uint16* buf, uint32 count) { 
	asm volatile ("cld; rep insw" : "=c" (count), "=D" (buf)
				  : "c" (count), "d" (port), "D" (buf));
}

void ins32(uint16 port, uint32* buf, uint32 count) { 
	asm volatile ("cld; rep insl" : "=c" (count), "=D" (buf)
				  : "c" (count), "d" (port), "D" (buf));
}

void insn(uint16 port, void* buf, int32 unit_size, uint32 total_size) {
	if (unit_size == 4)
		ins32(port, buf, total_size / 4);

	else if (unit_size == 2)
		ins16(port, buf, total_size / 2);

	else if (unit_size == 1)
		ins8(port, buf, total_size);
}

void out8(uint16 port, uint8 data) { 
	asm volatile ("outb %%al, %%dx" : : "a" (data), "d" (port));
}

void out16(uint16 port, uint16 data) { 
	asm volatile ("outw %%ax, %%dx" : : "a" (data), "d" (port));
}

void out32(uint16 port, uint32 data) { 
	asm volatile ("outl %%eax, %%dx" : : "a" (data), "d" (port));
}

void outs8(uint16 port, uint8* buf, uint32 count) { 
	asm volatile ("cld; rep outsb" : "=c" (count), "=S" (buf)
				  : "c" (count), "d" (port), "S" (buf));
}

void outs16(uint16 port, uint16* buf, uint32 count) { 
	asm volatile ("cld; rep outsw" : "=c" (count), "=S" (buf)
				  : "c" (count), "d" (port), "S" (buf));
}

void outs32(uint16 port, uint32* buf, uint32 count) { 
	asm volatile ("cld; rep outsl" : "=c" (count), "=S" (buf)
				  : "c" (count), "d" (port), "S" (buf));
}

void outsn(uint16 port, void* buf, int32 unit_size, uint32 total_size) {
	if (unit_size == 4)
		outs32(port, buf, total_size / 4);

	else if (unit_size == 2)
		outs16(port, buf, total_size / 2);

	else if (unit_size == 1)
		outs8(port, buf, total_size);
}