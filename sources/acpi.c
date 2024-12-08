#include "../headers/ACPI.h"

#include "../headers/types.h"

#include "../headers/io.h"

#include "../headers/error.h"

bool bACPIInitialized = false;

int32* SMI_CMD;

int8 ACPI_ENABLE;

int8 ACPI_DISABLE;

int32* PM1a_CNT;

int32* PM1b_CNT;

int16 SLP_TYPa;

int16 SLP_TYPb;

int16 SLP_EN;

int16 SCI_EN;

int8 PM1_CNT_LEN;

void ACPIInit(void) {
	if (bACPIInitialized) return;

	uint32* ptr = ACPIGetRSDPtr();

	if (ptr != 0 && ACPICheckHeader(ptr, "RSDT") == 0) {
		int entries = *(ptr + 1);

		entries = (entries - 36) / 4;

		ptr += 36 / 4;

		while (0 < entries--) {
			if (ACPICheckHeader((uint32* )* ptr, "FACP") == 0) {
				entries = -2;

				struct FACP* facp = (struct FACP*)*ptr;

				if (ACPICheckHeader((uint32*)facp->DSDT, "DSDT") == 0) {
					int8* S5Addr = (int8*) facp->DSDT + 36;

					int32 dsdtLength = *(facp->DSDT + 1) - 36;

					while (0 < dsdtLength--) {
						if (memcmp(S5Addr, "_S5_", 4) == 0)
							break;

						S5Addr++;
					}

					if (dsdtLength > 0) {
						if ((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\') ) && *(S5Addr + 4) == 0x12) {
							S5Addr += 5;

							S5Addr += ((*S5Addr & 0xC0) >> 6) + 2;

							if (*S5Addr == 0x0A)
								S5Addr++;

							SLP_TYPa = *(S5Addr) << 10;

							S5Addr++;

							if (*S5Addr == 0x0A)
								S5Addr++;

							SLP_TYPb = *(S5Addr) << 10;

							SMI_CMD = facp->SMI_CMD;

							ACPI_ENABLE = facp->ACPI_ENABLE;

							ACPI_DISABLE = facp->ACPI_DISABLE;

							PM1a_CNT = facp->PM1a_CNT_BLK;

							PM1b_CNT = facp->PM1b_CNT_BLK;
							
							PM1_CNT_LEN = facp->PM1_CNT_LEN;

							SLP_EN = 1 << 13;

							SCI_EN = 1;

							return 0;
						} 

						else {
							//wrstr("\\_S5 parse error.\n");
						}
					} 

					else {
						//wrstr("\\_S5 not present.\n");
					}
				} 

				else {
					//wrstr("DSDT invalid.\n");
				}
			}

			ptr++;
		}

		//wrstr("no valid FACP present.\n");
	} 

	else {
		UGSMASCIIputString("no ACPI.\n");
	}

	bACPIInitialized = true;
}

int32 ACPIEnable(void) {
	int16 temp = 0;

	in16((uint32)PM1a_CNT, &temp);

	if ( ( & SCI_EN) == 0 ) {
		if (SMI_CMD != 0 && ACPI_ENABLE != 0) {
			out8((uint32)SMI_CMD, ACPI_ENABLE);

			int32 i;

			for (i = 0; i < 300; i++) {
				in16((uint32)PM1a_CNT, &temp);

				if ((temp & SCI_EN) == 1)
					break;

				sleepMilliseconds(10);
			}

			if (PM1b_CNT != 0)
				for (; i < 300; i++) {
					in16((uint32)PM1b_CNT, &temp);

					if ((temp & SCI_EN) == 1)
						break;

					sleepMilliseconds(10);
				}

			if (i < 300) {
				return 0;
			} 

			else {
				return -1;
			}
		} 

		else {
			return -1;
		}
	} 

	else {
		return 0;
	}
}

uint32* ACPICheckRSDPtr(uint32* ptr) {
	int8* sig = "RSD PTR ";

	struct RSDPtr* rsdp = (struct RSDPtr* ) ptr;

	int8* bptr;

	int8 check = 0;

	if (memcmp(sig, rsdp, 8) == 0) {
		bptr = (int8*)ptr;

		for (int32 i = 0; i < sizeof(struct RSDPtr); i++) {
			check += *bptr;

			bptr++;
		}

		if (check == 0) {
			return (uint32*)rsdp->RSDTAddress;
		}
	}

	return 0;
}

uint32* ACPIGetRSDPtr(void) {
	uint32* addr;

	uint32* rsdp;

	for (addr = (uint32*)0x000E0000; (int32)addr < 0x00100000; addr += 0x10 / sizeof(addr)) {
		rsdp = ACPICheckRSDPtr(addr);

		if (rsdp != 0)
			return rsdp;
	}

	int32 ebda = *((int16*)0x40E);

	ebda = ebda * 0x10 & 0x000FFFFF;

	for (addr = (uint32*)ebda; (int32)addr < ebda + 1024; addr += 0x10 / sizeof(addr)) {
		rsdp = ACPICheckRSDPtr(addr);

		if (rsdp != 0)
			return rsdp;
	}

	return 0;
}

int32 ACPICheckHeader(uint32* ptr, int8* sig) {
	if (memcmp(ptr, sig, 4) == 0) {
		int8* checkPtr = (int8*) ptr;

		int32 len = *(ptr + 1);

		int8 check = 0;

		while (0 < len--) {
			check += *checkPtr;

			checkPtr++;
		}

		if (check == 0)
			return 0;
	}
	return -1;
}

void ACPIPowerOff(void) {
	if (SCI_EN == 0)
		return;

	ACPIEnable();

	out16((uint32)PM1a_CNT, SLP_TYPa | SLP_EN);

	if (PM1b_CNT != 0)
		out16((uint32)PM1b_CNT, SLP_TYPb | SLP_EN);

	//wrstr("ACPI poweroff failed.\n");
}