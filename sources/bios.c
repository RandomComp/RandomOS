#include "../headers/types.h"

#include "../headers/bios.h"

#include "../headers/x86emu.h"

#include "../headers/mem.h"

struct x86emu emulator;

bool bBIOSInitialized = false;

void BIOSInit(void) {
    memset(&emulator, 0, sizeof(emulator));

    x86emu_init_default(&emulator);

    emulator.mem_base = (int8*)0;

    emulator.mem_size = BIOS_SIZE;

    bBIOSInitialized = true;
}

void BIOSInterrupt(BIOS_REGS* p_regs, uint8 num) {
    memcpy(&(emulator.x86), p_regs, sizeof(BIOS_REGS));

    x86emu_exec_intr(&emulator, num);

    memcpy(p_regs, &(emulator.x86), sizeof(BIOS_REGS));
}