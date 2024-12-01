#include "../headers/idt.h"

#include "../headers/gdt.h"

#include "../headers/types.h"

#include "../headers/std.h"

#include "../headers/fatal_error.h"

#include "../headers/mem.h"

#include "../headers/io.h"

extern void IDTFlush(uint32);

struct IDTEntryStruct idtEntries[256];

struct IDTPtrStruct idtPtr;

bool bIDTInitialized = false;

void IDTInit(void) {
	if (bIDTInitialized) return;

	GDTInit();

	idtPtr.limit = sizeof(struct IDTEntryStruct) * 256 - 1;

	idtPtr.base = (uint32)&idtEntries;

	memset(&idtEntries, 0, sizeof(struct IDTEntryStruct) * 256);

	out8(0x20, 0x11);

	out8(0xa0, 0x11);

	out8(0x21, 0x20);

	out8(0xa1, 0x28);

	out8(0x21, 0x04);

	out8(0xa1, 0x02);

	out8(0x21, 0x01);

	out8(0xa1, 0x01);

	out8(0x21, 0x00);

	out8(0xa1, 0x00);

	IDTSetGate(0, (uint32)isr0, 0x08, 0x8e);

	IDTSetGate(1, (uint32)isr1, 0x08, 0x8e);

	IDTSetGate(2, (uint32)isr2, 0x08, 0x8e);

	IDTSetGate(3, (uint32)isr3, 0x08, 0x8e);

	IDTSetGate(4, (uint32)isr4, 0x08, 0x8e);

	IDTSetGate(5, (uint32)isr5, 0x08, 0x8e);

	IDTSetGate(6, (uint32)isr6, 0x08, 0x8e);

	IDTSetGate(7, (uint32)isr7, 0x08, 0x8e);

	IDTSetGate(8, (uint32)isr8, 0x08, 0x8e);

	IDTSetGate(9, (uint32)isr9, 0x08, 0x8e);

	IDTSetGate(10, (uint32)isr10, 0x08, 0x8e);

	IDTSetGate(11, (uint32)isr11, 0x08, 0x8e);

	IDTSetGate(12, (uint32)isr12, 0x08, 0x8e);

	IDTSetGate(13, (uint32)isr13, 0x08, 0x8e);

	IDTSetGate(14, (uint32)isr14, 0x08, 0x8e);

	IDTSetGate(15, (uint32)isr15, 0x08, 0x8e);

	IDTSetGate(16, (uint32)isr16, 0x08, 0x8e);

	IDTSetGate(17, (uint32)isr17, 0x08, 0x8e);

	IDTSetGate(18, (uint32)isr18, 0x08, 0x8e);

	IDTSetGate(19, (uint32)isr19, 0x08, 0x8e);

	IDTSetGate(20, (uint32)isr20, 0x08, 0x8e);

	IDTSetGate(21, (uint32)isr21, 0x08, 0x8e);

	IDTSetGate(22, (uint32)isr22, 0x08, 0x8e);

	IDTSetGate(23, (uint32)isr23, 0x08, 0x8e);

	IDTSetGate(24, (uint32)isr24, 0x08, 0x8e);

	IDTSetGate(25, (uint32)isr25, 0x08, 0x8e);

	IDTSetGate(26, (uint32)isr26, 0x08, 0x8e);

	IDTSetGate(27, (uint32)isr27, 0x08, 0x8e);

	IDTSetGate(28, (uint32)isr28, 0x08, 0x8e);

	IDTSetGate(29, (uint32)isr29, 0x08, 0x8e);

	IDTSetGate(30, (uint32)isr30, 0x08, 0x8e);

	IDTSetGate(31, (uint32)isr31, 0x08, 0x8e);


	IDTSetGate(32, (uint32)irq0, 0x08, 0x8e);

	IDTSetGate(33, (uint32)irq1, 0x08, 0x8e);

	IDTSetGate(34, (uint32)irq2, 0x08, 0x8e);

	IDTSetGate(35, (uint32)irq3, 0x08, 0x8e);

	IDTSetGate(36, (uint32)irq4, 0x08, 0x8e);

	IDTSetGate(37, (uint32)irq5, 0x08, 0x8e);

	IDTSetGate(38, (uint32)irq6, 0x08, 0x8e);

	IDTSetGate(39, (uint32)irq7, 0x08, 0x8e);

	IDTSetGate(40, (uint32)irq8, 0x08, 0x8e);

	IDTSetGate(41, (uint32)irq9, 0x08, 0x8e);

	IDTSetGate(42, (uint32)irq10, 0x08, 0x8e);

	IDTSetGate(43, (uint32)irq11, 0x08, 0x8e);

	IDTSetGate(44, (uint32)irq12, 0x08, 0x8e);

	IDTSetGate(45, (uint32)irq13, 0x08, 0x8e);

	IDTSetGate(46, (uint32)irq14, 0x08, 0x8e);

	IDTSetGate(47, (uint32)irq15, 0x08, 0x8e);

	IDTSetGate(128, (uint32)isr128, 0x08, 0x8e);

	IDTSetGate(177, (uint32)isr177, 0x08, 0x8e);

	IDTFlush((uint32)&idtPtr);

	bIDTInitialized = true;
}

void IDTSetGate(uint8 num, uint32 base, uint16 sel, uint8 flags) {
	idtEntries[num].base_low = base & 0xffff;

	idtEntries[num].base_high = (base >> 16) & 0xffff;

	idtEntries[num].sel = sel;

	idtEntries[num].always0 = 0;

	idtEntries[num].flags = flags | 0x60;
}

void IDTISRHandler(struct Registers* regs) {
	if (regs->int_no < 20)
		causeFatal(regs->int_no + 1);

	else if (regs->int_no < 32) 
		causeFatal(ReservedError);
}

void* IDTIRQRoutines[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
};

void IDTIRQInstallHandler(int32 irq, void (*handler)(struct Registers* regs)) {
	IDTIRQRoutines[irq] = handler;
}

void IDTIRQUninstallHandler(int32 irq) {
	IDTIRQRoutines[irq] = 0;
}

void IDTIRQHandler(struct Registers* regs) {
	void (*handler)(struct Registers* regs);

	handler = IDTIRQRoutines[regs->int_no - 32];

	if (handler)
		handler(regs);

	if (regs->int_no >= 40)
		out8(0xa0, 0x20);
	
	out8(0x20, 0x20);
}