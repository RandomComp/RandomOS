#ifndef _ACPI_H
#define _ACPI_H

#include "../headers/types.h"

struct RSDPtr {
	int8 Signature[8];

	int8 CheckSum;

	int8 OemID[6];

	int8 Revision;

	int32* RSDTAddress;
};

struct FACP {
	int8 Signature[4];

	int32 Length;

	int8 unneded1[40 - 8];

	int32* DSDT;

	int8 unneded2[48 - 44];

	int32* SMI_CMD;

	int8 ACPI_ENABLE;

	int8 ACPI_DISABLE;

	int8 unneded3[64 - 54];

	int32* PM1a_CNT_BLK;

	int32* PM1b_CNT_BLK;

	int8 unneded4[89 - 72];

	int8 PM1_CNT_LEN;
};

void ACPIInit(void);

int32 ACPIEnable(void);

uint32* ACPICheckRSDPtr(uint32* ptr);

uint32* ACPIGetRSDPtr(void);

int32 ACPICheckHeader(uint32* ptr, int8* sig);

void ACPIPowerOff(void);

#endif