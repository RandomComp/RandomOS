#ifndef _BIOS_H
#define _BIOS_H

#include "types.h"

#define BIOS_SIZE 0x100000

#define BIOS_HIGH_BASE 0xC0000

#define BIOS_HIGH_SIZE (0x100000 - 0xC0000)

#define BIOS_BDA_BASE 0x9fc00

#define BIOS_BDA_SIZE 0x400

#define VBE_BIOS_INFO_OFFSET 0x70000

#define VBE_BIOS_MODE_INFO_OFFSET 0x80000

typedef struct BIOS_REGS {
	uint16 CS;
	
	uint16 DS;
	
	uint16 ES;
	
	uint16 FS;
	
	uint16 GS;
	
	uint16 SS;
	
	uint32 EFLAGS;
	
	uint32 EAX;
	
	uint32 EBX;
	
	uint32 ECX;
	
	uint32 EDX;
	
	uint32 ESP;
	
	uint32 EBP;
	
	uint32 ESI;
	
	uint32 EDI;
	
	uint32 EIP;
} BIOS_REGS;

void BIOSInit(void);

void BIOSInterrupt(BIOS_REGS* p_regs, uint8 num);

#endif