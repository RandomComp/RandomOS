#ifndef _IDT_H
#define _IDT_H

#include "types.h"

struct IDTEntryStruct {
	uint16 base_low;

	uint16 sel;

	uint8 always0;

	uint8 flags;

	uint16 base_high;
} PACKED;

struct IDTPtrStruct {
	uint16 limit;

	uint32 base;
} PACKED;

struct Registers {
	uint32 cr2;

	uint32 ds;

	uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;

	uint32 int_no, err_code;

	uint32 eip, csm, eflags, useresp, ss;
};

void IDTInit(void);

void IDTSetGate(uint8 num, uint32 base, uint16 sel, uint8 flags);

void IDTISRHandler(struct Registers* regs);

void IDTIRQInstallHandler(int32 irq, void (*handler)(struct Registers* regs));

void IDTIRQUninstallHandler(int32 irq);

void IDTIRQHandler(struct Registers* regs);

extern void isr0(void);

extern void isr1(void);

extern void isr2(void);

extern void isr3(void);

extern void isr4(void);

extern void isr5(void);

extern void isr6(void);

extern void isr7(void);

extern void isr8(void);

extern void isr9(void);

extern void isr10(void);

extern void isr11(void);

extern void isr12(void);

extern void isr13(void);

extern void isr14(void);

extern void isr15(void);

extern void isr16(void);

extern void isr17(void);

extern void isr18(void);

extern void isr19(void);

extern void isr20(void);

extern void isr21(void);

extern void isr22(void);

extern void isr23(void);

extern void isr24(void);

extern void isr25(void);

extern void isr26(void);

extern void isr27(void);

extern void isr28(void);

extern void isr29(void);

extern void isr30(void);

extern void isr31(void);

extern void isr17(void);


extern void irq0(void);

extern void irq1(void);

extern void irq2(void);

extern void irq3(void);

extern void irq4(void);

extern void irq5(void);

extern void irq6(void);

extern void irq7(void);

extern void irq8(void);

extern void irq9(void);

extern void irq10(void);

extern void irq11(void);

extern void irq12(void);

extern void irq13(void);

extern void irq14(void);

extern void irq15(void);


extern void isr128(void);

extern void isr177(void);

#endif