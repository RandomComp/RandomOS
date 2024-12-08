#include "../headers/x86emu.h"

#include "../headers/x86emu_regs.h"

#include "../headers/io.h"

#define htole16(x) ((uint16)(x))

#define htole32(x) ((uint32)(x))

#define letoh16(x) ((uint16)(x))

#define letoh32(x) ((uint32)(x))


static uint8 rdb(struct x86emu* emu, uint32 addr) {
	if (addr > emu->mem_size - 1)
		x86emu_halt_sys(emu);

	return emu->mem_base[addr];
}


static uint16 rdw(struct x86emu* emu, uint32 addr) {
	if (addr > emu->mem_size - 2)
		x86emu_halt_sys(emu);

	#ifdef __STRICT_ALIGNMENT
		if (addr & 1) {
			uint8* a = emu->mem_base + addr;

			uint16 r;

			r = ((*(a + 0) << 0) & 0x00ff) |
				((*(a + 1) << 8) & 0xff00);

			return r;
		} else
			return letoh32(*(uint32* )(emu->mem_base + addr));
	#else
		return letoh16(*(uint16* )(emu->mem_base + addr));
	#endif
}


static uint32 rdl(struct x86emu* emu, uint32 addr) {
	if (addr > emu->mem_size - 4)
		x86emu_halt_sys(emu);

	#ifdef __STRICT_ALIGNMENT
		if (addr & 3) {
			uint8* a = emu->mem_base + addr;

			uint32 r;

			r = ((*(a + 0) <<  0) & 0x000000ff) |
				((*(a + 1) <<  8) & 0x0000ff00) |
				((*(a + 2) << 16) & 0x00ff0000) |
				((*(a + 3) << 24) & 0xff000000);

			return r;
		} 

		else return letoh32(*(uint32* )(emu->mem_base + addr));
	#else
		return letoh32(*(uint32* )(emu->mem_base + addr));
	#endif
}


static void wrb(struct x86emu* emu, uint32 addr, uint8 val) {
	if (addr > emu->mem_size - 1)
		x86emu_halt_sys(emu);

	emu->mem_base[addr] = val;
}


static void wrw(struct x86emu* emu, uint32 addr, uint16 val) {
	if (addr > emu->mem_size - 2)
		x86emu_halt_sys(emu);

	#ifdef __STRICT_ALIGNMENT
		if (addr & 1) {
			uint8* a = emu->mem_base + addr;

			*((a + 0)) = (val >> 0) & 0xff;

			*((a + 1)) = (val >> 8) & 0xff;
		}

		else* ((uint16* )(emu->mem_base + addr)) = htole16(val);
	#else
		*((uint16* )(emu->mem_base + addr)) = htole16(val);
	#endif
}


static void wrl(struct x86emu* emu, uint32 addr, uint32 val) {
	if (addr > emu->mem_size - 4)
		x86emu_halt_sys(emu);

	#ifdef __STRICT_ALIGNMENT
		if (addr & 3) {
			uint8* a = emu->mem_base + addr;

			*((a + 0)) = (val >> 0) & 0xff;

			*((a + 1)) = (val >> 8) & 0xff;

			*((a + 2)) = (val >> 16) & 0xff;

			*((a + 3)) = (val >> 24) & 0xff;
		}

		else* ((uint32* )(emu->mem_base + addr)) = htole32(val);
	#else
		*((uint32* )(emu->mem_base + addr)) = htole32(val);
	#endif
}

static uint8 x86emu_inb(struct x86emu* emu, uint16 port) {
	uint8 val = 0;
	
	in8(port, &val);

	return val;
}

static uint16 x86emu_inw(struct x86emu* emu, uint16 port) {
	uint16 val = 0;

	in16(port, &val);

	return val;
}

static uint32 x86emu_inl(struct x86emu* emu, uint16 port) {
	uint32 val = 0;

	in32(port, &val);

	return val;
}

static void x86emu_outb(struct x86emu* emu, uint16 port, uint8 data) {
	out8(port, data);
}

static void x86emu_outw(struct x86emu* emu, uint16 port, uint16 data) {
	out16(port, data);
}

static void x86emu_outl(struct x86emu* emu, uint16 port, uint32 data) {
	out32(port, data);
}

void x86emu_init_default(struct x86emu* emu) {
	emu->emu_rdb = rdb;

	emu->emu_rdw = rdw;

	emu->emu_rdl = rdl;

	emu->emu_wrb = wrb;

	emu->emu_wrw = wrw;

	emu->emu_wrl = wrl;

	emu->emu_inb = x86emu_inb;

	emu->emu_inw = x86emu_inw;

	emu->emu_inl = x86emu_inl;

	emu->emu_outb = x86emu_outb;

	emu->emu_outw = x86emu_outw;
	
	emu->emu_outl = x86emu_outl;

	for (uint16 i = 0; i < 256; i++)
		emu->_x86emu_intrTab[i] = 0;
}