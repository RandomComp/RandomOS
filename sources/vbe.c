#include "../headers/vbe.h"

#include "../headers/std.h"

#include "../headers/bios.h"

#include "../headers/mem.h"

#include "../headers/error.h"

uint32 vidmemaddr = 0;

uint32 vidmode = 0;

uint8 vidchannels = 0;

VbeInfoBlock* VBEGetGeneralInfo(void) {
	BIOSInit();

	BIOS_REGS regs;

	memset(&regs, 0, sizeof(BIOS_REGS));

	regs.ECX = 0;

	regs.EAX = 0x4f00;

	regs.ES = VBE_BIOS_INFO_OFFSET >> 4;

	regs.EDI = 0;

	BIOSInterrupt(&regs, 0x10);

	if (regs.EAX != 0x4f)
		return 0;

	return (VbeInfoBlock*)VBE_BIOS_INFO_OFFSET;
}

ModeInfoBlock* VBEGetModeInfo(uint32 mode) {
	BIOSInit();

	BIOS_REGS regs;

	memset(&regs, 0, sizeof(BIOS_REGS));

	regs.ECX = mode;

	regs.EAX = 0x4f01;

	regs.ES = VBE_BIOS_MODE_INFO_OFFSET >> 4;

	regs.EDI = 0;

	BIOSInterrupt(&regs, 0x10);

	if (regs.EAX != 0x4f)
		return 0;

	return (ModeInfoBlock*)VBE_BIOS_MODE_INFO_OFFSET;
}

bool VBESetMode(uint32 mode) {
	BIOSInit();

	BIOS_REGS regs;

	memset(&regs, 0, sizeof(BIOS_REGS));

	if (mode >= 0x100) {
		regs.EBX = mode;

		regs.EAX = 0x4f02;
	}
	
	else regs.EAX = mode;

	BIOSInterrupt(&regs, 0x10);

	return regs.EAX == 0x4f;
}

bool VBESetup(uint16 w, uint16 h) {
	BIOSInit();

	memset((uint8*)VBE_BIOS_INFO_OFFSET, 0, sizeof(VbeInfoBlock));
	
	memset((uint8*)VBE_BIOS_MODE_INFO_OFFSET, 0, sizeof(ModeInfoBlock));

	VbeInfoBlock* p_info = VBEGetGeneralInfo();

	bool vbeSupport = p_info != 0;

	if (!vbeSupport) cause(VBENotSupportedError);
	
	vbeSupport = p_info->VbeVersion >= 0x200;

	vbeSupport = vbeSupport && (p_info->VbeSignature.SigChr[0] == 'V');
	
	vbeSupport = vbeSupport && (p_info->VbeSignature.SigChr[1] == 'E');
	
	vbeSupport = vbeSupport && (p_info->VbeSignature.SigChr[2] == 'S');
	
	vbeSupport = vbeSupport && (p_info->VbeSignature.SigChr[3] == 'A');

	if (!vbeSupport) cause(VBENotSupportedError);

	bool found = false;

	for (uint32 m = 0; m < 0x200; m++) {
		ModeInfoBlock* p_m_info = VBEGetModeInfo(m);

		if (p_m_info) {
			if (p_m_info->PhysBasePtr != 0 
				&& p_m_info->XResolution == w 
				&& p_m_info->YResolution == h
				&& p_m_info->XResolution <= 1920
				&& p_m_info->YResolution <= 1080
				&& p_m_info->BitsPerPixel == 32) {
				
				found = true;

				vidmode = m;

				vidmemaddr = p_m_info->PhysBasePtr;

				vidchannels = p_m_info->BitsPerPixel / 8;

				/*putString("Screen information:\n\n");

				putString("\tWidth and height: ");

				putUX16Integer(p_m_info->XResolution);

				putString(", ");

				putUX16Integer(p_m_info->YResolution);

				putString("\n\n");

				putString("\tCount channels: ");

				putUX8Integer(p_m_info->BitsPerPixel);

				putString("\n\n");

				putString("\tVideo memory address at: ");

				putUX32Integer(p_m_info->PhysBasePtr);

				putString("\n\n");*/
			}
		}
	}

	return found;
}