#include "../headers/std.h"

#include "../headers/io.h"

#include "../headers/pci.h"

#include "../headers/pci_driver.h"

bool bPCIInitialized = false;

static void PCIVisit(uint32 bus, uint32 dev, uint32 func) {
	uint32 id = PCI_MAKE_ID(bus, dev, func);

	PCIDeviceInfo info;

	PCIIn16(id, PCI_CONFIG_VENDOR_ID, &info.vendorId);

	if (info.vendorId == 0xffff) return;

	PCIIn16(id, PCI_CONFIG_DEVICE_ID, &info.deviceId);

	PCIIn8(id, PCI_CONFIG_PROG_INTF, &info.progIntf);

	PCIIn8(id, PCI_CONFIG_SUBCLASS, &info.subclass);

	PCIIn8(id, PCI_CONFIG_CLASS_CODE, &info.classCode);

	/*ConsolePrint("%02x:%02x:%d 0x%04x/0x%04x: %s\n",
		bus, dev, func,
		info.vendorId, info.deviceId,
		PCIClassName(info.classCode, info.subclass, info.progIntf)
		);*/

	const PCIDriver* driver = g_PCIDriverTable;

	while (driver->init) {
		driver->init(id, &info);

		++driver;
	}
}

void PCIInit(void) {
	for (uint32 bus = 0; bus < 256; ++bus) {
		for (uint32 dev = 0; dev < 32; ++dev) {
			uint32 baseId = PCI_MAKE_ID(bus, dev, 0);

			uint8 headerType = 0;

			PCIIn8(baseId, PCI_CONFIG_HEADER_TYPE, &headerType);
			
			uint32 funcCount = headerType & PCI_TYPE_MULTIFUNC ? 8 : 1;

			for (uint32 func = 0; func < funcCount; ++func)
				PCIVisit(bus, dev, func);
		}
	}
}