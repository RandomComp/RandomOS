#ifndef _X86EMU_H
#define _X86EMU_H

#include "types.h"

#include "setjmp.h"

#define __BEGIN_DECLS

#define __END_DECLS

#define __dead

struct x86emu_register32 {
	uint32 e_reg;
};

struct x86emu_register16 {
	uint16 x_reg;
};

struct x86emu_register8 {
	uint8 l_reg, h_reg;
};

union x86emu_register {
	struct x86emu_register32 I32_reg;

	struct x86emu_register16 I16_reg;

	struct x86emu_register8 I8_reg;
};

struct x86emu_regs {
	uint16 register_cs;

	uint16 register_ds;

	uint16 register_es;

	uint16 register_fs;

	uint16 register_gs;

	uint16 register_ss;

	uint32 register_flags;

	union x86emu_register register_a;

	union x86emu_register register_b;

	union x86emu_register register_c;

	union x86emu_register register_d;

	union x86emu_register register_sp;

	union x86emu_register register_bp;

	union x86emu_register register_si;

	union x86emu_register register_di;

	union x86emu_register register_ip;
	
	uint32 mode;

	volatile int32 intr;

	uint8 intno;

	uint8 __pad[3];
};

struct x86emu {
	int8* mem_base;

	uint32 mem_size;

	void* sys_private;

	struct x86emu_regs x86;

	jmp_buf exec_state;

	uint64 cur_cycles;

	uint32 cur_mod:2;

	uint32 cur_rl:3;

	uint32 cur_rh:3;

	uint32 cur_offset;

	uint8 (*emu_rdb)(struct x86emu*, uint32 addr);

	uint16 (*emu_rdw)(struct x86emu*, uint32 addr);

	uint32 (*emu_rdl)(struct x86emu*, uint32 addr);

	void (*emu_wrb)(struct x86emu*, uint32 addr,uint8 val);

	void (*emu_wrw)(struct x86emu*, uint32 addr, uint16 val);

	void (*emu_wrl)(struct x86emu*, uint32 addr, uint32 val);

	uint8 (*emu_inb)(struct x86emu*, uint16 addr);

	uint16 (*emu_inw)(struct x86emu*, uint16 addr);

	uint32 (*emu_inl)(struct x86emu*, uint16 addr);

	void (*emu_outb)(struct x86emu*, uint16 addr, uint8 val);

	void (*emu_outw)(struct x86emu*, uint16 addr, uint16 val);

	void (*emu_outl)(struct x86emu*, uint16 addr, uint32 val);

	void (*_x86emu_intrTab[256])(struct x86emu*, int32);
};

__BEGIN_DECLS

void x86emu_init_default(struct x86emu* emu);

void x86emu_exec(struct x86emu* emu);

void x86emu_exec_call(struct x86emu* emu, uint16 seg, uint16 off);

void x86emu_exec_intr(struct x86emu* emu, uint8 intr);

void x86emu_halt_sys(struct x86emu* emu) __dead;

__END_DECLS

#endif