#include "../headers/pci_driver.h"

#include "../headers/io.h"

const PCIDriver g_PCIDriverTable[] = {
	{ 0 },
};

void PCIIn8(uint32 id, uint32 reg, uint8* result) {
	uint32 addr = 0x80000000 | id | (reg & 0xfc);

	out32(PCI_CONFIG_ADDR, addr);

	in8(PCI_CONFIG_DATA + (reg & 0x03), result);
}

void PCIIn16(uint32 id, uint32 reg, uint16* result) {
	uint32 addr = 0x80000000 | id | (reg & 0xfc);

	out32(PCI_CONFIG_ADDR, addr);

	in16(PCI_CONFIG_DATA + (reg & 0x02), result);
}

void PCIIn32(uint32 id, uint32 reg, uint32* result) {
	uint32 addr = 0x80000000 | id | (reg & 0xfc);

	out32(PCI_CONFIG_ADDR, addr);

	in32(PCI_CONFIG_DATA, result);
}

void PCIOut8(uint32 id, uint32 reg, uint8 data) {
	uint32 address = 0x80000000 | id | (reg & 0xfc);

	out32(PCI_CONFIG_ADDR, address);

	out8(PCI_CONFIG_DATA + (reg & 0x03), data);
}

void PCIOut16(uint32 id, uint32 reg, uint16 data) {
	uint32 address = 0x80000000 | id | (reg & 0xfc);

	out32(PCI_CONFIG_ADDR, address);

	out16(PCI_CONFIG_DATA + (reg & 0x02), data);
}


void PCIOut32(uint32 id, uint32 reg, uint32 data) {
	uint32 address = 0x80000000 | id | (reg & 0xfc);

	out32(PCI_CONFIG_ADDR, address);

	out32(PCI_CONFIG_DATA, data);
}

static void PCIReadBar(uint32 id, uint32 index, uint32* address, uint32* mask) {
	uint32 reg = PCI_CONFIG_BAR0 + index * sizeof(uint32);
	
	PCIIn32(id, reg, address);
	
	PCIOut32(id, reg, 0xffffffff);

	PCIIn32(id, reg, mask);
	
	PCIOut32(id, reg, *address);
}

void PCIGetBar(PCIBar* bar, uint32 id, uint32 index) {
	uint32 addressLow;

	uint32 maskLow;

	PCIReadBar(id, index, &addressLow, &maskLow);

	if (addressLow & PCI_BAR_64) {
		uint32 addressHigh;

		uint32 maskHigh;

		PCIReadBar(id, index + 1, &addressHigh, &maskHigh);

		bar->u.address = (void*)(((uint64)addressHigh << 32) | (addressLow & ~0xf));
		
		bar->size = ~(((uint64)maskHigh << 32) | (maskLow & ~0xf)) + 1;
		
		bar->flags = addressLow & 0xf;
	}

	else if (addressLow & PCI_BAR_IO) {
		bar->u.port = (uint16)(addressLow & ~0x3);

		bar->size = (uint16)(~(maskLow & ~0x3) + 1);

		bar->flags = addressLow & 0x3;
	}

	else {
		bar->u.address = (void*)((uint64)(addressLow & ~0xf));

		bar->size = ~(maskLow & ~0xf) + 1;

		bar->flags = addressLow & 0xf;
	}
}