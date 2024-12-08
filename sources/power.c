#include "../headers/types.h"

#include "../headers/io.h"

#include "../headers/acpi.h"

void shutdown(void) {
	// Shutdown for real pc

	ACPIInit(); // Init ACPI

	ACPIPowerOff(); // ACPI Power off

	// Shutdown for emulators

	out16(0xB004, 0x2000); // for old qemu or bochs

	out16(0x604, 0x2000); // for modern qemu

	out16(0x4004, 0x3400); // for virtualbox

	out16(0x600, 0x34); // for cloud hypervisor

	for (;;);
}

void reboot(void) {
	uint8 good = 0x02;

	while ((good & 0x02) != 0)
		in8(0x64, &good);

	out8(0x64, 0xFE);

	for (;;);
}