#include "../headers/x86emu.h"

#include "../headers/x86emu_regs.h"

static void 	x86emu_intr_raise (struct x86emu *, uint8 type);

static void	x86emu_exec_one_byte(struct x86emu *);
static void	x86emu_exec_two_byte(struct x86emu *);

static void	fetch_decode_modrm (struct x86emu *);
static uint8	fetch_byte_imm (struct x86emu *);
static uint16	fetch_word_imm (struct x86emu *);
static uint32	fetch_long_imm (struct x86emu *);
static uint8	fetch_data_byte (struct x86emu *, uint32 offset);
static uint8	fetch_byte (struct x86emu *, uint32 segment, uint32 offset);
static uint16	fetch_data_word (struct x86emu *, uint32 offset);
static uint16	fetch_word (struct x86emu *, uint32 segment, uint32 offset);
static uint32	fetch_data_long (struct x86emu *, uint32 offset);
static uint32	fetch_long (struct x86emu *, uint32 segment, uint32 offset);
static void	store_data_byte (struct x86emu *, uint32 offset, uint8 val);
static void	store_byte (struct x86emu *, uint32 segment, uint32 offset, uint8 val);
static void	store_data_word (struct x86emu *, uint32 offset, uint16 val);
static void	store_word (struct x86emu *, uint32 segment, uint32 offset, uint16 val);
static void	store_data_long (struct x86emu *, uint32 offset, uint32 val);
static void	store_long (struct x86emu *, uint32 segment, uint32 offset, uint32 val);
static uint8*	decode_rl_byte_register(struct x86emu *);
static uint16*	decode_rl_word_register(struct x86emu *);
static uint32* 	decode_rl_long_register(struct x86emu *);
static uint8* 	decode_rh_byte_register(struct x86emu *);
static uint16* 	decode_rh_word_register(struct x86emu *);
static uint32* 	decode_rh_long_register(struct x86emu *);
static uint16* 	decode_rh_seg_register(struct x86emu *);
static uint32	decode_rl_address(struct x86emu *);

static uint8 	decode_and_fetch_byte(struct x86emu *);
static uint16 	decode_and_fetch_word(struct x86emu *);
static uint32 	decode_and_fetch_long(struct x86emu *);

static uint8 	decode_and_fetch_byte_imm8(struct x86emu *, uint8 *);
static uint16 	decode_and_fetch_word_imm8(struct x86emu *, uint8 *);
static uint32 	decode_and_fetch_long_imm8(struct x86emu *, uint8 *);

static uint16 	decode_and_fetch_word_disp(struct x86emu *, int16);
static uint32 	decode_and_fetch_long_disp(struct x86emu *, int16);

static void	write_back_byte(struct x86emu *, uint8);
static void	write_back_word(struct x86emu *, uint16);
static void	write_back_long(struct x86emu *, uint32);

static uint16	aaa_word (struct x86emu *, uint16 d);
static uint16	aas_word (struct x86emu *, uint16 d);
static uint16	aad_word (struct x86emu *, uint16 d);
static uint16	aam_word (struct x86emu *, uint8 d);
static uint8	adc_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	adc_word (struct x86emu *, uint16 d, uint16 s);
static uint32	adc_long (struct x86emu *, uint32 d, uint32 s);
static uint8	add_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	add_word (struct x86emu *, uint16 d, uint16 s);
static uint32	add_long (struct x86emu *, uint32 d, uint32 s);
static uint8	and_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	and_word (struct x86emu *, uint16 d, uint16 s);
static uint32	and_long (struct x86emu *, uint32 d, uint32 s);
static uint8	cmp_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	cmp_word (struct x86emu *, uint16 d, uint16 s);
static uint32	cmp_long (struct x86emu *, uint32 d, uint32 s);
static void	cmp_byte_no_return (struct x86emu *, uint8 d, uint8 s);
static void	cmp_word_no_return (struct x86emu *, uint16 d, uint16 s);
static void	cmp_long_no_return (struct x86emu *, uint32 d, uint32 s);
static uint8	daa_byte (struct x86emu *, uint8 d);
static uint8	das_byte (struct x86emu *, uint8 d);
static uint8	dec_byte (struct x86emu *, uint8 d);
static uint16	dec_word (struct x86emu *, uint16 d);
static uint32	dec_long (struct x86emu *, uint32 d);
static uint8	inc_byte (struct x86emu *, uint8 d);
static uint16	inc_word (struct x86emu *, uint16 d);
static uint32	inc_long (struct x86emu *, uint32 d);
static uint8	or_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	or_word (struct x86emu *, uint16 d, uint16 s);
static uint32	or_long (struct x86emu *, uint32 d, uint32 s);
static uint8	neg_byte (struct x86emu *, uint8 s);
static uint16	neg_word (struct x86emu *, uint16 s);
static uint32	neg_long (struct x86emu *, uint32 s);
static uint8	rcl_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	rcl_word (struct x86emu *, uint16 d, uint8 s);
static uint32	rcl_long (struct x86emu *, uint32 d, uint8 s);
static uint8	rcr_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	rcr_word (struct x86emu *, uint16 d, uint8 s);
static uint32	rcr_long (struct x86emu *, uint32 d, uint8 s);
static uint8	rol_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	rol_word (struct x86emu *, uint16 d, uint8 s);
static uint32	rol_long (struct x86emu *, uint32 d, uint8 s);
static uint8	ror_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	ror_word (struct x86emu *, uint16 d, uint8 s);
static uint32	ror_long (struct x86emu *, uint32 d, uint8 s);
static uint8	shl_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	shl_word (struct x86emu *, uint16 d, uint8 s);
static uint32	shl_long (struct x86emu *, uint32 d, uint8 s);
static uint8	shr_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	shr_word (struct x86emu *, uint16 d, uint8 s);
static uint32	shr_long (struct x86emu *, uint32 d, uint8 s);
static uint8	sar_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	sar_word (struct x86emu *, uint16 d, uint8 s);
static uint32	sar_long (struct x86emu *, uint32 d, uint8 s);
static uint16	shld_word (struct x86emu *, uint16 d, uint16 fill, uint8 s);
static uint32	shld_long (struct x86emu *, uint32 d, uint32 fill, uint8 s);
static uint16	shrd_word (struct x86emu *, uint16 d, uint16 fill, uint8 s);
static uint32	shrd_long (struct x86emu *, uint32 d, uint32 fill, uint8 s);
static uint8	sbb_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	sbb_word (struct x86emu *, uint16 d, uint16 s);
static uint32	sbb_long (struct x86emu *, uint32 d, uint32 s);
static uint8	sub_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	sub_word (struct x86emu *, uint16 d, uint16 s);
static uint32	sub_long (struct x86emu *, uint32 d, uint32 s);
static void	test_byte (struct x86emu *, uint8 d, uint8 s);
static void	test_word (struct x86emu *, uint16 d, uint16 s);
static void	test_long (struct x86emu *, uint32 d, uint32 s);
static uint8	xor_byte (struct x86emu *, uint8 d, uint8 s);
static uint16	xor_word (struct x86emu *, uint16 d, uint16 s);
static uint32	xor_long (struct x86emu *, uint32 d, uint32 s);
static void	imul_byte (struct x86emu *, uint8 s);
static void	imul_word (struct x86emu *, uint16 s);
static void	imul_long (struct x86emu *, uint32 s);
static void	mul_byte (struct x86emu *, uint8 s);
static void	mul_word (struct x86emu *, uint16 s);
static void	mul_long (struct x86emu *, uint32 s);
static void	idiv_byte (struct x86emu *, uint8 s);
static void	idiv_word (struct x86emu *, uint16 s);
static void	idiv_long (struct x86emu *, uint32 s);
static void	div_byte (struct x86emu *, uint8 s);
static void	div_word (struct x86emu *, uint16 s);
static void	div_long (struct x86emu *, uint32 s);
static void	ins (struct x86emu *, int32 size);
static void	outs (struct x86emu *, int32 size);
static void	push_word (struct x86emu *, uint16 w);
static void	push_long (struct x86emu *, uint32 w);
static uint16	pop_word (struct x86emu *);
static uint32	pop_long (struct x86emu *);


static void
x86emu_intr_dispatch(struct x86emu *emu, uint8 intno)
{
	if (emu->_x86emu_intrTab[intno]) {
		(*emu->_x86emu_intrTab[intno]) (emu, intno);
	} else {
		push_word(emu, (uint16) emu->x86.R_FLG);
		CLEAR_FLAG(F_IF);
		CLEAR_FLAG(F_TF);
		push_word(emu, emu->x86.R_CS);
		emu->x86.R_CS = fetch_word(emu, 0, intno * 4 + 2);
		push_word(emu, emu->x86.R_IP);
		emu->x86.R_IP = fetch_word(emu, 0, intno * 4);
	}
}

static void 
x86emu_intr_handle(struct x86emu *emu)
{
	uint8 intno;

	if (emu->x86.intr & INTR_SYNCH) {
		intno = emu->x86.intno;
		emu->x86.intr = 0;
		x86emu_intr_dispatch(emu, intno);
	}
}


void 
x86emu_intr_raise(struct x86emu *emu, uint8 intrnum)
{
	emu->x86.intno = intrnum;
	emu->x86.intr |= INTR_SYNCH;
}


void 
x86emu_exec(struct x86emu *emu)
{
	emu->x86.intr = 0;

	if (setjmp(emu->exec_state))
		return;

	for (;;) {
		if (emu->x86.intr) {
			if (((emu->x86.intr & INTR_SYNCH) &&
			    (emu->x86.intno == 0 || emu->x86.intno == 2)) ||
			    !ACCESS_FLAG(F_IF)) {
				x86emu_intr_handle(emu);
			}
		}

		if (emu->x86.R_CS == 0 && emu->x86.R_IP == 0)
			return;

		x86emu_exec_one_byte(emu);

		++emu->cur_cycles;
	}
}

void
x86emu_exec_call(struct x86emu *emu, uint16 seg, uint16 off)
{
	push_word(emu, 0);
	push_word(emu, 0);
	emu->x86.R_CS = seg;
	emu->x86.R_IP = off;

	x86emu_exec(emu);
}

void
x86emu_exec_intr(struct x86emu *emu, uint8 intr)
{
	push_word(emu, emu->x86.R_FLG);
	CLEAR_FLAG(F_IF);
	CLEAR_FLAG(F_TF);
	push_word(emu, 0);
	push_word(emu, 0);
	emu->x86.R_CS = (*emu->emu_rdw)(emu, intr * 4 + 2);
	emu->x86.R_IP = (*emu->emu_rdw)(emu, intr * 4);
	emu->x86.intr = 0;

	x86emu_exec(emu);
}


void 
x86emu_halt_sys(struct x86emu *emu)
{
#ifdef _KERNEL
	longjmp(&emu->exec_state);
#else
	longjmp(emu->exec_state, 1);
#endif
}


static void 
fetch_decode_modrm(struct x86emu *emu)
{
	int32 fetched;

	fetched = fetch_byte_imm(emu);
	emu->cur_mod = (fetched >> 6) & 0x03;
	emu->cur_rh = (fetched >> 3) & 0x07;
	emu->cur_rl = (fetched >> 0) & 0x07;
}


static uint8 
fetch_byte_imm(struct x86emu *emu)
{
	uint8 fetched;

	fetched = fetch_byte(emu, emu->x86.R_CS, emu->x86.R_IP);
	emu->x86.R_IP++;
	return fetched;
}


static uint16 
fetch_word_imm(struct x86emu *emu)
{
	uint16 fetched;

	fetched = fetch_word(emu, emu->x86.R_CS, emu->x86.R_IP);
	emu->x86.R_IP += 2;
	return fetched;
}


static uint32 
fetch_long_imm(struct x86emu *emu)
{
	uint32 fetched;

	fetched = fetch_long(emu, emu->x86.R_CS, emu->x86.R_IP);
	emu->x86.R_IP += 4;
	return fetched;
}


static uint32 
get_data_segment(struct x86emu *emu)
{
	switch (emu->x86.mode & SYSMODE_SEGMASK) {
	case 0:		
	case SYSMODE_SEGOVR_DS:
	case SYSMODE_SEGOVR_DS | SYSMODE_SEG_DS_SS:
		return emu->x86.R_DS;
	case SYSMODE_SEG_DS_SS:
		return emu->x86.R_SS;
	case SYSMODE_SEGOVR_CS:
	case SYSMODE_SEGOVR_CS | SYSMODE_SEG_DS_SS:
		return emu->x86.R_CS;
	case SYSMODE_SEGOVR_ES:
	case SYSMODE_SEGOVR_ES | SYSMODE_SEG_DS_SS:
		return emu->x86.R_ES;
	case SYSMODE_SEGOVR_FS:
	case SYSMODE_SEGOVR_FS | SYSMODE_SEG_DS_SS:
		return emu->x86.R_FS;
	case SYSMODE_SEGOVR_GS:
	case SYSMODE_SEGOVR_GS | SYSMODE_SEG_DS_SS:
		return emu->x86.R_GS;
	case SYSMODE_SEGOVR_SS:
	case SYSMODE_SEGOVR_SS | SYSMODE_SEG_DS_SS:
		return emu->x86.R_SS;
	}
	x86emu_halt_sys(emu);

	return 0;
}


static uint8 
fetch_data_byte(struct x86emu *emu, uint32 offset)
{
	return fetch_byte(emu, get_data_segment(emu), offset);
}


static uint16 
fetch_data_word(struct x86emu *emu, uint32 offset)
{
	return fetch_word(emu, get_data_segment(emu), offset);
}


static uint32 
fetch_data_long(struct x86emu *emu, uint32 offset)
{
	return fetch_long(emu, get_data_segment(emu), offset);
}


static uint8 
fetch_byte(struct x86emu *emu, uint32 segment, uint32 offset)
{
	return (*emu->emu_rdb) (emu, ((uint32) segment << 4) + offset);
}


static uint16 
fetch_word(struct x86emu *emu, uint32 segment, uint32 offset)
{
	return (*emu->emu_rdw) (emu, ((uint32) segment << 4) + offset);
}


static uint32 
fetch_long(struct x86emu *emu, uint32 segment, uint32 offset)
{
	return (*emu->emu_rdl) (emu, ((uint32) segment << 4) + offset);
}


static void 
store_data_byte(struct x86emu *emu, uint32 offset, uint8 val)
{
	store_byte(emu, get_data_segment(emu), offset, val);
}


static void 
store_data_word(struct x86emu *emu, uint32 offset, uint16 val)
{
	store_word(emu, get_data_segment(emu), offset, val);
}


static void 
store_data_long(struct x86emu *emu, uint32 offset, uint32 val)
{
	store_long(emu, get_data_segment(emu), offset, val);
}


static void 
store_byte(struct x86emu *emu, uint32 segment, uint32 offset, uint8 val)
{
	(*emu->emu_wrb) (emu, ((uint32) segment << 4) + offset, val);
}


static void 
store_word(struct x86emu *emu, uint32 segment, uint32 offset, uint16 val)
{
	(*emu->emu_wrw) (emu, ((uint32) segment << 4) + offset, val);
}


static void 
store_long(struct x86emu *emu, uint32 segment, uint32 offset, uint32 val)
{
	(*emu->emu_wrl) (emu, ((uint32) segment << 4) + offset, val);
}


static uint8 *
decode_rm_byte_register(struct x86emu *emu, int32 reg)
{
	switch (reg) {
	case 0:
		return &emu->x86.R_AL;
	case 1:
		return &emu->x86.R_CL;
	case 2:
		return &emu->x86.R_DL;
	case 3:
		return &emu->x86.R_BL;
	case 4:
		return &emu->x86.R_AH;
	case 5:
		return &emu->x86.R_CH;
	case 6:
		return &emu->x86.R_DH;
	case 7:
		return &emu->x86.R_BH;
	default:
		x86emu_halt_sys(emu);
	}

	return 0;
}

static uint8 *
decode_rl_byte_register(struct x86emu *emu)
{
	return decode_rm_byte_register(emu, emu->cur_rl);
}

static uint8 *
decode_rh_byte_register(struct x86emu *emu)
{
	return decode_rm_byte_register(emu, emu->cur_rh);
}


static uint16 *
decode_rm_word_register(struct x86emu *emu, int32 reg)
{
	switch (reg) {
	case 0:
		return &emu->x86.R_AX;
	case 1:
		return &emu->x86.R_CX;
	case 2:
		return &emu->x86.R_DX;
	case 3:
		return &emu->x86.R_BX;
	case 4:
		return &emu->x86.R_SP;
	case 5:
		return &emu->x86.R_BP;
	case 6:
		return &emu->x86.R_SI;
	case 7:
		return &emu->x86.R_DI;
	default:
		x86emu_halt_sys(emu);
	}

	return 0;
}

static uint16 *
decode_rl_word_register(struct x86emu *emu)
{
	return decode_rm_word_register(emu, emu->cur_rl);
}

static uint16 *
decode_rh_word_register(struct x86emu *emu)
{
	return decode_rm_word_register(emu, emu->cur_rh);
}


static uint32 *
decode_rm_long_register(struct x86emu *emu, int32 reg)
{
	switch (reg) {
	case 0:
		return &emu->x86.R_EAX;
	case 1:
		return &emu->x86.R_ECX;
	case 2:
		return &emu->x86.R_EDX;
	case 3:
		return &emu->x86.R_EBX;
	case 4:
		return &emu->x86.R_ESP;
	case 5:
		return &emu->x86.R_EBP;
	case 6:
		return &emu->x86.R_ESI;
	case 7:
		return &emu->x86.R_EDI;
	default:
		x86emu_halt_sys(emu);
	}

	return 0;
}

static uint32 *
decode_rl_long_register(struct x86emu *emu)
{
	return decode_rm_long_register(emu, emu->cur_rl);
}

static uint32 *
decode_rh_long_register(struct x86emu *emu)
{
	return decode_rm_long_register(emu, emu->cur_rh);
}



static uint16 *
decode_rh_seg_register(struct x86emu *emu)
{
	switch (emu->cur_rh) {
	case 0:
		return &emu->x86.R_ES;
	case 1:
		return &emu->x86.R_CS;
	case 2:
		return &emu->x86.R_SS;
	case 3:
		return &emu->x86.R_DS;
	case 4:
		return &emu->x86.R_FS;
	case 5:
		return &emu->x86.R_GS;
	default:
		x86emu_halt_sys(emu);
	}

	return 0;
}


static uint32 
decode_sib_address(struct x86emu *emu, int32 sib, int32 mod)
{
	uint32 base = 0, i = 0, scale = 1;

	switch (sib & 0x07) {
	case 0:
		base = emu->x86.R_EAX;
		break;
	case 1:
		base = emu->x86.R_ECX;

		break;
	case 2:
		base = emu->x86.R_EDX;
		break;
	case 3:
		base = emu->x86.R_EBX;
		break;
	case 4:
		base = emu->x86.R_ESP;
		emu->x86.mode |= SYSMODE_SEG_DS_SS;
		break;
	case 5:
		if (mod == 0) {
			base = fetch_long_imm(emu);
		} else {
			base = emu->x86.R_EBP;
			emu->x86.mode |= SYSMODE_SEG_DS_SS;
		}
		break;
	case 6:
		base = emu->x86.R_ESI;
		break;
	case 7:
		base = emu->x86.R_EDI;
		break;
	}
	switch ((sib >> 3) & 0x07) {
	case 0:
		i = emu->x86.R_EAX;
		break;
	case 1:
		i = emu->x86.R_ECX;
		break;
	case 2:
		i = emu->x86.R_EDX;
		break;
	case 3:
		i = emu->x86.R_EBX;
		break;
	case 4:
		i = 0;
		break;
	case 5:
		i = emu->x86.R_EBP;
		break;
	case 6:
		i = emu->x86.R_ESI;
		break;
	case 7:
		i = emu->x86.R_EDI;
		break;
	}
	scale = 1 << ((sib >> 6) & 0x03);
	return base + (i * scale);
}


static uint32
decode_rl_address(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_ADDR) {
		uint32 offset, sib;
		
		switch (emu->cur_rl) {
		case 0:
			offset = emu->x86.R_EAX;
			break;
		case 1:
			offset = emu->x86.R_ECX;
			break;
		case 2:
			offset = emu->x86.R_EDX;
			break;
		case 3:
			offset = emu->x86.R_EBX;
			break;
		case 4:
			sib = fetch_byte_imm(emu);
			offset = decode_sib_address(emu, sib, 0);
			break;
		case 5:
			if (emu->cur_mod == 0) {
				offset = fetch_long_imm(emu);
			} else {
				emu->x86.mode |= SYSMODE_SEG_DS_SS;
				offset = emu->x86.R_EBP;
			}
			break;
		case 6:
			offset = emu->x86.R_ESI;
			break;
		case 7:
			offset = emu->x86.R_EDI;
			break;
		default:
			x86emu_halt_sys(emu);
		}
		if (emu->cur_mod == 1)
			offset += (int8)fetch_byte_imm(emu);
		else if (emu->cur_mod == 2)
			offset += fetch_long_imm(emu);
		return offset;
	} else {
		uint16 offset;

		
		switch (emu->cur_rl) {
		case 0:
			offset = emu->x86.R_BX + emu->x86.R_SI;
			break;
		case 1:
			offset = emu->x86.R_BX + emu->x86.R_DI;
			break;
		case 2:
			emu->x86.mode |= SYSMODE_SEG_DS_SS;
			offset = emu->x86.R_BP + emu->x86.R_SI;
			break;
		case 3:
			emu->x86.mode |= SYSMODE_SEG_DS_SS;
			offset = emu->x86.R_BP + emu->x86.R_DI;
			break;
		case 4:
			offset = emu->x86.R_SI;
			break;
		case 5:
			offset = emu->x86.R_DI;
			break;
		case 6:
			if (emu->cur_mod == 0) {
				offset = fetch_word_imm(emu);
			} else {
				emu->x86.mode |= SYSMODE_SEG_DS_SS;
				offset = emu->x86.R_BP;
			}
			break;
		case 7:
			offset = emu->x86.R_BX;
			break;
		default:
			x86emu_halt_sys(emu);
		}
		if (emu->cur_mod == 1)
			offset += (int8)fetch_byte_imm(emu);
		else if (emu->cur_mod == 2)
			offset += fetch_word_imm(emu);
		return offset;
	}
}

static uint8
decode_and_fetch_byte(struct x86emu *emu)
{
	if (emu->cur_mod != 3) {
		emu->cur_offset = decode_rl_address(emu);
		return fetch_data_byte(emu, emu->cur_offset);
	} else {
		return *decode_rl_byte_register(emu);
	}
}

static uint16
decode_and_fetch_word_disp(struct x86emu *emu, int16 disp)
{
	if (emu->cur_mod != 3) {
		
		emu->cur_offset = decode_rl_address(emu) + disp;
		if ((emu->x86.mode & SYSMODE_PREFIX_ADDR) == 0)
			emu->cur_offset &= 0xffff;
		return fetch_data_word(emu, emu->cur_offset);
	} else {
		return *decode_rl_word_register(emu);
	}
}

static uint32
decode_and_fetch_long_disp(struct x86emu *emu, int16 disp)
{
	if (emu->cur_mod != 3) {
		
		emu->cur_offset = decode_rl_address(emu) + disp;
		if ((emu->x86.mode & SYSMODE_PREFIX_ADDR) == 0)
			emu->cur_offset &= 0xffff;
		return fetch_data_long(emu, emu->cur_offset);
	} else {
		return *decode_rl_long_register(emu);
	}
}

uint16
decode_and_fetch_word(struct x86emu *emu)
{
	return decode_and_fetch_word_disp(emu, 0);
}

uint32
decode_and_fetch_long(struct x86emu *emu)
{
	return decode_and_fetch_long_disp(emu, 0);
}

uint8
decode_and_fetch_byte_imm8(struct x86emu *emu, uint8 *imm)
{
	if (emu->cur_mod != 3) {
		emu->cur_offset = decode_rl_address(emu);
		*imm = fetch_byte_imm(emu);
		return fetch_data_byte(emu, emu->cur_offset);
	} else {
		*imm = fetch_byte_imm(emu);
		return *decode_rl_byte_register(emu);
	}
}

static uint16
decode_and_fetch_word_imm8(struct x86emu *emu, uint8 *imm)
{
	if (emu->cur_mod != 3) {
		emu->cur_offset = decode_rl_address(emu);
		*imm = fetch_byte_imm(emu);
		return fetch_data_word(emu, emu->cur_offset);
	} else {
		*imm = fetch_byte_imm(emu);
		return *decode_rl_word_register(emu);
	}
}

static uint32
decode_and_fetch_long_imm8(struct x86emu *emu, uint8 *imm)
{
	if (emu->cur_mod != 3) {
		emu->cur_offset = decode_rl_address(emu);
		*imm = fetch_byte_imm(emu);
		return fetch_data_long(emu, emu->cur_offset);
	} else {
		*imm = fetch_byte_imm(emu);
		return *decode_rl_long_register(emu);
	}
}

static void
write_back_byte(struct x86emu *emu, uint8 val)
{
	if (emu->cur_mod != 3)
		store_data_byte(emu, emu->cur_offset, val);
	else
		*decode_rl_byte_register(emu) = val;
}

static void
write_back_word(struct x86emu *emu, uint16 val)
{
	if (emu->cur_mod != 3)
		store_data_word(emu, emu->cur_offset, val);
	else
		*decode_rl_word_register(emu) = val;
}

static void
write_back_long(struct x86emu *emu, uint32 val)
{
	if (emu->cur_mod != 3)
		store_data_long(emu, emu->cur_offset, val);
	else
		*decode_rl_long_register(emu) = val;
}

static void
common_inc_word_long(struct x86emu *emu, union x86emu_register *reg)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		reg->I32_reg.e_reg = inc_long(emu, reg->I32_reg.e_reg);
	else
		reg->I16_reg.x_reg = inc_word(emu, reg->I16_reg.x_reg);
}

static void
common_dec_word_long(struct x86emu *emu, union x86emu_register *reg)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		reg->I32_reg.e_reg = dec_long(emu, reg->I32_reg.e_reg);
	else
		reg->I16_reg.x_reg = dec_word(emu, reg->I16_reg.x_reg);
}

static void
common_binop_byte_rm_r(struct x86emu *emu, 
    uint8 (*binop)(struct x86emu *, uint8, uint8))
{
	uint32 destoffset;
	uint8 *destreg, srcval;
	uint8 destval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_byte_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_byte(emu, destoffset);
		destval = (*binop)(emu, destval, srcval);
		store_data_byte(emu, destoffset, destval);
	} else {
		destreg = decode_rl_byte_register(emu);
		*destreg = (*binop)(emu, *destreg, srcval);
	}
}

static void
common_binop_ns_byte_rm_r(struct x86emu *emu, 
    void (*binop)(struct x86emu *, uint8, uint8))
{
	uint32 destoffset;
	uint8 destval, srcval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_byte_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_byte(emu, destoffset);
	} else {
		destval = *decode_rl_byte_register(emu);
	}
	(*binop)(emu, destval, srcval);
}

static void
common_binop_word_rm_r(struct x86emu *emu,
    uint16 (*binop)(struct x86emu *, uint16, uint16))
{
	uint32 destoffset;
	uint16 destval, *destreg, srcval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_word_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_word(emu, destoffset);
		destval = (*binop)(emu, destval, srcval);
		store_data_word(emu, destoffset, destval);
	} else {
		destreg = decode_rl_word_register(emu);
		*destreg = (*binop)(emu, *destreg, srcval);
	}
}

static void
common_binop_byte_r_rm(struct x86emu *emu,
    uint8 (*binop)(struct x86emu *, uint8, uint8))
{
	uint8 *destreg, srcval;
	uint32 srcoffset;

	fetch_decode_modrm(emu);
	destreg = decode_rh_byte_register(emu);
	if (emu->cur_mod != 3) {
		srcoffset = decode_rl_address(emu);
		srcval = fetch_data_byte(emu, srcoffset);
	} else {
		srcval = *decode_rl_byte_register(emu);
	}
	*destreg = (*binop)(emu, *destreg, srcval);
}

static void
common_binop_long_rm_r(struct x86emu *emu,
    uint32 (*binop)(struct x86emu *, uint32, uint32))
{
	uint32 destoffset;
	uint32 destval, *destreg, srcval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_long_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_long(emu, destoffset);
		destval = (*binop)(emu, destval, srcval);
		store_data_long(emu, destoffset, destval);
	} else {
		destreg = decode_rl_long_register(emu);
		*destreg = (*binop)(emu, *destreg, srcval);
	}
}

static void
common_binop_word_long_rm_r(struct x86emu *emu,
    uint16 (*binop16)(struct x86emu *, uint16, uint16),
    uint32 (*binop32)(struct x86emu *, uint32, uint32))
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		common_binop_long_rm_r(emu, binop32);
	else
		common_binop_word_rm_r(emu, binop16);
}

static void
common_binop_ns_word_rm_r(struct x86emu *emu,
    void (*binop)(struct x86emu *, uint16, uint16))
{
	uint32 destoffset;
	uint16 destval, srcval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_word_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_word(emu, destoffset);
	} else {
		destval = *decode_rl_word_register(emu);
	}
	(*binop)(emu, destval, srcval);
}


static void
common_binop_ns_long_rm_r(struct x86emu *emu,
    void (*binop)(struct x86emu *, uint32, uint32))
{
	uint32 destoffset;
	uint32 destval, srcval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_long_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_long(emu, destoffset);
	} else {
		destval = *decode_rl_long_register(emu);
	}
	(*binop)(emu, destval, srcval);
}

static void
common_binop_ns_word_long_rm_r(struct x86emu *emu,
    void (*binop16)(struct x86emu *, uint16, uint16),
    void (*binop32)(struct x86emu *, uint32, uint32))
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		common_binop_ns_long_rm_r(emu, binop32);
	else
		common_binop_ns_word_rm_r(emu, binop16);
}

static void
common_binop_long_r_rm(struct x86emu *emu,
    uint32 (*binop)(struct x86emu *, uint32, uint32))
{
	uint32 srcoffset;
	uint32 *destreg, srcval;

	fetch_decode_modrm(emu);
	destreg = decode_rh_long_register(emu);
	if (emu->cur_mod != 3) {
		srcoffset = decode_rl_address(emu);
		srcval = fetch_data_long(emu, srcoffset);
	} else {
		srcval = *decode_rl_long_register(emu);
	}
	*destreg = (*binop)(emu, *destreg, srcval);
}

static void
common_binop_word_r_rm(struct x86emu *emu,
    uint16 (*binop)(struct x86emu *, uint16, uint16))
{
	uint32 srcoffset;
	uint16 *destreg, srcval;

	fetch_decode_modrm(emu);
	destreg = decode_rh_word_register(emu);
	if (emu->cur_mod != 3) {
		srcoffset = decode_rl_address(emu);
		srcval = fetch_data_word(emu, srcoffset);
	} else {
		srcval = *decode_rl_word_register(emu);
	}
	*destreg = (*binop)(emu, *destreg, srcval);
}

static void
common_binop_word_long_r_rm(struct x86emu *emu,
    uint16 (*binop16)(struct x86emu *, uint16, uint16),
    uint32 (*binop32)(struct x86emu *, uint32, uint32))
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		common_binop_long_r_rm(emu, binop32);
	else
		common_binop_word_r_rm(emu, binop16);
}

static void
common_binop_byte_imm(struct x86emu *emu,
    uint8 (*binop)(struct x86emu *, uint8, uint8))
{
	uint8 srcval;

	srcval = fetch_byte_imm(emu);
	emu->x86.R_AL = (*binop)(emu, emu->x86.R_AL, srcval);
}

static void
common_binop_word_long_imm(struct x86emu *emu,
    uint16 (*binop16)(struct x86emu *, uint16, uint16),
    uint32 (*binop32)(struct x86emu *, uint32, uint32))
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		uint32 srcval;

		srcval = fetch_long_imm(emu);
		emu->x86.R_EAX = (*binop32)(emu, emu->x86.R_EAX, srcval);
	} else {
		uint16 srcval;

		srcval = fetch_word_imm(emu);
		emu->x86.R_AX = (*binop16)(emu, emu->x86.R_AX, srcval);
	}
}

static void
common_push_word_long(struct x86emu *emu, union x86emu_register *reg)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		push_long(emu, reg->I32_reg.e_reg);
	else
		push_word(emu, reg->I16_reg.x_reg);
}

static void
common_pop_word_long(struct x86emu *emu, union x86emu_register *reg)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		reg->I32_reg.e_reg = pop_long(emu);
	else
		reg->I16_reg.x_reg = pop_word(emu);
}

static void
common_imul_long_IMM(struct x86emu *emu, int32 byte_imm)
{
	uint32 srcoffset;
	uint32 *destreg, srcval;
	int32 imm;
	uint64 res;

	fetch_decode_modrm(emu);
	destreg = decode_rh_long_register(emu);
	if (emu->cur_mod != 3) {
		srcoffset = decode_rl_address(emu);
		srcval = fetch_data_long(emu, srcoffset);
	} else {
		srcval = *decode_rl_long_register(emu);
	}

	if (byte_imm)
		imm = (int8)fetch_byte_imm(emu);
	else
		imm = fetch_long_imm(emu);
	res = (int32)srcval * imm;

	if (res > 0xffffffff) {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	} else {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	}
	*destreg = (uint32)res;
}

static void
common_imul_word_IMM(struct x86emu *emu, int32 byte_imm)
{
	uint32 srcoffset;
	uint16 *destreg, srcval;
	int16 imm;
	uint32 res;

	fetch_decode_modrm(emu);
	destreg = decode_rh_word_register(emu);
	if (emu->cur_mod != 3) {
		srcoffset = decode_rl_address(emu);
		srcval = fetch_data_word(emu, srcoffset);
	} else {
		srcval = *decode_rl_word_register(emu);
	}

	if (byte_imm)
		imm = (int8)fetch_byte_imm(emu);
	else
		imm = fetch_word_imm(emu);
	res = (int16)srcval * imm;

	if (res > 0xffff) {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	} else {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	}
	*destreg = (uint16) res;
}

static void
common_imul_imm(struct x86emu *emu, int32 byte_imm)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		common_imul_long_IMM(emu, byte_imm);
	else
		common_imul_word_IMM(emu, byte_imm);
}

static void
common_jmp_near(struct x86emu *emu, int32 cond)
{
	int8 offset;
	uint16 target;

	offset = (int8) fetch_byte_imm(emu);
	target = (uint16) (emu->x86.R_IP + (int16) offset);
	if (cond)
		emu->x86.R_IP = target;
}

static void
common_load_far_pointer(struct x86emu *emu, uint16 *seg)
{
	uint16 *dstreg;
	uint32 srcoffset;

	fetch_decode_modrm(emu);
	if (emu->cur_mod == 3)
		x86emu_halt_sys(emu);

	dstreg = decode_rh_word_register(emu);
	srcoffset = decode_rl_address(emu);
	*dstreg = fetch_data_word(emu, srcoffset);
	*seg = fetch_data_word(emu, srcoffset + 2);
}




static void
x86emuOp_cmp_byte_R_RM(struct x86emu *emu)
{
	uint8 *destreg, srcval;

	fetch_decode_modrm(emu);
	destreg = decode_rh_byte_register(emu);
	srcval = decode_and_fetch_byte(emu);
	cmp_byte(emu, *destreg, srcval);
}


static void
x86emuOp32_cmp_word_R_RM(struct x86emu *emu)
{
	uint32 srcval, *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_long_register(emu);
	srcval = decode_and_fetch_long(emu);
	cmp_long(emu, *destreg, srcval);
}

static void
x86emuOp16_cmp_word_R_RM(struct x86emu *emu)
{
	uint16 srcval, *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_word_register(emu);
	srcval = decode_and_fetch_word(emu);
	cmp_word(emu, *destreg, srcval);
}

static void
x86emuOp_cmp_word_R_RM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_cmp_word_R_RM(emu);
	else
		x86emuOp16_cmp_word_R_RM(emu);
}


static void
x86emuOp_cmp_byte_AL_IMM(struct x86emu *emu)
{
	uint8 srcval;

	srcval = fetch_byte_imm(emu);
	cmp_byte(emu, emu->x86.R_AL, srcval);
}


static void
x86emuOp32_cmp_word_AX_IMM(struct x86emu *emu)
{
	uint32 srcval;

	srcval = fetch_long_imm(emu);
	cmp_long(emu, emu->x86.R_EAX, srcval);
}

static void
x86emuOp16_cmp_word_AX_IMM(struct x86emu *emu)
{
	uint16 srcval;

	srcval = fetch_word_imm(emu);
	cmp_word(emu, emu->x86.R_AX, srcval);
}

static void
x86emuOp_cmp_word_AX_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_cmp_word_AX_IMM(emu);
	else
		x86emuOp16_cmp_word_AX_IMM(emu);
}


static void
x86emuOp_push_all(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		uint32 old_sp = emu->x86.R_ESP;

		push_long(emu, emu->x86.R_EAX);
		push_long(emu, emu->x86.R_ECX);
		push_long(emu, emu->x86.R_EDX);
		push_long(emu, emu->x86.R_EBX);
		push_long(emu, old_sp);
		push_long(emu, emu->x86.R_EBP);
		push_long(emu, emu->x86.R_ESI);
		push_long(emu, emu->x86.R_EDI);
	} else {
		uint16 old_sp = emu->x86.R_SP;

		push_word(emu, emu->x86.R_AX);
		push_word(emu, emu->x86.R_CX);
		push_word(emu, emu->x86.R_DX);
		push_word(emu, emu->x86.R_BX);
		push_word(emu, old_sp);
		push_word(emu, emu->x86.R_BP);
		push_word(emu, emu->x86.R_SI);
		push_word(emu, emu->x86.R_DI);
	}
}


static void
x86emuOp_pop_all(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		emu->x86.R_EDI = pop_long(emu);
		emu->x86.R_ESI = pop_long(emu);
		emu->x86.R_EBP = pop_long(emu);
		emu->x86.R_ESP += 4;	
		emu->x86.R_EBX = pop_long(emu);
		emu->x86.R_EDX = pop_long(emu);
		emu->x86.R_ECX = pop_long(emu);
		emu->x86.R_EAX = pop_long(emu);
	} else {
		emu->x86.R_DI = pop_word(emu);
		emu->x86.R_SI = pop_word(emu);
		emu->x86.R_BP = pop_word(emu);
		emu->x86.R_SP += 2;
		emu->x86.R_BX = pop_word(emu);
		emu->x86.R_DX = pop_word(emu);
		emu->x86.R_CX = pop_word(emu);
		emu->x86.R_AX = pop_word(emu);
	}
}





static void
x86emuOp_push_word_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		uint32 imm;

		imm = fetch_long_imm(emu);
		push_long(emu, imm);
	} else {
		uint16 imm;

		imm = fetch_word_imm(emu);
		push_word(emu, imm);
	}
}


static void
x86emuOp_push_byte_IMM(struct x86emu *emu)
{
	int16 imm;

	imm = (int8) fetch_byte_imm(emu);
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		push_long(emu, (int32) imm);
	} else {
		push_word(emu, imm);
	}
}


static void
x86emuOp_ins_word(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		ins(emu, 4);
	} else {
		ins(emu, 2);
	}
}


static void
x86emuOp_outs_word(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		outs(emu, 4);
	} else {
		outs(emu, 2);
	}
}


static void
x86emuOp_jump_near_L(struct x86emu *emu)
{
	int32 sf, of;

	sf = ACCESS_FLAG(F_SF) != 0;
	of = ACCESS_FLAG(F_OF) != 0;

	common_jmp_near(emu, sf != of);
}


static void
x86emuOp_jump_near_NL(struct x86emu *emu)
{
	int32 sf, of;

	sf = ACCESS_FLAG(F_SF) != 0;
	of = ACCESS_FLAG(F_OF) != 0;

	common_jmp_near(emu, sf == of);
}


static void
x86emuOp_jump_near_LE(struct x86emu *emu)
{
	int32 sf, of;

	sf = ACCESS_FLAG(F_SF) != 0;
	of = ACCESS_FLAG(F_OF) != 0;

	common_jmp_near(emu, sf != of || ACCESS_FLAG(F_ZF));
}


static void
x86emuOp_jump_near_NLE(struct x86emu *emu)
{
	int32 sf, of;

	sf = ACCESS_FLAG(F_SF) != 0;
	of = ACCESS_FLAG(F_OF) != 0;

	common_jmp_near(emu, sf == of && !ACCESS_FLAG(F_ZF));
}

static
uint8(*const opc80_byte_operation[]) (struct x86emu *, uint8 d, uint8 s) =
{
	add_byte,		
	or_byte,		
	adc_byte,		
	sbb_byte,		
	and_byte,		
	sub_byte,		
	xor_byte,		
	cmp_byte,		
};


static void
x86emuOp_opc80_byte_RM_IMM(struct x86emu *emu)
{
	uint8 imm, destval;

	
	fetch_decode_modrm(emu);
	destval = decode_and_fetch_byte(emu);
	imm = fetch_byte_imm(emu);
	destval = (*opc80_byte_operation[emu->cur_rh]) (emu, destval, imm);
	if (emu->cur_rh != 7)
		write_back_byte(emu, destval);
}

static
uint16(* const opc81_word_operation[])
    (struct x86emu *, uint16 d, uint16 s) =
{
	add_word,		
	or_word,		
	adc_word,		
	sbb_word,		
	and_word,		
	sub_word,		
	xor_word,		
	cmp_word,		
};

static
uint32(* const opc81_long_operation[])
    (struct x86emu *, uint32 d, uint32 s) =
{
	add_long,		
	or_long,		
	adc_long,		
	sbb_long,		
	and_long,		
	sub_long,		
	xor_long,		
	cmp_long,		
};


static void
x86emuOp32_opc81_word_RM_IMM(struct x86emu *emu)
{
	uint32 destval, imm;

	
	fetch_decode_modrm(emu);
	destval = decode_and_fetch_long(emu);
	imm = fetch_long_imm(emu);
	destval = (*opc81_long_operation[emu->cur_rh]) (emu, destval, imm);
	if (emu->cur_rh != 7)
		write_back_long(emu, destval);
}

static void
x86emuOp16_opc81_word_RM_IMM(struct x86emu *emu)
{
	uint16 destval, imm;

	
	fetch_decode_modrm(emu);
	destval = decode_and_fetch_word(emu);
	imm = fetch_word_imm(emu);
	destval = (*opc81_word_operation[emu->cur_rh]) (emu, destval, imm);
	if (emu->cur_rh != 7)
		write_back_word(emu, destval);
}

static void
x86emuOp_opc81_word_RM_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_opc81_word_RM_IMM(emu);
	else
		x86emuOp16_opc81_word_RM_IMM(emu);
}

static
uint8(* const opc82_byte_operation[])
    (struct x86emu *, uint8 s, uint8 d) =
{
	add_byte,		
	or_byte,		
	adc_byte,		
	sbb_byte,		
	and_byte,		
	sub_byte,		
	xor_byte,		
	cmp_byte,		
};


static void
x86emuOp_opc82_byte_RM_IMM(struct x86emu *emu)
{
	uint8 imm, destval;

	
	fetch_decode_modrm(emu);
	destval = decode_and_fetch_byte(emu);
	imm = fetch_byte_imm(emu);
	destval = (*opc82_byte_operation[emu->cur_rh]) (emu, destval, imm);
	if (emu->cur_rh != 7)
		write_back_byte(emu, destval);
}

static
uint16(* const opc83_word_operation[])
    (struct x86emu *, uint16 s, uint16 d) =
{
	add_word,		
	or_word,		
	adc_word,		
	sbb_word,		
	and_word,		
	sub_word,		
	xor_word,		
	cmp_word,		
};

static
uint32(* const opc83_long_operation[])
    (struct x86emu *, uint32 s, uint32 d) =
{
	add_long,		
	or_long,		
	adc_long,		
	sbb_long,		
	and_long,		
	sub_long,		
	xor_long,		
	cmp_long,		
};


static void
x86emuOp32_opc83_word_RM_IMM(struct x86emu *emu)
{
	uint32 destval, imm;

	fetch_decode_modrm(emu);
	destval = decode_and_fetch_long(emu);
	imm = (int8) fetch_byte_imm(emu);
	destval = (*opc83_long_operation[emu->cur_rh]) (emu, destval, imm);
	if (emu->cur_rh != 7)
		write_back_long(emu, destval);
}

static void
x86emuOp16_opc83_word_RM_IMM(struct x86emu *emu)
{
	uint16 destval, imm;

	fetch_decode_modrm(emu);
	destval = decode_and_fetch_word(emu);
	imm = (int8) fetch_byte_imm(emu);
	destval = (*opc83_word_operation[emu->cur_rh]) (emu, destval, imm);
	if (emu->cur_rh != 7)
		write_back_word(emu, destval);
}

static void
x86emuOp_opc83_word_RM_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_opc83_word_RM_IMM(emu);
	else
		x86emuOp16_opc83_word_RM_IMM(emu);
}


static void
x86emuOp_xchg_byte_RM_R(struct x86emu *emu)
{
	uint8 *srcreg, destval, tmp;

	fetch_decode_modrm(emu);
	destval = decode_and_fetch_byte(emu);
	srcreg = decode_rh_byte_register(emu);
	tmp = destval;
	destval = *srcreg;
	*srcreg = tmp;
	write_back_byte(emu, destval);
}


static void
x86emuOp32_xchg_word_RM_R(struct x86emu *emu)
{
	uint32 *srcreg, destval, tmp;

	fetch_decode_modrm(emu);
	destval = decode_and_fetch_long(emu);
	srcreg = decode_rh_long_register(emu);
	tmp = destval;
	destval = *srcreg;
	*srcreg = tmp;
	write_back_long(emu, destval);
}

static void
x86emuOp16_xchg_word_RM_R(struct x86emu *emu)
{
	uint16 *srcreg, destval, tmp;

	fetch_decode_modrm(emu);
	destval = decode_and_fetch_word(emu);
	srcreg = decode_rh_word_register(emu);
	tmp = destval;
	destval = *srcreg;
	*srcreg = tmp;
	write_back_word(emu, destval);
}

static void
x86emuOp_xchg_word_RM_R(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_xchg_word_RM_R(emu);
	else
		x86emuOp16_xchg_word_RM_R(emu);
}


static void
x86emuOp_mov_byte_RM_R(struct x86emu *emu)
{
	uint8 *destreg, *srcreg;
	uint32 destoffset;

	fetch_decode_modrm(emu);
	srcreg = decode_rh_byte_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		store_data_byte(emu, destoffset, *srcreg);
	} else {
		destreg = decode_rl_byte_register(emu);
		*destreg = *srcreg;
	}
}


static void
x86emuOp32_mov_word_RM_R(struct x86emu *emu)
{
	uint32 destoffset;
	uint32 *destreg, srcval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_long_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		store_data_long(emu, destoffset, srcval);
	} else {
		destreg = decode_rl_long_register(emu);
		*destreg = srcval;
	}
}

static void
x86emuOp16_mov_word_RM_R(struct x86emu *emu)
{
	uint32 destoffset;
	uint16 *destreg, srcval;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_word_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		store_data_word(emu, destoffset, srcval);
	} else {
		destreg = decode_rl_word_register(emu);
		*destreg = srcval;
	}
}

static void
x86emuOp_mov_word_RM_R(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_mov_word_RM_R(emu);
	else
		x86emuOp16_mov_word_RM_R(emu);
}


static void
x86emuOp_mov_byte_R_RM(struct x86emu *emu)
{
	uint8 *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_byte_register(emu);
	*destreg = decode_and_fetch_byte(emu);
}


static void
x86emuOp_mov_word_R_RM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		uint32 *destreg;

		fetch_decode_modrm(emu);
		destreg = decode_rh_long_register(emu);
		*destreg = decode_and_fetch_long(emu);
	} else {
		uint16 *destreg;

		fetch_decode_modrm(emu);
		destreg = decode_rh_word_register(emu);
		*destreg = decode_and_fetch_word(emu);
	}
}


static void
x86emuOp_mov_word_RM_SR(struct x86emu *emu)
{
	uint16 *destreg, srcval;
	uint32 destoffset;

	fetch_decode_modrm(emu);
	srcval = *decode_rh_seg_register(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		store_data_word(emu, destoffset, srcval);
	} else {
		destreg = decode_rl_word_register(emu);
		*destreg = srcval;
	}
}


static void
x86emuOp_lea_word_R_M(struct x86emu *emu)
{
	uint16 *srcreg;
	uint32 destoffset;


	fetch_decode_modrm(emu);
	if (emu->cur_mod == 3)
		x86emu_halt_sys(emu);

	srcreg = decode_rh_word_register(emu);
	destoffset = decode_rl_address(emu);
	*srcreg = (uint16) destoffset;
}


static void
x86emuOp_mov_word_SR_RM(struct x86emu *emu)
{
	uint16 *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_seg_register(emu);
	*destreg = decode_and_fetch_word(emu);
	
}


static void
x86emuOp32_pop_RM(struct x86emu *emu)
{
	uint32 destoffset;
	uint32 destval, *destreg;

	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = pop_long(emu);
		store_data_long(emu, destoffset, destval);
	} else {
		destreg = decode_rl_long_register(emu);
		*destreg = pop_long(emu);
	}
}

static void
x86emuOp16_pop_RM(struct x86emu *emu)
{
	uint32 destoffset;
	uint16 destval, *destreg;

	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = pop_word(emu);
		store_data_word(emu, destoffset, destval);
	} else {
		destreg = decode_rl_word_register(emu);
		*destreg = pop_word(emu);
	}
}

static void
x86emuOp_pop_RM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_pop_RM(emu);
	else
		x86emuOp16_pop_RM(emu);
}


static void
x86emuOp_xchg_word_AX_CX(struct x86emu *emu)
{
	uint32 tmp;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		tmp = emu->x86.R_EAX;
		emu->x86.R_EAX = emu->x86.R_ECX;
		emu->x86.R_ECX = tmp;
	} else {
		tmp = emu->x86.R_AX;
		emu->x86.R_AX = emu->x86.R_CX;
		emu->x86.R_CX = (uint16) tmp;
	}
}


static void
x86emuOp_xchg_word_AX_DX(struct x86emu *emu)
{
	uint32 tmp;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		tmp = emu->x86.R_EAX;
		emu->x86.R_EAX = emu->x86.R_EDX;
		emu->x86.R_EDX = tmp;
	} else {
		tmp = emu->x86.R_AX;
		emu->x86.R_AX = emu->x86.R_DX;
		emu->x86.R_DX = (uint16) tmp;
	}
}


static void
x86emuOp_xchg_word_AX_BX(struct x86emu *emu)
{
	uint32 tmp;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		tmp = emu->x86.R_EAX;
		emu->x86.R_EAX = emu->x86.R_EBX;
		emu->x86.R_EBX = tmp;
	} else {
		tmp = emu->x86.R_AX;
		emu->x86.R_AX = emu->x86.R_BX;
		emu->x86.R_BX = (uint16) tmp;
	}
}


static void
x86emuOp_xchg_word_AX_SP(struct x86emu *emu)
{
	uint32 tmp;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		tmp = emu->x86.R_EAX;
		emu->x86.R_EAX = emu->x86.R_ESP;
		emu->x86.R_ESP = tmp;
	} else {
		tmp = emu->x86.R_AX;
		emu->x86.R_AX = emu->x86.R_SP;
		emu->x86.R_SP = (uint16) tmp;
	}
}


static void
x86emuOp_xchg_word_AX_BP(struct x86emu *emu)
{
	uint32 tmp;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		tmp = emu->x86.R_EAX;
		emu->x86.R_EAX = emu->x86.R_EBP;
		emu->x86.R_EBP = tmp;
	} else {
		tmp = emu->x86.R_AX;
		emu->x86.R_AX = emu->x86.R_BP;
		emu->x86.R_BP = (uint16) tmp;
	}
}


static void
x86emuOp_xchg_word_AX_SI(struct x86emu *emu)
{
	uint32 tmp;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		tmp = emu->x86.R_EAX;
		emu->x86.R_EAX = emu->x86.R_ESI;
		emu->x86.R_ESI = tmp;
	} else {
		tmp = emu->x86.R_AX;
		emu->x86.R_AX = emu->x86.R_SI;
		emu->x86.R_SI = (uint16) tmp;
	}
}


static void
x86emuOp_xchg_word_AX_DI(struct x86emu *emu)
{
	uint32 tmp;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		tmp = emu->x86.R_EAX;
		emu->x86.R_EAX = emu->x86.R_EDI;
		emu->x86.R_EDI = tmp;
	} else {
		tmp = emu->x86.R_AX;
		emu->x86.R_AX = emu->x86.R_DI;
		emu->x86.R_DI = (uint16) tmp;
	}
}


static void
x86emuOp_cbw(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		if (emu->x86.R_AX & 0x8000) {
			emu->x86.R_EAX |= 0xffff0000;
		} else {
			emu->x86.R_EAX &= 0x0000ffff;
		}
	} else {
		if (emu->x86.R_AL & 0x80) {
			emu->x86.R_AH = 0xff;
		} else {
			emu->x86.R_AH = 0x0;
		}
	}
}


static void
x86emuOp_cwd(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		if (emu->x86.R_EAX & 0x80000000) {
			emu->x86.R_EDX = 0xffffffff;
		} else {
			emu->x86.R_EDX = 0x0;
		}
	} else {
		if (emu->x86.R_AX & 0x8000) {
			emu->x86.R_DX = 0xffff;
		} else {
			emu->x86.R_DX = 0x0;
		}
	}
}


static void
x86emuOp_call_far_IMM(struct x86emu *emu)
{
	uint16 farseg, faroff;

	faroff = fetch_word_imm(emu);
	farseg = fetch_word_imm(emu);
	
	push_word(emu, emu->x86.R_CS);
	emu->x86.R_CS = farseg;
	push_word(emu, emu->x86.R_IP);
	emu->x86.R_IP = faroff;
}


static void
x86emuOp_pushf_word(struct x86emu *emu)
{
	uint32 flags;

	
	flags = (emu->x86.R_EFLG & F_MSK) | F_ALWAYS_ON;
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		push_long(emu, flags);
	} else {
		push_word(emu, (uint16) flags);
	}
}


static void
x86emuOp_popf_word(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		emu->x86.R_EFLG = pop_long(emu);
	} else {
		emu->x86.R_FLG = pop_word(emu);
	}
}


static void
x86emuOp_sahf(struct x86emu *emu)
{
	
	emu->x86.R_FLG &= 0xffffff00;
	
	emu->x86.R_FLG |= emu->x86.R_AH;
}


static void
x86emuOp_lahf(struct x86emu *emu)
{
	emu->x86.R_AH = (uint8) (emu->x86.R_FLG & 0xff);
	
	emu->x86.R_AH |= 0x2;
}


static void
x86emuOp_mov_AL_M_IMM(struct x86emu *emu)
{
	uint16 offset;

	offset = fetch_word_imm(emu);
	emu->x86.R_AL = fetch_data_byte(emu, offset);
}


static void
x86emuOp_mov_AX_M_IMM(struct x86emu *emu)
{
	uint16 offset;

	offset = fetch_word_imm(emu);
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		emu->x86.R_EAX = fetch_data_long(emu, offset);
	} else {
		emu->x86.R_AX = fetch_data_word(emu, offset);
	}
}


static void
x86emuOp_mov_M_AL_IMM(struct x86emu *emu)
{
	uint16 offset;

	offset = fetch_word_imm(emu);
	store_data_byte(emu, offset, emu->x86.R_AL);
}


static void
x86emuOp_mov_M_AX_IMM(struct x86emu *emu)
{
	uint16 offset;

	offset = fetch_word_imm(emu);
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		store_data_long(emu, offset, emu->x86.R_EAX);
	} else {
		store_data_word(emu, offset, emu->x86.R_AX);
	}
}


static void
x86emuOp_movs_byte(struct x86emu *emu)
{
	uint8 val;
	uint32 count;
	int32 inc;

	if (ACCESS_FLAG(F_DF))	
		inc = -1;
	else
		inc = 1;
	count = 1;
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		count = emu->x86.R_CX;
		emu->x86.R_CX = 0;
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	}
	while (count--) {
		val = fetch_data_byte(emu, emu->x86.R_SI);
		store_byte(emu, emu->x86.R_ES, emu->x86.R_DI, val);
		emu->x86.R_SI += inc;
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_movs_word(struct x86emu *emu)
{
	uint32 val;
	int32 inc;
	uint32 count;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		inc = 4;
	else
		inc = 2;

	if (ACCESS_FLAG(F_DF))	
		inc = -inc;

	count = 1;
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		count = emu->x86.R_CX;
		emu->x86.R_CX = 0;
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	}
	while (count--) {
		if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
			val = fetch_data_long(emu, emu->x86.R_SI);
			store_long(emu, emu->x86.R_ES, emu->x86.R_DI, val);
		} else {
			val = fetch_data_word(emu, emu->x86.R_SI);
			store_word(emu, emu->x86.R_ES, emu->x86.R_DI,
			    (uint16) val);
		}
		emu->x86.R_SI += inc;
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_cmps_byte(struct x86emu *emu)
{
	int8 val1, val2;
	int32 inc;

	if (ACCESS_FLAG(F_DF))	
		inc = -1;
	else
		inc = 1;

	if (emu->x86.mode & SYSMODE_PREFIX_REPE) {
		
		
		while (emu->x86.R_CX != 0) {
			val1 = fetch_data_byte(emu, emu->x86.R_SI);
			val2 = fetch_byte(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_byte(emu, val1, val2);
			emu->x86.R_CX -= 1;
			emu->x86.R_SI += inc;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF) == 0)
				break;
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPE;
	} else if (emu->x86.mode & SYSMODE_PREFIX_REPNE) {
		
		
		while (emu->x86.R_CX != 0) {
			val1 = fetch_data_byte(emu, emu->x86.R_SI);
			val2 = fetch_byte(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_byte(emu, val1, val2);
			emu->x86.R_CX -= 1;
			emu->x86.R_SI += inc;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF))
				break;	
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPNE;
	} else {
		val1 = fetch_data_byte(emu, emu->x86.R_SI);
		val2 = fetch_byte(emu, emu->x86.R_ES, emu->x86.R_DI);
		cmp_byte(emu, val1, val2);
		emu->x86.R_SI += inc;
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_cmps_word(struct x86emu *emu)
{
	uint32 val1, val2;
	int32 inc;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		if (ACCESS_FLAG(F_DF))	
			inc = -4;
		else
			inc = 4;
	} else {
		if (ACCESS_FLAG(F_DF))	
			inc = -2;
		else
			inc = 2;
	}
	if (emu->x86.mode & SYSMODE_PREFIX_REPE) {
		
		
		while (emu->x86.R_CX != 0) {
			if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
				val1 = fetch_data_long(emu, emu->x86.R_SI);
				val2 = fetch_long(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_long(emu, val1, val2);
			} else {
				val1 = fetch_data_word(emu, emu->x86.R_SI);
				val2 = fetch_word(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_word(emu, (uint16) val1, (uint16) val2);
			}
			emu->x86.R_CX -= 1;
			emu->x86.R_SI += inc;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF) == 0)
				break;
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPE;
	} else if (emu->x86.mode & SYSMODE_PREFIX_REPNE) {
		
		
		while (emu->x86.R_CX != 0) {
			if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
				val1 = fetch_data_long(emu, emu->x86.R_SI);
				val2 = fetch_long(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_long(emu, val1, val2);
			} else {
				val1 = fetch_data_word(emu, emu->x86.R_SI);
				val2 = fetch_word(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_word(emu, (uint16) val1, (uint16) val2);
			}
			emu->x86.R_CX -= 1;
			emu->x86.R_SI += inc;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF))
				break;	
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPNE;
	} else {
		if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
			val1 = fetch_data_long(emu, emu->x86.R_SI);
			val2 = fetch_long(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_long(emu, val1, val2);
		} else {
			val1 = fetch_data_word(emu, emu->x86.R_SI);
			val2 = fetch_word(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_word(emu, (uint16) val1, (uint16) val2);
		}
		emu->x86.R_SI += inc;
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_test_AX_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		test_long(emu, emu->x86.R_EAX, fetch_long_imm(emu));
	} else {
		test_word(emu, emu->x86.R_AX, fetch_word_imm(emu));
	}
}


static void
x86emuOp_stos_byte(struct x86emu *emu)
{
	int32 inc;

	if (ACCESS_FLAG(F_DF))	
		inc = -1;
	else
		inc = 1;
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		while (emu->x86.R_CX != 0) {
			store_byte(emu, emu->x86.R_ES, emu->x86.R_DI,
			    emu->x86.R_AL);
			emu->x86.R_CX -= 1;
			emu->x86.R_DI += inc;
		}
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	} else {
		store_byte(emu, emu->x86.R_ES, emu->x86.R_DI, emu->x86.R_AL);
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_stos_word(struct x86emu *emu)
{
	int32 inc;
	uint32 count;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		inc = 4;
	else
		inc = 2;
	
	if (ACCESS_FLAG(F_DF))	
		inc = -inc;

	count = 1;
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		count = emu->x86.R_CX;
		emu->x86.R_CX = 0;
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	}
	while (count--) {
		if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
			store_long(emu, emu->x86.R_ES, emu->x86.R_DI,
			    emu->x86.R_EAX);
		} else {
			store_word(emu, emu->x86.R_ES, emu->x86.R_DI,
			    emu->x86.R_AX);
		}
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_lods_byte(struct x86emu *emu)
{
	int32 inc;

	if (ACCESS_FLAG(F_DF))	
		inc = -1;
	else
		inc = 1;
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		while (emu->x86.R_CX != 0) {
			emu->x86.R_AL = fetch_data_byte(emu, emu->x86.R_SI);
			emu->x86.R_CX -= 1;
			emu->x86.R_SI += inc;
		}
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	} else {
		emu->x86.R_AL = fetch_data_byte(emu, emu->x86.R_SI);
		emu->x86.R_SI += inc;
	}
}


static void
x86emuOp_lods_word(struct x86emu *emu)
{
	int32 inc;
	uint32 count;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		inc = 4;
	else
		inc = 2;

	if (ACCESS_FLAG(F_DF))	
		inc = -inc;

	count = 1;
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		count = emu->x86.R_CX;
		emu->x86.R_CX = 0;
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	}
	while (count--) {
		if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
			emu->x86.R_EAX = fetch_data_long(emu, emu->x86.R_SI);
		} else {
			emu->x86.R_AX = fetch_data_word(emu, emu->x86.R_SI);
		}
		emu->x86.R_SI += inc;
	}
}


static void
x86emuOp_scas_byte(struct x86emu *emu)
{
	int8 val2;
	int32 inc;

	if (ACCESS_FLAG(F_DF))	
		inc = -1;
	else
		inc = 1;
	if (emu->x86.mode & SYSMODE_PREFIX_REPE) {
		
		
		while (emu->x86.R_CX != 0) {
			val2 = fetch_byte(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_byte(emu, emu->x86.R_AL, val2);
			emu->x86.R_CX -= 1;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF) == 0)
				break;
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPE;
	} else if (emu->x86.mode & SYSMODE_PREFIX_REPNE) {
		
		
		while (emu->x86.R_CX != 0) {
			val2 = fetch_byte(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_byte(emu, emu->x86.R_AL, val2);
			emu->x86.R_CX -= 1;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF))
				break;	
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPNE;
	} else {
		val2 = fetch_byte(emu, emu->x86.R_ES, emu->x86.R_DI);
		cmp_byte(emu, emu->x86.R_AL, val2);
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_scas_word(struct x86emu *emu)
{
	int32 inc;
	uint32 val;

	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		inc = 4;
	else
		inc = 2;

	if (ACCESS_FLAG(F_DF))	
		inc = -inc;

	if (emu->x86.mode & SYSMODE_PREFIX_REPE) {
		
		
		while (emu->x86.R_CX != 0) {
			if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
				val = fetch_long(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_long(emu, emu->x86.R_EAX, val);
			} else {
				val = fetch_word(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_word(emu, emu->x86.R_AX, (uint16) val);
			}
			emu->x86.R_CX -= 1;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF) == 0)
				break;
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPE;
	} else if (emu->x86.mode & SYSMODE_PREFIX_REPNE) {
		
		
		while (emu->x86.R_CX != 0) {
			if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
				val = fetch_long(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_long(emu, emu->x86.R_EAX, val);
			} else {
				val = fetch_word(emu, emu->x86.R_ES,
				    emu->x86.R_DI);
				cmp_word(emu, emu->x86.R_AX, (uint16) val);
			}
			emu->x86.R_CX -= 1;
			emu->x86.R_DI += inc;
			if (ACCESS_FLAG(F_ZF))
				break;	
		}
		emu->x86.mode &= ~SYSMODE_PREFIX_REPNE;
	} else {
		if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
			val = fetch_long(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_long(emu, emu->x86.R_EAX, val);
		} else {
			val = fetch_word(emu, emu->x86.R_ES, emu->x86.R_DI);
			cmp_word(emu, emu->x86.R_AX, (uint16) val);
		}
		emu->x86.R_DI += inc;
	}
}


static void
x86emuOp_mov_word_AX_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_EAX = fetch_long_imm(emu);
	else
		emu->x86.R_AX = fetch_word_imm(emu);
}


static void
x86emuOp_mov_word_CX_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_ECX = fetch_long_imm(emu);
	else
		emu->x86.R_CX = fetch_word_imm(emu);
}


static void
x86emuOp_mov_word_DX_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_EDX = fetch_long_imm(emu);
	else
		emu->x86.R_DX = fetch_word_imm(emu);
}


static void
x86emuOp_mov_word_BX_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_EBX = fetch_long_imm(emu);
	else
		emu->x86.R_BX = fetch_word_imm(emu);
}


static void
x86emuOp_mov_word_SP_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_ESP = fetch_long_imm(emu);
	else
		emu->x86.R_SP = fetch_word_imm(emu);
}


static void
x86emuOp_mov_word_BP_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_EBP = fetch_long_imm(emu);
	else
		emu->x86.R_BP = fetch_word_imm(emu);
}


static void
x86emuOp_mov_word_SI_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_ESI = fetch_long_imm(emu);
	else
		emu->x86.R_SI = fetch_word_imm(emu);
}


static void
x86emuOp_mov_word_DI_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		emu->x86.R_EDI = fetch_long_imm(emu);
	else
		emu->x86.R_DI = fetch_word_imm(emu);
}

static
uint8(* const opcD0_byte_operation[])
    (struct x86emu *, uint8 d, uint8 s) =
{
	rol_byte,
	ror_byte,
	rcl_byte,
	rcr_byte,
	shl_byte,
	shr_byte,
	shl_byte,		
	sar_byte,
};


static void
x86emuOp_opcC0_byte_RM_MEM(struct x86emu *emu)
{
	uint8 destval, amt;

	
	fetch_decode_modrm(emu);
	
	destval = decode_and_fetch_byte_imm8(emu, &amt);
	destval = (*opcD0_byte_operation[emu->cur_rh]) (emu, destval, amt);
	write_back_byte(emu, destval);
}

static
uint16(* const opcD1_word_operation[])
    (struct x86emu *, uint16 s, uint8 d) =
{
	rol_word,
	ror_word,
	rcl_word,
	rcr_word,
	shl_word,
	shr_word,
	shl_word,		
	sar_word,
};

static
uint32(* const opcD1_long_operation[])
    (struct x86emu *, uint32 s, uint8 d) =
{
	rol_long,
	ror_long,
	rcl_long,
	rcr_long,
	shl_long,
	shr_long,
	shl_long,		
	sar_long,
};


static void
x86emuOp_opcC1_word_RM_MEM(struct x86emu *emu)
{
	uint8 amt;

	
	fetch_decode_modrm(emu);
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		uint32 destval;

		destval = decode_and_fetch_long_imm8(emu, &amt);
		destval = (*opcD1_long_operation[emu->cur_rh])
		    (emu, destval, amt);
		write_back_long(emu, destval);
	} else {
		uint16 destval;

		destval = decode_and_fetch_word_imm8(emu, &amt);
		destval = (*opcD1_word_operation[emu->cur_rh])
		    (emu, destval, amt);
		write_back_word(emu, destval);
	}
}


static void
x86emuOp_ret_near_IMM(struct x86emu *emu)
{
	uint16 imm;

	imm = fetch_word_imm(emu);
	emu->x86.R_IP = pop_word(emu);
	emu->x86.R_SP += imm;
}


static void
x86emuOp_mov_byte_RM_IMM(struct x86emu *emu)
{
	uint8 *destreg;
	uint32 destoffset;
	uint8 imm;

	fetch_decode_modrm(emu);
	if (emu->cur_rh != 0)
		x86emu_halt_sys(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		imm = fetch_byte_imm(emu);
		store_data_byte(emu, destoffset, imm);
	} else {
		destreg = decode_rl_byte_register(emu);
		imm = fetch_byte_imm(emu);
		*destreg = imm;
	}
}


static void
x86emuOp32_mov_word_RM_IMM(struct x86emu *emu)
{
	uint32 destoffset;
	uint32 imm, *destreg;

	fetch_decode_modrm(emu);
	if (emu->cur_rh != 0)
		x86emu_halt_sys(emu);

	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		imm = fetch_long_imm(emu);
		store_data_long(emu, destoffset, imm);
	} else {
		destreg = decode_rl_long_register(emu);
		imm = fetch_long_imm(emu);
		*destreg = imm;
	}
}

static void
x86emuOp16_mov_word_RM_IMM(struct x86emu *emu)
{
	uint32 destoffset;
	uint16 imm, *destreg;

	fetch_decode_modrm(emu);
	if (emu->cur_rh != 0)
		x86emu_halt_sys(emu);

	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		imm = fetch_word_imm(emu);
		store_data_word(emu, destoffset, imm);
	} else {
		destreg = decode_rl_word_register(emu);
		imm = fetch_word_imm(emu);
		*destreg = imm;
	}
}

static void
x86emuOp_mov_word_RM_IMM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_mov_word_RM_IMM(emu);
	else
		x86emuOp16_mov_word_RM_IMM(emu);
}


static void
x86emuOp_enter(struct x86emu *emu)
{
	uint16 local, frame_pointer;
	uint8 nesting;
	int32 i;

	local = fetch_word_imm(emu);
	nesting = fetch_byte_imm(emu);
	push_word(emu, emu->x86.R_BP);
	frame_pointer = emu->x86.R_SP;
	if (nesting > 0) {
		for (i = 1; i < nesting; i++) {
			emu->x86.R_BP -= 2;
			push_word(emu, fetch_word(emu, emu->x86.R_SS,
			    emu->x86.R_BP));
		}
		push_word(emu, frame_pointer);
	}
	emu->x86.R_BP = frame_pointer;
	emu->x86.R_SP = (uint16) (emu->x86.R_SP - local);
}


static void
x86emuOp_leave(struct x86emu *emu)
{
	emu->x86.R_SP = emu->x86.R_BP;
	emu->x86.R_BP = pop_word(emu);
}


static void
x86emuOp_ret_far_IMM(struct x86emu *emu)
{
	uint16 imm;

	imm = fetch_word_imm(emu);
	emu->x86.R_IP = pop_word(emu);
	emu->x86.R_CS = pop_word(emu);
	emu->x86.R_SP += imm;
}


static void
x86emuOp_ret_far(struct x86emu *emu)
{
	emu->x86.R_IP = pop_word(emu);
	emu->x86.R_CS = pop_word(emu);
}


static void
x86emuOp_int3(struct x86emu *emu)
{
	x86emu_intr_dispatch(emu, 3);
}


static void
x86emuOp_int_IMM(struct x86emu *emu)
{
	uint8 intnum;

	intnum = fetch_byte_imm(emu);
	x86emu_intr_dispatch(emu, intnum);
}


static void
x86emuOp_into(struct x86emu *emu)
{
	if (ACCESS_FLAG(F_OF))
		x86emu_intr_dispatch(emu, 4);
}


static void
x86emuOp_iret(struct x86emu *emu)
{
	emu->x86.R_IP = pop_word(emu);
	emu->x86.R_CS = pop_word(emu);
	emu->x86.R_FLG = pop_word(emu);
}


static void
x86emuOp_opcD0_byte_RM_1(struct x86emu *emu)
{
	uint8 destval;

	fetch_decode_modrm(emu);
	destval = decode_and_fetch_byte(emu);
	destval = (*opcD0_byte_operation[emu->cur_rh]) (emu, destval, 1);
	write_back_byte(emu, destval);
}


static void
x86emuOp_opcD1_word_RM_1(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		uint32 destval;

		fetch_decode_modrm(emu);
		destval = decode_and_fetch_long(emu);
		destval = (*opcD1_long_operation[emu->cur_rh])(emu, destval, 1);
		write_back_long(emu, destval);
	} else {
		uint16 destval;

		fetch_decode_modrm(emu);
		destval = decode_and_fetch_word(emu);
		destval = (*opcD1_word_operation[emu->cur_rh])(emu, destval, 1);
		write_back_word(emu, destval);
	}
}


static void
x86emuOp_opcD2_byte_RM_CL(struct x86emu *emu)
{
	uint8 destval;

	fetch_decode_modrm(emu);
	destval = decode_and_fetch_byte(emu);
	destval = (*opcD0_byte_operation[emu->cur_rh])
	    (emu, destval, emu->x86.R_CL);
	write_back_byte(emu, destval);
}


static void
x86emuOp_opcD3_word_RM_CL(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		uint32 destval;

		fetch_decode_modrm(emu);
		destval = decode_and_fetch_long(emu);
		destval = (*opcD1_long_operation[emu->cur_rh])
		    (emu, destval, emu->x86.R_CL);
		write_back_long(emu, destval);
	} else {
		uint16 destval;

		fetch_decode_modrm(emu);
		destval = decode_and_fetch_word(emu);
		destval = (*opcD1_word_operation[emu->cur_rh])
		    (emu, destval, emu->x86.R_CL);
		write_back_word(emu, destval);
	}
}


static void
x86emuOp_aam(struct x86emu *emu)
{
	uint8 a;

	a = fetch_byte_imm(emu);	
	if (a != 10) {
		
		x86emu_halt_sys(emu);
	}
	
	emu->x86.R_AX = aam_word(emu, emu->x86.R_AL);
}


static void
x86emuOp_aad(struct x86emu *emu)
{
	uint8 a;

	a = fetch_byte_imm(emu);
	if (a != 10) {
		
		x86emu_halt_sys(emu);
	}
	emu->x86.R_AX = aad_word(emu, emu->x86.R_AX);
}




static void
x86emuOp_xlat(struct x86emu *emu)
{
	uint16 addr;

	addr = (uint16) (emu->x86.R_BX + (uint8) emu->x86.R_AL);
	emu->x86.R_AL = fetch_data_byte(emu, addr);
}


static void 
x86emuOp_esc_coprocess_d8(struct x86emu *emu)
{
}

static void 
x86emuOp_esc_coprocess_d9(struct x86emu *emu)
{
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3)
		decode_rl_address(emu);
}

static void 
x86emuOp_esc_coprocess_da(struct x86emu *emu)
{
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3)
		decode_rl_address(emu);
}

static void 
x86emuOp_esc_coprocess_db(struct x86emu *emu)
{
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3)
		decode_rl_address(emu);
}

static void 
x86emuOp_esc_coprocess_dc(struct x86emu *emu)
{
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3)
		decode_rl_address(emu);
}

static void 
x86emuOp_esc_coprocess_dd(struct x86emu *emu)
{
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3)
		decode_rl_address(emu);
}

static void 
x86emuOp_esc_coprocess_de(struct x86emu *emu)
{
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3)
		decode_rl_address(emu);
}

static void 
x86emuOp_esc_coprocess_df(struct x86emu *emu)
{
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3)
		decode_rl_address(emu);
}



static void
x86emuOp_loopne(struct x86emu *emu)
{
	int16 ip;

	ip = (int8) fetch_byte_imm(emu);
	ip += (int16) emu->x86.R_IP;
	emu->x86.R_CX -= 1;
	if (emu->x86.R_CX != 0 && !ACCESS_FLAG(F_ZF))	
		emu->x86.R_IP = ip;
}


static void
x86emuOp_loope(struct x86emu *emu)
{
	int16 ip;

	ip = (int8) fetch_byte_imm(emu);
	ip += (int16) emu->x86.R_IP;
	emu->x86.R_CX -= 1;
	if (emu->x86.R_CX != 0 && ACCESS_FLAG(F_ZF))	
		emu->x86.R_IP = ip;
}


static void
x86emuOp_loop(struct x86emu *emu)
{
	int16 ip;

	ip = (int8) fetch_byte_imm(emu);
	ip += (int16) emu->x86.R_IP;
	emu->x86.R_CX -= 1;
	if (emu->x86.R_CX != 0)
		emu->x86.R_IP = ip;
}


static void
x86emuOp_jcxz(struct x86emu *emu)
{
	uint16 target;
	int8 offset;

	
	offset = (int8) fetch_byte_imm(emu);
	target = (uint16) (emu->x86.R_IP + offset);
	if (emu->x86.R_CX == 0)
		emu->x86.R_IP = target;
}


static void
x86emuOp_in_byte_AL_IMM(struct x86emu *emu)
{
	uint8 port;

	port = (uint8) fetch_byte_imm(emu);
	emu->x86.R_AL = (*emu->emu_inb) (emu, port);
}


static void
x86emuOp_in_word_AX_IMM(struct x86emu *emu)
{
	uint8 port;

	port = (uint8) fetch_byte_imm(emu);
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		emu->x86.R_EAX = (*emu->emu_inl) (emu, port);
	} else {
		emu->x86.R_AX = (*emu->emu_inw) (emu, port);
	}
}


static void
x86emuOp_out_byte_IMM_AL(struct x86emu *emu)
{
	uint8 port;

	port = (uint8) fetch_byte_imm(emu);
	(*emu->emu_outb) (emu, port, emu->x86.R_AL);
}


static void
x86emuOp_out_word_IMM_AX(struct x86emu *emu)
{
	uint8 port;

	port = (uint8) fetch_byte_imm(emu);
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		(*emu->emu_outl) (emu, port, emu->x86.R_EAX);
	} else {
		(*emu->emu_outw) (emu, port, emu->x86.R_AX);
	}
}


static void
x86emuOp_call_near_IMM(struct x86emu *emu)
{
	int16 ip;

	ip = (int16) fetch_word_imm(emu);
	ip += (int16) emu->x86.R_IP;	
	push_word(emu, emu->x86.R_IP);
	emu->x86.R_IP = ip;
}


static void
x86emuOp_jump_near_IMM(struct x86emu *emu)
{
	int32 ip;

	ip = (int16) fetch_word_imm(emu);
	ip += (int16) emu->x86.R_IP;
	emu->x86.R_IP = (uint16) ip;
}


static void
x86emuOp_jump_far_IMM(struct x86emu *emu)
{
	uint16 cs, ip;

	ip = fetch_word_imm(emu);
	cs = fetch_word_imm(emu);
	emu->x86.R_IP = ip;
	emu->x86.R_CS = cs;
}


static void
x86emuOp_jump_byte_IMM(struct x86emu *emu)
{
	uint16 target;
	int8 offset;

	offset = (int8) fetch_byte_imm(emu);
	target = (uint16) (emu->x86.R_IP + offset);
	emu->x86.R_IP = target;
}


static void
x86emuOp_in_byte_AL_DX(struct x86emu *emu)
{
	emu->x86.R_AL = (*emu->emu_inb) (emu, emu->x86.R_DX);
}


static void
x86emuOp_in_word_AX_DX(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		emu->x86.R_EAX = (*emu->emu_inl) (emu, emu->x86.R_DX);
	} else {
		emu->x86.R_AX = (*emu->emu_inw) (emu, emu->x86.R_DX);
	}
}


static void
x86emuOp_out_byte_DX_AL(struct x86emu *emu)
{
	(*emu->emu_outb) (emu, emu->x86.R_DX, emu->x86.R_AL);
}


static void
x86emuOp_out_word_DX_AX(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
		(*emu->emu_outl) (emu, emu->x86.R_DX, emu->x86.R_EAX);
	} else {
		(*emu->emu_outw) (emu, emu->x86.R_DX, emu->x86.R_AX);
	}
}


static void
x86emuOp_lock(struct x86emu *emu)
{
}




static void
x86emuOp_cmc(struct x86emu *emu)
{
	if (ACCESS_FLAG(F_CF))
		CLEAR_FLAG(F_CF);
	else
		SET_FLAG(F_CF);
}


static void
x86emuOp_opcF6_byte_RM(struct x86emu *emu)
{
	uint8 destval, srcval;

	
	fetch_decode_modrm(emu);
	if (emu->cur_rh == 1)
		x86emu_halt_sys(emu);

	if (emu->cur_rh == 0) {
		destval = decode_and_fetch_byte_imm8(emu, &srcval);
		test_byte(emu, destval, srcval);
		return;
	}
	destval = decode_and_fetch_byte(emu);
	switch (emu->cur_rh) {
	case 2:
		destval = ~destval;
		write_back_byte(emu, destval);
		break;
	case 3:
		destval = neg_byte(emu, destval);
		write_back_byte(emu, destval);
		break;
	case 4:
		mul_byte(emu, destval);
		break;
	case 5:
		imul_byte(emu, destval);
		break;
	case 6:
		div_byte(emu, destval);
		break;
	case 7:
		idiv_byte(emu, destval);
		break;
	}
}


static void
x86emuOp32_opcF7_word_RM(struct x86emu *emu)
{
	uint32 destval, srcval;

	
	fetch_decode_modrm(emu);
	if (emu->cur_rh == 1)
		x86emu_halt_sys(emu);

	if (emu->cur_rh == 0) {
		if (emu->cur_mod != 3) {
			uint32 destoffset;

			destoffset = decode_rl_address(emu);
			srcval = fetch_long_imm(emu);
			destval = fetch_data_long(emu, destoffset);
		} else {
			srcval = fetch_long_imm(emu);
			destval = *decode_rl_long_register(emu);
		}
		test_long(emu, destval, srcval);
		return;
	}
	destval = decode_and_fetch_long(emu);
	switch (emu->cur_rh) {
	case 2:
		destval = ~destval;
		write_back_long(emu, destval);
		break;
	case 3:
		destval = neg_long(emu, destval);
		write_back_long(emu, destval);
		break;
	case 4:
		mul_long(emu, destval);
		break;
	case 5:
		imul_long(emu, destval);
		break;
	case 6:
		div_long(emu, destval);
		break;
	case 7:
		idiv_long(emu, destval);
		break;
	}
}
static void
x86emuOp16_opcF7_word_RM(struct x86emu *emu)
{
	uint16 destval, srcval;

	
	fetch_decode_modrm(emu);
	if (emu->cur_rh == 1)
		x86emu_halt_sys(emu);

	if (emu->cur_rh == 0) {
		if (emu->cur_mod != 3) {
			uint32 destoffset;

			destoffset = decode_rl_address(emu);
			srcval = fetch_word_imm(emu);
			destval = fetch_data_word(emu, destoffset);
		} else {
			srcval = fetch_word_imm(emu);
			destval = *decode_rl_word_register(emu);
		}
		test_word(emu, destval, srcval);
		return;
	}
	destval = decode_and_fetch_word(emu);
	switch (emu->cur_rh) {
	case 2:
		destval = ~destval;
		write_back_word(emu, destval);
		break;
	case 3:
		destval = neg_word(emu, destval);
		write_back_word(emu, destval);
		break;
	case 4:
		mul_word(emu, destval);
		break;
	case 5:
		imul_word(emu, destval);
		break;
	case 6:
		div_word(emu, destval);
		break;
	case 7:
		idiv_word(emu, destval);
		break;
	}
}
static void
x86emuOp_opcF7_word_RM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp32_opcF7_word_RM(emu);
	else
		x86emuOp16_opcF7_word_RM(emu);
}


static void
x86emuOp_opcFE_byte_RM(struct x86emu *emu)
{
	uint8 destval;
	uint32 destoffset;
	uint8 *destreg;

	
	fetch_decode_modrm(emu);
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		switch (emu->cur_rh) {
		case 0:	
			destval = fetch_data_byte(emu, destoffset);
			destval = inc_byte(emu, destval);
			store_data_byte(emu, destoffset, destval);
			break;
		case 1:	
			destval = fetch_data_byte(emu, destoffset);
			destval = dec_byte(emu, destval);
			store_data_byte(emu, destoffset, destval);
			break;
		}
	} else {
		destreg = decode_rl_byte_register(emu);
		switch (emu->cur_rh) {
		case 0:
			*destreg = inc_byte(emu, *destreg);
			break;
		case 1:
			*destreg = dec_byte(emu, *destreg);
			break;
		}
	}
}


static void
x86emuOp32_opcFF_word_RM(struct x86emu *emu)
{
	uint32 destoffset = 0;
	uint32 destval, *destreg;

	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_long(emu, destoffset);
		switch (emu->cur_rh) {
		case 0:	
			destval = inc_long(emu, destval);
			store_data_long(emu, destoffset, destval);
			break;
		case 1:	
			destval = dec_long(emu, destval);
			store_data_long(emu, destoffset, destval);
			break;
		case 6:	
			push_long(emu, destval);
			break;
		}
	} else {
		destreg = decode_rl_long_register(emu);
		switch (emu->cur_rh) {
		case 0:
			*destreg = inc_long(emu, *destreg);
			break;
		case 1:
			*destreg = dec_long(emu, *destreg);
			break;
		case 6:
			push_long(emu, *destreg);
			break;
		}
	}
}

static void
x86emuOp16_opcFF_word_RM(struct x86emu *emu)
{
	uint32 destoffset = 0;
	uint16 *destreg;
	uint16 destval;

	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_word(emu, destoffset);
		switch (emu->cur_rh) {
		case 0:
			destval = inc_word(emu, destval);
			store_data_word(emu, destoffset, destval);
			break;
		case 1:	
			destval = dec_word(emu, destval);
			store_data_word(emu, destoffset, destval);
			break;
		case 6:	
			push_word(emu, destval);
			break;
		}
	} else {
		destreg = decode_rl_word_register(emu);
		switch (emu->cur_rh) {
		case 0:
			*destreg = inc_word(emu, *destreg);
			break;
		case 1:
			*destreg = dec_word(emu, *destreg);
			break;
		case 6:
			push_word(emu, *destreg);
			break;
		}
	}
}

static void
x86emuOp_opcFF_word_RM(struct x86emu *emu)
{
	uint32 destoffset = 0;
	uint16 destval, destval2;

	
	fetch_decode_modrm(emu);
	if ((emu->cur_mod == 3 && (emu->cur_rh == 3 || emu->cur_rh == 5)) ||
	    emu->cur_rh == 7)
		x86emu_halt_sys(emu);
	if (emu->cur_rh == 0 || emu->cur_rh == 1 || emu->cur_rh == 6) {
		if (emu->x86.mode & SYSMODE_PREFIX_DATA)
			x86emuOp32_opcFF_word_RM(emu);
		else
			x86emuOp16_opcFF_word_RM(emu);
		return;
	}

	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		destval = fetch_data_word(emu, destoffset);
		switch (emu->cur_rh) {
		case 3:	
			destval2 = fetch_data_word(emu, destoffset + 2);
			push_word(emu, emu->x86.R_CS);
			emu->x86.R_CS = destval2;
			push_word(emu, emu->x86.R_IP);
			emu->x86.R_IP = destval;
			break;
		case 5:	
			destval2 = fetch_data_word(emu, destoffset + 2);
			emu->x86.R_IP = destval;
			emu->x86.R_CS = destval2;
			break;
		}
	} else {
		destval = *decode_rl_word_register(emu);
	}

	switch (emu->cur_rh) {
	case 2: 
		push_word(emu, emu->x86.R_IP);
		emu->x86.R_IP = destval;
		break;
	case 4: 
		emu->x86.R_IP = destval;
		break;
	}
}


static void
x86emu_exec_one_byte(struct x86emu * emu)
{
	uint8 op1 = fetch_byte_imm(emu);

	switch (op1) {
	case 0x00:
		common_binop_byte_rm_r(emu, add_byte);
		break;
	case 0x01:
		common_binop_word_long_rm_r(emu, add_word, add_long);
		break;
	case 0x02:
		common_binop_byte_r_rm(emu, add_byte);
		break;
	case 0x03:
		common_binop_word_long_r_rm(emu, add_word, add_long);
		break;
	case 0x04:
		common_binop_byte_imm(emu, add_byte);
		break;
	case 0x05:
		common_binop_word_long_imm(emu, add_word, add_long);
		break;
	case 0x06:
		push_word(emu, emu->x86.R_ES);
		break;
	case 0x07:
		emu->x86.R_ES = pop_word(emu);
		break;

	case 0x08:
		common_binop_byte_rm_r(emu, or_byte);
		break;
	case 0x09:
		common_binop_word_long_rm_r(emu, or_word, or_long);
		break;
	case 0x0a:
		common_binop_byte_r_rm(emu, or_byte);
		break;
	case 0x0b:
		common_binop_word_long_r_rm(emu, or_word, or_long);
		break;
	case 0x0c:
		common_binop_byte_imm(emu, or_byte);
		break;
	case 0x0d:
		common_binop_word_long_imm(emu, or_word, or_long);
		break;
	case 0x0e:
		push_word(emu, emu->x86.R_CS);
		break;
	case 0x0f:
		x86emu_exec_two_byte(emu);
		break;

	case 0x10:
		common_binop_byte_rm_r(emu, adc_byte);
		break;
	case 0x11:
		common_binop_word_long_rm_r(emu, adc_word, adc_long);
		break;
	case 0x12:
		common_binop_byte_r_rm(emu, adc_byte);
		break;
	case 0x13:
		common_binop_word_long_r_rm(emu, adc_word, adc_long);
		break;
	case 0x14:
		common_binop_byte_imm(emu, adc_byte);
		break;
	case 0x15:
		common_binop_word_long_imm(emu, adc_word, adc_long);
		break;
	case 0x16:
		push_word(emu, emu->x86.R_SS);
		break;
	case 0x17:
		emu->x86.R_SS = pop_word(emu);
		break;

	case 0x18:
		common_binop_byte_rm_r(emu, sbb_byte);
		break;
	case 0x19:
		common_binop_word_long_rm_r(emu, sbb_word, sbb_long);
		break;
	case 0x1a:
		common_binop_byte_r_rm(emu, sbb_byte);
		break;
	case 0x1b:
		common_binop_word_long_r_rm(emu, sbb_word, sbb_long);
		break;
	case 0x1c:
		common_binop_byte_imm(emu, sbb_byte);
		break;
	case 0x1d:
		common_binop_word_long_imm(emu, sbb_word, sbb_long);
		break;
	case 0x1e:
		push_word(emu, emu->x86.R_DS);
		break;
	case 0x1f:
		emu->x86.R_DS = pop_word(emu);
		break;

	case 0x20:
		common_binop_byte_rm_r(emu, and_byte);
		break;
	case 0x21:
		common_binop_word_long_rm_r(emu, and_word, and_long);
		break;
	case 0x22:
		common_binop_byte_r_rm(emu, and_byte);
		break;
	case 0x23:
		common_binop_word_long_r_rm(emu, and_word, and_long);
		break;
	case 0x24:
		common_binop_byte_imm(emu, and_byte);
		break;
	case 0x25:
		common_binop_word_long_imm(emu, and_word, and_long);
		break;
	case 0x26:
		emu->x86.mode |= SYSMODE_SEGOVR_ES;
		break;
	case 0x27:
		emu->x86.R_AL = daa_byte(emu, emu->x86.R_AL);
		break;

	case 0x28:
		common_binop_byte_rm_r(emu, sub_byte);
		break;
	case 0x29:
		common_binop_word_long_rm_r(emu, sub_word, sub_long);
		break;
	case 0x2a:
		common_binop_byte_r_rm(emu, sub_byte);
		break;
	case 0x2b:
		common_binop_word_long_r_rm(emu, sub_word, sub_long);
		break;
	case 0x2c:
		common_binop_byte_imm(emu, sub_byte);
		break;
	case 0x2d:
		common_binop_word_long_imm(emu, sub_word, sub_long);
		break;
	case 0x2e:
		emu->x86.mode |= SYSMODE_SEGOVR_CS;
		break;
	case 0x2f:
		emu->x86.R_AL = das_byte(emu, emu->x86.R_AL);
		break;

	case 0x30:
		common_binop_byte_rm_r(emu, xor_byte);
		break;
	case 0x31:
		common_binop_word_long_rm_r(emu, xor_word, xor_long);
		break;
	case 0x32:
		common_binop_byte_r_rm(emu, xor_byte);
		break;
	case 0x33:
		common_binop_word_long_r_rm(emu, xor_word, xor_long);
		break;
	case 0x34:
		common_binop_byte_imm(emu, xor_byte);
		break;
	case 0x35:
		common_binop_word_long_imm(emu, xor_word, xor_long);
		break;
	case 0x36:
		emu->x86.mode |= SYSMODE_SEGOVR_SS;
		break;
	case 0x37:
		emu->x86.R_AX = aaa_word(emu, emu->x86.R_AX);
		break;

	case 0x38:
		common_binop_ns_byte_rm_r(emu, cmp_byte_no_return);
		break;
	case 0x39:
		common_binop_ns_word_long_rm_r(emu, cmp_word_no_return,
		    cmp_long_no_return);
		break;
	case 0x3a:
		x86emuOp_cmp_byte_R_RM(emu);
		break;
	case 0x3b:
		x86emuOp_cmp_word_R_RM(emu);
		break;
	case 0x3c:
		x86emuOp_cmp_byte_AL_IMM(emu);
		break;
	case 0x3d:
		x86emuOp_cmp_word_AX_IMM(emu);
		break;
	case 0x3e:
		emu->x86.mode |= SYSMODE_SEGOVR_DS;
		break;
	case 0x3f:
		emu->x86.R_AX = aas_word(emu, emu->x86.R_AX);
		break;

	case 0x40:
		common_inc_word_long(emu, &emu->x86.register_a);
		break;
	case 0x41:
		common_inc_word_long(emu, &emu->x86.register_c);
		break;
	case 0x42:
		common_inc_word_long(emu, &emu->x86.register_d);
		break;
	case 0x43:
		common_inc_word_long(emu, &emu->x86.register_b);
		break;
	case 0x44:
		common_inc_word_long(emu, &emu->x86.register_sp);
		break;
	case 0x45:
		common_inc_word_long(emu, &emu->x86.register_bp);
		break;
	case 0x46:
		common_inc_word_long(emu, &emu->x86.register_si);
		break;
	case 0x47:
		common_inc_word_long(emu, &emu->x86.register_di);
		break;

	case 0x48:
		common_dec_word_long(emu, &emu->x86.register_a);
		break;
	case 0x49:
		common_dec_word_long(emu, &emu->x86.register_c);
		break;
	case 0x4a:
		common_dec_word_long(emu, &emu->x86.register_d);
		break;
	case 0x4b:
		common_dec_word_long(emu, &emu->x86.register_b);
		break;
	case 0x4c:
		common_dec_word_long(emu, &emu->x86.register_sp);
		break;
	case 0x4d:
		common_dec_word_long(emu, &emu->x86.register_bp);
		break;
	case 0x4e:
		common_dec_word_long(emu, &emu->x86.register_si);
		break;
	case 0x4f:
		common_dec_word_long(emu, &emu->x86.register_di);
		break;

	case 0x50:
		common_push_word_long(emu, &emu->x86.register_a);
		break;
	case 0x51:
		common_push_word_long(emu, &emu->x86.register_c);
		break;
	case 0x52:
		common_push_word_long(emu, &emu->x86.register_d);
		break;
	case 0x53:
		common_push_word_long(emu, &emu->x86.register_b);
		break;
	case 0x54:
		common_push_word_long(emu, &emu->x86.register_sp);
		break;
	case 0x55:
		common_push_word_long(emu, &emu->x86.register_bp);
		break;
	case 0x56:
		common_push_word_long(emu, &emu->x86.register_si);
		break;
	case 0x57:
		common_push_word_long(emu, &emu->x86.register_di);
		break;

	case 0x58:
		common_pop_word_long(emu, &emu->x86.register_a);
		break;
	case 0x59:
		common_pop_word_long(emu, &emu->x86.register_c);
		break;
	case 0x5a:
		common_pop_word_long(emu, &emu->x86.register_d);
		break;
	case 0x5b:
		common_pop_word_long(emu, &emu->x86.register_b);
		break;
	case 0x5c:
		common_pop_word_long(emu, &emu->x86.register_sp);
		break;
	case 0x5d:
		common_pop_word_long(emu, &emu->x86.register_bp);
		break;
	case 0x5e:
		common_pop_word_long(emu, &emu->x86.register_si);
		break;
	case 0x5f:
		common_pop_word_long(emu, &emu->x86.register_di);
		break;

	case 0x60:
		x86emuOp_push_all(emu);
		break;
	case 0x61:
		x86emuOp_pop_all(emu);
		break;
	
	
	case 0x64:
		emu->x86.mode |= SYSMODE_SEGOVR_FS;
		break;
	case 0x65:
		emu->x86.mode |= SYSMODE_SEGOVR_GS;
		break;
	case 0x66:
		emu->x86.mode |= SYSMODE_PREFIX_DATA;
		break;
	case 0x67:
		emu->x86.mode |= SYSMODE_PREFIX_ADDR;
		break;

	case 0x68:
		x86emuOp_push_word_IMM(emu);
		break;
	case 0x69:
		common_imul_imm(emu, 0);
		break;
	case 0x6a:
		x86emuOp_push_byte_IMM(emu);
		break;
	case 0x6b:
		common_imul_imm(emu, 1);
		break;
	case 0x6c:
		ins(emu, 1);
		break;
	case 0x6d:
		x86emuOp_ins_word(emu);
		break;
	case 0x6e:
		outs(emu, 1);
		break;
	case 0x6f:
		x86emuOp_outs_word(emu);
		break;

	case 0x70:
		common_jmp_near(emu, ACCESS_FLAG(F_OF));
		break;
	case 0x71:
		common_jmp_near(emu, !ACCESS_FLAG(F_OF));
		break;
	case 0x72:
		common_jmp_near(emu, ACCESS_FLAG(F_CF));
		break;
	case 0x73:
		common_jmp_near(emu, !ACCESS_FLAG(F_CF));
		break;
	case 0x74:
		common_jmp_near(emu, ACCESS_FLAG(F_ZF));
		break;
	case 0x75:
		common_jmp_near(emu, !ACCESS_FLAG(F_ZF));
		break;
	case 0x76:
		common_jmp_near(emu, ACCESS_FLAG(F_CF) || ACCESS_FLAG(F_ZF));
		break;
	case 0x77:
		common_jmp_near(emu, !ACCESS_FLAG(F_CF) && !ACCESS_FLAG(F_ZF));
		break;

	case 0x78:
		common_jmp_near(emu, ACCESS_FLAG(F_SF));
		break;
	case 0x79:
		common_jmp_near(emu, !ACCESS_FLAG(F_SF));
		break;
	case 0x7a:
		common_jmp_near(emu, ACCESS_FLAG(F_PF));
		break;
	case 0x7b:
		common_jmp_near(emu, !ACCESS_FLAG(F_PF));
		break;
	case 0x7c:
		x86emuOp_jump_near_L(emu);
		break;
	case 0x7d:
		x86emuOp_jump_near_NL(emu);
		break;
	case 0x7e:
		x86emuOp_jump_near_LE(emu);
		break;
	case 0x7f:
		x86emuOp_jump_near_NLE(emu);
		break;

	case 0x80:
		x86emuOp_opc80_byte_RM_IMM(emu);
		break;
	case 0x81:
		x86emuOp_opc81_word_RM_IMM(emu);
		break;
	case 0x82:
		x86emuOp_opc82_byte_RM_IMM(emu);
		break;
	case 0x83:
		x86emuOp_opc83_word_RM_IMM(emu);
		break;
	case 0x84:
		common_binop_ns_byte_rm_r(emu, test_byte);
		break;
	case 0x85:
		common_binop_ns_word_long_rm_r(emu, test_word, test_long);
		break;
	case 0x86:
		x86emuOp_xchg_byte_RM_R(emu);
		break;
	case 0x87:
		x86emuOp_xchg_word_RM_R(emu);
		break;

	case 0x88:
		x86emuOp_mov_byte_RM_R(emu);
		break;
	case 0x89:
		x86emuOp_mov_word_RM_R(emu);
		break;
	case 0x8a:
		x86emuOp_mov_byte_R_RM(emu);
		break;
	case 0x8b:
		x86emuOp_mov_word_R_RM(emu);
		break;
	case 0x8c:
		x86emuOp_mov_word_RM_SR(emu);
		break;
	case 0x8d:
		x86emuOp_lea_word_R_M(emu);
		break;
	case 0x8e:
		x86emuOp_mov_word_SR_RM(emu);
		break;
	case 0x8f:
		x86emuOp_pop_RM(emu);
		break;

	case 0x90:
		
		break;
	case 0x91:
		x86emuOp_xchg_word_AX_CX(emu);
		break;
	case 0x92:
		x86emuOp_xchg_word_AX_DX(emu);
		break;
	case 0x93:
		x86emuOp_xchg_word_AX_BX(emu);
		break;
	case 0x94:
		x86emuOp_xchg_word_AX_SP(emu);
		break;
	case 0x95:
		x86emuOp_xchg_word_AX_BP(emu);
		break;
	case 0x96:
		x86emuOp_xchg_word_AX_SI(emu);
		break;
	case 0x97:
		x86emuOp_xchg_word_AX_DI(emu);
		break;

	case 0x98:
		x86emuOp_cbw(emu);
		break;
	case 0x99:
		x86emuOp_cwd(emu);
		break;
	case 0x9a:
		x86emuOp_call_far_IMM(emu);
		break;
	case 0x9b:
		
		break;
	case 0x9c:
		x86emuOp_pushf_word(emu);
		break;
	case 0x9d:
		x86emuOp_popf_word(emu);
		break;
	case 0x9e:
		x86emuOp_sahf(emu);
		break;
	case 0x9f:
		x86emuOp_lahf(emu);
		break;

	case 0xa0:
		x86emuOp_mov_AL_M_IMM(emu);
		break;
	case 0xa1:
		x86emuOp_mov_AX_M_IMM(emu);
		break;
	case 0xa2:
		x86emuOp_mov_M_AL_IMM(emu);
		break;
	case 0xa3:
		x86emuOp_mov_M_AX_IMM(emu);
		break;
	case 0xa4:
		x86emuOp_movs_byte(emu);
		break;
	case 0xa5:
		x86emuOp_movs_word(emu);
		break;
	case 0xa6:
		x86emuOp_cmps_byte(emu);
		break;
	case 0xa7:
		x86emuOp_cmps_word(emu);
		break;

	case 0xa8:
		test_byte(emu, emu->x86.R_AL, fetch_byte_imm(emu));
		break;
	case 0xa9:
		x86emuOp_test_AX_IMM(emu);
		break;
	case 0xaa:
		x86emuOp_stos_byte(emu);
		break;
	case 0xab:
		x86emuOp_stos_word(emu);
		break;
	case 0xac:
		x86emuOp_lods_byte(emu);
		break;
	case 0xad:
		x86emuOp_lods_word(emu);
		break;
	case 0xae:
		x86emuOp_scas_byte(emu);
		break;
	case 0xaf:
		x86emuOp_scas_word(emu);
		break;

	case 0xb0:
		emu->x86.R_AL = fetch_byte_imm(emu);
		break;
	case 0xb1:
		emu->x86.R_CL = fetch_byte_imm(emu);
		break;
	case 0xb2:
		emu->x86.R_DL = fetch_byte_imm(emu);
		break;
	case 0xb3:
		emu->x86.R_BL = fetch_byte_imm(emu);
		break;
	case 0xb4:
		emu->x86.R_AH = fetch_byte_imm(emu);
		break;
	case 0xb5:
		emu->x86.R_CH = fetch_byte_imm(emu);
		break;
	case 0xb6:
		emu->x86.R_DH = fetch_byte_imm(emu);
		break;
	case 0xb7:
		emu->x86.R_BH = fetch_byte_imm(emu);
		break;

	case 0xb8:
		x86emuOp_mov_word_AX_IMM(emu);
		break;
	case 0xb9:
		x86emuOp_mov_word_CX_IMM(emu);
		break;
	case 0xba:
		x86emuOp_mov_word_DX_IMM(emu);
		break;
	case 0xbb:
		x86emuOp_mov_word_BX_IMM(emu);
		break;
	case 0xbc:

		x86emuOp_mov_word_SP_IMM(emu);
		break;
	case 0xbd:
		x86emuOp_mov_word_BP_IMM(emu);
		break;
	case 0xbe:
		x86emuOp_mov_word_SI_IMM(emu);
		break;
	case 0xbf:
		x86emuOp_mov_word_DI_IMM(emu);
		break;

	case 0xc0:
		x86emuOp_opcC0_byte_RM_MEM(emu);
		break;
	case 0xc1:
		x86emuOp_opcC1_word_RM_MEM(emu);
		break;
	case 0xc2:
		x86emuOp_ret_near_IMM(emu);
		break;
	case 0xc3:
		emu->x86.R_IP = pop_word(emu);
		break;
	case 0xc4:
		common_load_far_pointer(emu, &emu->x86.R_ES);
		break;
	case 0xc5:
		common_load_far_pointer(emu, &emu->x86.R_DS);
		break;
	case 0xc6:
		x86emuOp_mov_byte_RM_IMM(emu);
		break;
	case 0xc7:
		x86emuOp_mov_word_RM_IMM(emu);
		break;
	case 0xc8:
		x86emuOp_enter(emu);
		break;
	case 0xc9:
		x86emuOp_leave(emu);
		break;
	case 0xca:
		x86emuOp_ret_far_IMM(emu);
		break;
	case 0xcb:
		x86emuOp_ret_far(emu);
		break;
	case 0xcc:
		x86emuOp_int3(emu);
		break;
	case 0xcd:
		x86emuOp_int_IMM(emu);
		break;
	case 0xce:
		x86emuOp_into(emu);
		break;
	case 0xcf:
		x86emuOp_iret(emu);
		break;

	case 0xd0:
		x86emuOp_opcD0_byte_RM_1(emu);
		break;
	case 0xd1:
		x86emuOp_opcD1_word_RM_1(emu);
		break;
	case 0xd2:
		x86emuOp_opcD2_byte_RM_CL(emu);
		break;
	case 0xd3:
		x86emuOp_opcD3_word_RM_CL(emu);
		break;
	case 0xd4:
		x86emuOp_aam(emu);
		break;
	case 0xd5:
		x86emuOp_aad(emu);
		break;
	
	case 0xd7:
		x86emuOp_xlat(emu);
		break;
	case 0xd8:
		x86emuOp_esc_coprocess_d8(emu);
		break;
	case 0xd9:
		x86emuOp_esc_coprocess_d9(emu);
		break;
	case 0xda:
		x86emuOp_esc_coprocess_da(emu);
		break;
	case 0xdb:
		x86emuOp_esc_coprocess_db(emu);
		break;
	case 0xdc:
		x86emuOp_esc_coprocess_dc(emu);
		break;
	case 0xdd:
		x86emuOp_esc_coprocess_dd(emu);
		break;
	case 0xde:
		x86emuOp_esc_coprocess_de(emu);
		break;
	case 0xdf:
		x86emuOp_esc_coprocess_df(emu);
		break;

	case 0xe0:
		x86emuOp_loopne(emu);
		break;
	case 0xe1:
		x86emuOp_loope(emu);
		break;
	case 0xe2:
		x86emuOp_loop(emu);
		break;
	case 0xe3:
		x86emuOp_jcxz(emu);
		break;
	case 0xe4:
		x86emuOp_in_byte_AL_IMM(emu);
		break;
	case 0xe5:
		x86emuOp_in_word_AX_IMM(emu);
		break;
	case 0xe6:
		x86emuOp_out_byte_IMM_AL(emu);
		break;
	case 0xe7:
		x86emuOp_out_word_IMM_AX(emu);
		break;

	case 0xe8:
		x86emuOp_call_near_IMM(emu);
		break;
	case 0xe9:
		x86emuOp_jump_near_IMM(emu);
		break;
	case 0xea:
		x86emuOp_jump_far_IMM(emu);
		break;
	case 0xeb:
		x86emuOp_jump_byte_IMM(emu);
		break;
	case 0xec:
		x86emuOp_in_byte_AL_DX(emu);
		break;
	case 0xed:
		x86emuOp_in_word_AX_DX(emu);
		break;
	case 0xee:
		x86emuOp_out_byte_DX_AL(emu);
		break;
	case 0xef:
		x86emuOp_out_word_DX_AX(emu);
		break;

	case 0xf0:
		x86emuOp_lock(emu);
		break;
	case 0xf2:
		emu->x86.mode |= SYSMODE_PREFIX_REPNE;
		break;
	case 0xf3:
		emu->x86.mode |= SYSMODE_PREFIX_REPE;
		break;
	case 0xf4:
		x86emu_halt_sys(emu);
		break;
	case 0xf5:
		x86emuOp_cmc(emu);
		break;
	case 0xf6:
		x86emuOp_opcF6_byte_RM(emu);
		break;
	case 0xf7:
		x86emuOp_opcF7_word_RM(emu);
		break;

	case 0xf8:
		CLEAR_FLAG(F_CF);
		break;
	case 0xf9:
		SET_FLAG(F_CF);
		break;
	case 0xfa:
		CLEAR_FLAG(F_IF);
		break;
	case 0xfb:
		SET_FLAG(F_IF);
		break;
	case 0xfc:
		CLEAR_FLAG(F_DF);
		break;
	case 0xfd:
		SET_FLAG(F_DF);
		break;
	case 0xfe:
		x86emuOp_opcFE_byte_RM(emu);
		break;
	case 0xff:
		x86emuOp_opcFF_word_RM(emu);
		break;
	default:
		x86emu_halt_sys(emu);
		break;
	}
	if (op1 != 0x26 && op1 != 0x2e && op1 != 0x36 && op1 != 0x3e &&
	    (op1 | 3) != 0x67)
		emu->x86.mode &= ~SYSMODE_CLRMASK;
}

static void
common_jmp_long(struct x86emu *emu, int32 cond)
{
	int16 target;

	target = (int16) fetch_word_imm(emu);
	target += (int16) emu->x86.R_IP;
	if (cond)
		emu->x86.R_IP = (uint16) target;
}

static void
common_set_byte(struct x86emu *emu, int32 cond)
{
	uint32 destoffset;
	uint8 *destreg, destval;

	fetch_decode_modrm(emu);
	destval = cond ? 0x01 : 0x00;
	if (emu->cur_mod != 3) {
		destoffset = decode_rl_address(emu);
		store_data_byte(emu, destoffset, destval);
	} else {
		destreg = decode_rl_byte_register(emu);
		*destreg = destval;
	}
}

static void
common_bitstring32(struct x86emu *emu, int32 op)
{
	int32 bit;
	uint32 srcval, *shiftreg, mask;

	fetch_decode_modrm(emu);
	shiftreg = decode_rh_long_register(emu);
	srcval = decode_and_fetch_long_disp(emu, (int16) *shiftreg >> 5);
	bit = *shiftreg & 0x1F;
	mask =  0x1 << bit;
	CONDITIONAL_SET_FLAG(srcval & mask, F_CF);

	switch (op) {
	case 0:
		break;
	case 1:
		write_back_long(emu, srcval | mask);
		break;
	case 2:
		write_back_long(emu, srcval & ~mask);
		break;
	case 3:
		write_back_long(emu, srcval ^ mask);
		break;
	}
}

static void
common_bitstring16(struct x86emu *emu, int32 op)
{
	int32 bit;
	uint16 srcval, *shiftreg, mask;

	fetch_decode_modrm(emu);
	shiftreg = decode_rh_word_register(emu);
	srcval = decode_and_fetch_word_disp(emu, (int16) *shiftreg >> 4);
	bit = *shiftreg & 0xF;
	mask =  0x1 << bit;
	CONDITIONAL_SET_FLAG(srcval & mask, F_CF);

	switch (op) {
	case 0:
		break;
	case 1:
		write_back_word(emu, srcval | mask);
		break;
	case 2:
		write_back_word(emu, srcval & ~mask);
		break;
	case 3:
		write_back_word(emu, srcval ^ mask);
		break;
	}
}

static void
common_bitstring(struct x86emu *emu, int32 op)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		common_bitstring32(emu, op);
	else
		common_bitstring16(emu, op);
}

static void
common_bitsearch32(struct x86emu *emu, int32 diff)
{
	uint32 srcval, *dstreg;

	fetch_decode_modrm(emu);
	dstreg = decode_rh_long_register(emu);
	srcval = decode_and_fetch_long(emu);
	CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	for (*dstreg = 0; *dstreg < 32; *dstreg += diff) {
		if ((srcval >> *dstreg) & 1)
			break;
	}
}

static void
common_bitsearch16(struct x86emu *emu, int32 diff)
{
	uint16 srcval, *dstreg;

	fetch_decode_modrm(emu);
	dstreg = decode_rh_word_register(emu);
	srcval = decode_and_fetch_word(emu);
	CONDITIONAL_SET_FLAG(srcval == 0, F_ZF);
	for (*dstreg = 0; *dstreg < 16; *dstreg += diff) {
		if ((srcval >> *dstreg) & 1)
			break;
	}
}

static void
common_bitsearch(struct x86emu *emu, int32 diff)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		common_bitsearch32(emu, diff);
	else
		common_bitsearch16(emu, diff);
}

static void
common_shift32(struct x86emu *emu, int32 shift_left, int32 use_cl)
{
	uint8 shift;
	uint32 destval, *shiftreg;

	fetch_decode_modrm(emu);
	shiftreg = decode_rh_long_register(emu);
	if (use_cl) {
		destval = decode_and_fetch_long(emu);
		shift = emu->x86.R_CL;
	} else {
		destval = decode_and_fetch_long_imm8(emu, &shift);
	}
	if (shift_left)
		destval = shld_long(emu, destval, *shiftreg, shift);
	else
		destval = shrd_long(emu, destval, *shiftreg, shift);
	write_back_long(emu, destval);
}

static void
common_shift16(struct x86emu *emu, int32 shift_left, int32 use_cl)
{
	uint8 shift;
	uint16 destval, *shiftreg;

	fetch_decode_modrm(emu);
	shiftreg = decode_rh_word_register(emu);
	if (use_cl) {
		destval = decode_and_fetch_word(emu);
		shift = emu->x86.R_CL;
	} else {
		destval = decode_and_fetch_word_imm8(emu, &shift);
	}
	if (shift_left)
		destval = shld_word(emu, destval, *shiftreg, shift);
	else
		destval = shrd_word(emu, destval, *shiftreg, shift);
	write_back_word(emu, destval);
}

static void
common_shift(struct x86emu *emu, int32 shift_left, int32 use_cl)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		common_shift32(emu, shift_left, use_cl);
	else
		common_shift16(emu, shift_left, use_cl);
}


#define xorl(a,b)   ((a) && !(b)) || (!(a) && (b))



static void
x86emuOp2_rdtsc(struct x86emu *emu)
{
	emu->x86.R_EAX = emu->cur_cycles & 0xffffffff;
	emu->x86.R_EDX = emu->cur_cycles >> 32;
}


static void
x86emuOp2_push_FS(struct x86emu *emu)
{
	push_word(emu, emu->x86.R_FS);
}


static void
x86emuOp2_pop_FS(struct x86emu *emu)
{
	emu->x86.R_FS = pop_word(emu);
}


#if defined(__i386__) || defined(__amd64__)
static void
hw_cpuid(uint32 *a, uint32 *b, uint32 *c, uint32 *d)
{
	__asm__ __volatile__("cpuid"
			     : "=a" (*a), "=b" (*b),
			       "=c" (*c), "=d" (*d)
			     : "a" (*a), "c" (*c)
			     : "cc");
}
#endif
static void
x86emuOp2_cpuid(struct x86emu *emu)
{
#if defined(__i386__) || defined(__amd64__)
	hw_cpuid(&emu->x86.R_EAX, &emu->x86.R_EBX, &emu->x86.R_ECX,
	    &emu->x86.R_EDX);
#endif
	switch (emu->x86.R_EAX) {
	case 0:
		emu->x86.R_EAX = 1;
#if !defined(__i386__) && !defined(__amd64__)
		
		emu->x86.R_EBX = 0x756e6547;
		emu->x86.R_EDX = 0x49656e69;
		emu->x86.R_ECX = 0x6c65746e;
#endif
		break;
	case 1:
#if !defined(__i386__) && !defined(__amd64__)
		emu->x86.R_EAX = 0x00000480;
		emu->x86.R_EBX = emu->x86.R_ECX = 0;
		emu->x86.R_EDX = 0x00000002;
#else
		emu->x86.R_EDX &= 0x00000012;
#endif
		break;
	default:
		emu->x86.R_EAX = emu->x86.R_EBX = emu->x86.R_ECX =
		    emu->x86.R_EDX = 0;
		break;
	}
}


static void
x86emuOp2_bt_R(struct x86emu *emu)
{
	common_bitstring(emu, 0);
}


static void
x86emuOp2_shld_IMM(struct x86emu *emu)
{
	common_shift(emu, 1, 0);
}


static void
x86emuOp2_shld_CL(struct x86emu *emu)
{
	common_shift(emu, 1, 1);
}


static void
x86emuOp2_push_GS(struct x86emu *emu)
{
	push_word(emu, emu->x86.R_GS);
}


static void
x86emuOp2_pop_GS(struct x86emu *emu)
{
	emu->x86.R_GS = pop_word(emu);
}


static void
x86emuOp2_bts_R(struct x86emu *emu)
{
	common_bitstring(emu, 1);
}


static void
x86emuOp2_shrd_IMM(struct x86emu *emu)
{
	common_shift(emu, 0, 0);
}


static void
x86emuOp2_shrd_CL(struct x86emu *emu)
{
	common_shift(emu, 0, 1);
}


static void
x86emuOp2_32_imul_R_RM(struct x86emu *emu)
{
	uint32 *destreg, srcval;
	uint64 res;

	fetch_decode_modrm(emu);
	destreg = decode_rh_long_register(emu);
	srcval = decode_and_fetch_long(emu);
	res = (int32) *destreg * (int32)srcval;
	if (res > 0xffffffff) {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	} else {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	}
	*destreg = (uint32) res;
}

static void
x86emuOp2_16_imul_R_RM(struct x86emu *emu)
{
	uint16 *destreg, srcval;
	uint32 res;

	fetch_decode_modrm(emu);
	destreg = decode_rh_word_register(emu);
	srcval = decode_and_fetch_word(emu);
	res = (int16) * destreg * (int16)srcval;
	if (res > 0xFFFF) {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	} else {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	}
	*destreg = (uint16) res;
}

static void
x86emuOp2_imul_R_RM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp2_32_imul_R_RM(emu);
	else
		x86emuOp2_16_imul_R_RM(emu);
}


static void
x86emuOp2_lss_R_IMM(struct x86emu *emu)
{
	common_load_far_pointer(emu, &emu->x86.R_SS);
}


static void
x86emuOp2_btr_R(struct x86emu *emu)
{
	common_bitstring(emu, 2);
}


static void
x86emuOp2_lfs_R_IMM(struct x86emu *emu)
{
	common_load_far_pointer(emu, &emu->x86.R_FS);
}


static void
x86emuOp2_lgs_R_IMM(struct x86emu *emu)
{
	common_load_far_pointer(emu, &emu->x86.R_GS);
}


static void
x86emuOp2_32_movzx_byte_R_RM(struct x86emu *emu)
{
	uint32 *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_long_register(emu);
	*destreg = decode_and_fetch_byte(emu);
}

static void
x86emuOp2_16_movzx_byte_R_RM(struct x86emu *emu)
{
	uint16 *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_word_register(emu);
	*destreg = decode_and_fetch_byte(emu);
}

static void
x86emuOp2_movzx_byte_R_RM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp2_32_movzx_byte_R_RM(emu);
	else
		x86emuOp2_16_movzx_byte_R_RM(emu);
}


static void
x86emuOp2_movzx_word_R_RM(struct x86emu *emu)
{
	uint32 *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_long_register(emu);
	*destreg = decode_and_fetch_word(emu);
}


static void
x86emuOp2_32_btX_I(struct x86emu *emu)
{
	int32 bit;
	uint32 srcval, mask;
	uint8 shift;

	fetch_decode_modrm(emu);
	if (emu->cur_rh < 4)
		x86emu_halt_sys(emu);

	srcval = decode_and_fetch_long_imm8(emu, &shift);
	bit = shift & 0x1F;
	mask = (0x1 << bit);

	switch (emu->cur_rh) {
	case 5:
		write_back_long(emu, srcval | mask);
		break;
	case 6:
		write_back_long(emu, srcval & ~mask);
		break;
	case 7:
		write_back_long(emu, srcval ^ mask);
		break;
	}
	CONDITIONAL_SET_FLAG(srcval & mask, F_CF);
}

static void
x86emuOp2_16_btX_I(struct x86emu *emu)
{
	int32 bit;

	uint16 srcval, mask;
	uint8 shift;

	fetch_decode_modrm(emu);
	if (emu->cur_rh < 4)
		x86emu_halt_sys(emu);

	srcval = decode_and_fetch_word_imm8(emu, &shift);
	bit = shift & 0xF;
	mask = (0x1 << bit);
	switch (emu->cur_rh) {
	case 5:
		write_back_word(emu, srcval | mask);
		break;
	case 6:
		write_back_word(emu, srcval & ~mask);
		break;
	case 7:
		write_back_word(emu, srcval ^ mask);
		break;
	}
	CONDITIONAL_SET_FLAG(srcval & mask, F_CF);
}

static void
x86emuOp2_btX_I(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp2_32_btX_I(emu);
	else
		x86emuOp2_16_btX_I(emu);
}


static void
x86emuOp2_btc_R(struct x86emu *emu)
{
	common_bitstring(emu, 3);
}


static void
x86emuOp2_bsf(struct x86emu *emu)
{
	common_bitsearch(emu, +1);
}


static void
x86emuOp2_bsr(struct x86emu *emu)
{
	common_bitsearch(emu, -1);
}


static void
x86emuOp2_32_movsx_byte_R_RM(struct x86emu *emu)
{
	uint32 *destreg;

	destreg = decode_rh_long_register(emu);
	*destreg = (int32)(int8)decode_and_fetch_byte(emu);
}

static void
x86emuOp2_16_movsx_byte_R_RM(struct x86emu *emu)
{
	uint16 *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_word_register(emu);
	*destreg = (int16)(int8)decode_and_fetch_byte(emu);
}

static void
x86emuOp2_movsx_byte_R_RM(struct x86emu *emu)
{
	if (emu->x86.mode & SYSMODE_PREFIX_DATA)
		x86emuOp2_32_movsx_byte_R_RM(emu);
	else
		x86emuOp2_16_movsx_byte_R_RM(emu);
}


static void
x86emuOp2_movsx_word_R_RM(struct x86emu *emu)
{
	uint32 *destreg;

	fetch_decode_modrm(emu);
	destreg = decode_rh_long_register(emu);
	*destreg = (int32)(int16)decode_and_fetch_word(emu);
}

static void
x86emu_exec_two_byte(struct x86emu * emu)
{
	uint8 op2;

	op2 = fetch_byte_imm(emu);

	switch (op2) {
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	

	case 0x31:
		x86emuOp2_rdtsc(emu);
		break;

	case 0x80:
		common_jmp_long(emu, ACCESS_FLAG(F_OF));
		break;
	case 0x81:
		common_jmp_long(emu, !ACCESS_FLAG(F_OF));
		break;
	case 0x82:
		common_jmp_long(emu, ACCESS_FLAG(F_CF));
		break;
	case 0x83:
		common_jmp_long(emu, !ACCESS_FLAG(F_CF));
		break;
	case 0x84:
		common_jmp_long(emu, ACCESS_FLAG(F_ZF));
		break;
	case 0x85:
		common_jmp_long(emu, !ACCESS_FLAG(F_ZF));
		break;
	case 0x86:
		common_jmp_long(emu, ACCESS_FLAG(F_CF) || ACCESS_FLAG(F_ZF));
		break;
	case 0x87:
		common_jmp_long(emu, !(ACCESS_FLAG(F_CF) || ACCESS_FLAG(F_ZF)));
		break;
	case 0x88:
		common_jmp_long(emu, ACCESS_FLAG(F_SF));
		break;
	case 0x89:
		common_jmp_long(emu, !ACCESS_FLAG(F_SF));
		break;
	case 0x8a:
		common_jmp_long(emu, ACCESS_FLAG(F_PF));
		break;
	case 0x8b:
		common_jmp_long(emu, !ACCESS_FLAG(F_PF));
		break;
	case 0x8c:
		common_jmp_long(emu, xorl(ACCESS_FLAG(F_SF),
		    ACCESS_FLAG(F_OF)));
		break;
	case 0x8d:
		common_jmp_long(emu, !(xorl(ACCESS_FLAG(F_SF),
		    ACCESS_FLAG(F_OF))));
		break;
	case 0x8e:
		common_jmp_long(emu, (xorl(ACCESS_FLAG(F_SF), ACCESS_FLAG(F_OF))
		    || ACCESS_FLAG(F_ZF)));
		break;
	case 0x8f:
		common_jmp_long(emu, 
		    !(xorl(ACCESS_FLAG(F_SF), ACCESS_FLAG(F_OF)) ||
		    ACCESS_FLAG(F_ZF)));
		break;

	case 0x90:
		common_set_byte(emu, ACCESS_FLAG(F_OF));
		break;
	case 0x91:
		common_set_byte(emu, !ACCESS_FLAG(F_OF));
		break;
	case 0x92:
		common_set_byte(emu, ACCESS_FLAG(F_CF));
		break;
	case 0x93:
		common_set_byte(emu, !ACCESS_FLAG(F_CF));
		break;
	case 0x94:
		common_set_byte(emu, ACCESS_FLAG(F_ZF));
		break;
	case 0x95:
		common_set_byte(emu, !ACCESS_FLAG(F_ZF));
		break;
	case 0x96:
		common_set_byte(emu, ACCESS_FLAG(F_CF) || ACCESS_FLAG(F_ZF));
		break;
	case 0x97:
		common_set_byte(emu, !(ACCESS_FLAG(F_CF) || ACCESS_FLAG(F_ZF)));
		break;
	case 0x98:
		common_set_byte(emu, ACCESS_FLAG(F_SF));
		break;
	case 0x99:
		common_set_byte(emu, !ACCESS_FLAG(F_SF));
		break;
	case 0x9a:
		common_set_byte(emu, ACCESS_FLAG(F_PF));
		break;
	case 0x9b:
		common_set_byte(emu, !ACCESS_FLAG(F_PF));
		break;
	case 0x9c:
		common_set_byte(emu, xorl(ACCESS_FLAG(F_SF),
		    ACCESS_FLAG(F_OF)));
		break;
	case 0x9d:
		common_set_byte(emu, xorl(ACCESS_FLAG(F_SF),
		    ACCESS_FLAG(F_OF)));
		break;
	case 0x9e:
		common_set_byte(emu,
		    (xorl(ACCESS_FLAG(F_SF), ACCESS_FLAG(F_OF)) ||
		    ACCESS_FLAG(F_ZF)));
		break;
	case 0x9f:
		common_set_byte(emu,
		    !(xorl(ACCESS_FLAG(F_SF), ACCESS_FLAG(F_OF)) ||
		    ACCESS_FLAG(F_ZF)));
		break;

	case 0xa0:
		x86emuOp2_push_FS(emu);
		break;
	case 0xa1:
		x86emuOp2_pop_FS(emu);
		break;
	case 0xa2:
		x86emuOp2_cpuid(emu);
		break;
	case 0xa3:
		x86emuOp2_bt_R(emu);
		break;
	case 0xa4:
		x86emuOp2_shld_IMM(emu);
		break;
	case 0xa5:
		x86emuOp2_shld_CL(emu);
		break;
	case 0xa8:
		x86emuOp2_push_GS(emu);
		break;
	case 0xa9:
		x86emuOp2_pop_GS(emu);
		break;
	case 0xab:
		x86emuOp2_bts_R(emu);
		break;
	case 0xac:
		x86emuOp2_shrd_IMM(emu);
		break;
	case 0xad:
		x86emuOp2_shrd_CL(emu);
		break;
	case 0xaf:
		x86emuOp2_imul_R_RM(emu);
		break;

	
	
	case 0xb2:
		x86emuOp2_lss_R_IMM(emu);
		break;
	case 0xb3:
		x86emuOp2_btr_R(emu);
		break;
	case 0xb4:
		x86emuOp2_lfs_R_IMM(emu);
		break;
	case 0xb5:
		x86emuOp2_lgs_R_IMM(emu);
		break;
	case 0xb6:
		x86emuOp2_movzx_byte_R_RM(emu);
		break;
	case 0xb7:
		x86emuOp2_movzx_word_R_RM(emu);
		break;
	case 0xba:
		x86emuOp2_btX_I(emu);
		break;
	case 0xbb:
		x86emuOp2_btc_R(emu);
		break;
	case 0xbc:
		x86emuOp2_bsf(emu);
		break;
	case 0xbd:
		x86emuOp2_bsr(emu);
		break;
	case 0xbe:
		x86emuOp2_movsx_byte_R_RM(emu);
		break;
	case 0xbf:
		x86emuOp2_movsx_word_R_RM(emu);
		break;

	
	
	
	
	
	
	
	
	
	

	default:
		x86emu_halt_sys(emu);
		break;
	}
}





static uint32 x86emu_parity_tab[8] =
{
	0x96696996,
	0x69969669,
	0x69969669,
	0x96696996,
	0x69969669,
	0x96696996,
	0x96696996,
	0x69969669,
};
#define PARITY(x)   (((x86emu_parity_tab[(x) / 32] >> ((x) % 32)) & 1) == 0)
#define XOR2(x) 	(((x) ^ ((x)>>1)) & 0x1)



static uint16 
aaa_word(struct x86emu *emu, uint16 d)
{
	uint16 res;
	if ((d & 0xf) > 0x9 || ACCESS_FLAG(F_AF)) {
		d += 0x6;
		d += 0x100;
		SET_FLAG(F_AF);
		SET_FLAG(F_CF);
	} else {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_AF);
	}
	res = (uint16) (d & 0xFF0F);
	CLEAR_FLAG(F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	return res;
}


static uint16 
aas_word(struct x86emu *emu, uint16 d)
{
	uint16 res;
	if ((d & 0xf) > 0x9 || ACCESS_FLAG(F_AF)) {
		d -= 0x6;
		d -= 0x100;
		SET_FLAG(F_AF);
		SET_FLAG(F_CF);
	} else {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_AF);
	}
	res = (uint16) (d & 0xFF0F);
	CLEAR_FLAG(F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	return res;
}


static uint16 
aad_word(struct x86emu *emu, uint16 d)
{
	uint16 l;
	uint8 hb, lb;

	hb = (uint8) ((d >> 8) & 0xff);
	lb = (uint8) ((d & 0xff));
	l = (uint16) ((lb + 10 * hb) & 0xFF);

	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(l & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(l == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(l & 0xff), F_PF);
	return l;
}


static uint16 
aam_word(struct x86emu *emu, uint8 d)
{
	uint16 h, l;

	h = (uint16) (d / 10);
	l = (uint16) (d % 10);
	l |= (uint16) (h << 8);

	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(l & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(l == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(l & 0xff), F_PF);
	return l;
}


static uint8 
adc_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res;	
	uint32 cc;

	if (ACCESS_FLAG(F_CF))
		res = 1 + d + s;
	else
		res = d + s;

	CONDITIONAL_SET_FLAG(res & 0x100, F_CF);
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (s & d) | ((~res) & (s | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return (uint8) res;
}


static uint16 
adc_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint32 res;	
	uint32 cc;

	if (ACCESS_FLAG(F_CF))
		res = 1 + d + s;
	else
		res = d + s;

	CONDITIONAL_SET_FLAG(res & 0x10000, F_CF);
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (s & d) | ((~res) & (s | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return (uint16) res;
}


static uint32 
adc_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 lo;	
	uint32 hi;
	uint32 res;
	uint32 cc;

	if (ACCESS_FLAG(F_CF)) {
		lo = 1 + (d & 0xFFFF) + (s & 0xFFFF);
		res = 1 + d + s;
	} else {
		lo = (d & 0xFFFF) + (s & 0xFFFF);
		res = d + s;
	}
	hi = (lo >> 16) + (d >> 16) + (s >> 16);

	CONDITIONAL_SET_FLAG(hi & 0x10000, F_CF);
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (s & d) | ((~res) & (s | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return res;
}


static uint8 
add_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res;	
	uint32 cc;

	res = d + s;
	CONDITIONAL_SET_FLAG(res & 0x100, F_CF);
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (s & d) | ((~res) & (s | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return (uint8) res;
}


static uint16 
add_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint32 res;	
	uint32 cc;

	res = d + s;
	CONDITIONAL_SET_FLAG(res & 0x10000, F_CF);
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (s & d) | ((~res) & (s | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return (uint16) res;
}


static uint32 
add_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 lo;	
	uint32 hi;
	uint32 res;
	uint32 cc;

	lo = (d & 0xFFFF) + (s & 0xFFFF);
	res = d + s;
	hi = (lo >> 16) + (d >> 16) + (s >> 16);

	CONDITIONAL_SET_FLAG(hi & 0x10000, F_CF);
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (s & d) | ((~res) & (s | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);

	return res;
}


static uint8 
and_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint8 res;	

	res = d & s;

	
	CLEAR_FLAG(F_OF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res), F_PF);
	return res;
}


static uint16 
and_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint16 res;	

	res = d & s;

	
	CLEAR_FLAG(F_OF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	return res;
}


static uint32 
and_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 res;	

	res = d & s;

	
	CLEAR_FLAG(F_OF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	return res;
}


static uint8 
cmp_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res;	
	uint32 bc;

	res = d - s;
	CLEAR_FLAG(F_CF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x80, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return d;
}

static void 
cmp_byte_no_return(struct x86emu *emu, uint8 d, uint8 s)
{
	cmp_byte(emu, d, s);
}


static uint16 
cmp_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint32 res;	
	uint32 bc;

	res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x8000, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return d;
}

static void 
cmp_word_no_return(struct x86emu *emu, uint16 d, uint16 s)
{
	cmp_word(emu, d, s);
}


static uint32 
cmp_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 res;	
	uint32 bc;

	res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x80000000, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return d;
}

static void 
cmp_long_no_return(struct x86emu *emu, uint32 d, uint32 s)
{
	cmp_long(emu, d, s);
}


static uint8 
daa_byte(struct x86emu *emu, uint8 d)
{
	uint32 res = d;
	if ((d & 0xf) > 9 || ACCESS_FLAG(F_AF)) {
		res += 6;
		SET_FLAG(F_AF);
	}
	if (res > 0x9F || ACCESS_FLAG(F_CF)) {
		res += 0x60;
		SET_FLAG(F_CF);
	}
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xFF) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	return (uint8) res;
}


static uint8 
das_byte(struct x86emu *emu, uint8 d)
{
	if ((d & 0xf) > 9 || ACCESS_FLAG(F_AF)) {
		d -= 6;
		SET_FLAG(F_AF);
	}
	if (d > 0x9F || ACCESS_FLAG(F_CF)) {
		d -= 0x60;
		SET_FLAG(F_CF);
	}
	CONDITIONAL_SET_FLAG(d & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(d == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(d & 0xff), F_PF);
	return d;
}


static uint8 
dec_byte(struct x86emu *emu, uint8 d)
{
	uint32 res;	
	uint32 bc;

	res = d - 1;
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	
	bc = (res & (~d | 1)) | (~d & 1);
	
	CONDITIONAL_SET_FLAG(XOR2(bc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return (uint8) res;
}


static uint16 
dec_word(struct x86emu *emu, uint16 d)
{
	uint32 res;	
	uint32 bc;

	res = d - 1;
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	
	bc = (res & (~d | 1)) | (~d & 1);
	
	CONDITIONAL_SET_FLAG(XOR2(bc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return (uint16) res;
}


static uint32 
dec_long(struct x86emu *emu, uint32 d)
{
	uint32 res;	
	uint32 bc;

	res = d - 1;

	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | 1)) | (~d & 1);
	
	CONDITIONAL_SET_FLAG(XOR2(bc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return res;
}


static uint8 
inc_byte(struct x86emu *emu, uint8 d)
{
	uint32 res;	
	uint32 cc;

	res = d + 1;
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = ((1 & d) | (~res)) & (1 | d);
	CONDITIONAL_SET_FLAG(XOR2(cc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return (uint8) res;
}


static uint16 
inc_word(struct x86emu *emu, uint16 d)
{
	uint32 res;	
	uint32 cc;

	res = d + 1;
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (1 & d) | ((~res) & (1 | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return (uint16) res;
}


static uint32 
inc_long(struct x86emu *emu, uint32 d)
{
	uint32 res;	
	uint32 cc;

	res = d + 1;
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	cc = (1 & d) | ((~res) & (1 | d));
	CONDITIONAL_SET_FLAG(XOR2(cc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(cc & 0x8, F_AF);
	return res;
}


static uint8 
or_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint8 res;	

	res = d | s;
	CLEAR_FLAG(F_OF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res), F_PF);
	return res;
}


static uint16 
or_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint16 res;	

	res = d | s;
	
	CLEAR_FLAG(F_OF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	return res;
}


static uint32 
or_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 res;	

	res = d | s;

	
	CLEAR_FLAG(F_OF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	return res;
}


static uint8 
neg_byte(struct x86emu *emu, uint8 s)
{
	uint8 res;
	uint8 bc;

	CONDITIONAL_SET_FLAG(s != 0, F_CF);
	res = (uint8) - s;
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res), F_PF);
	
	bc = res | s;
	CONDITIONAL_SET_FLAG(XOR2(bc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return res;
}


static uint16 
neg_word(struct x86emu *emu, uint16 s)
{
	uint16 res;
	uint16 bc;

	CONDITIONAL_SET_FLAG(s != 0, F_CF);
	res = (uint16) - s;
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = res | s;
	CONDITIONAL_SET_FLAG(XOR2(bc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return res;
}


static uint32 
neg_long(struct x86emu *emu, uint32 s)
{
	uint32 res;
	uint32 bc;

	CONDITIONAL_SET_FLAG(s != 0, F_CF);
	res = (uint32) - s;
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = res | s;
	CONDITIONAL_SET_FLAG(XOR2(bc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return res;
}


static uint8 
rcl_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res, cnt, mask, cf;
	
	res = d;
	if ((cnt = s % 9) != 0) {
		
		
		cf = (d >> (8 - cnt)) & 0x1;

		
		res = (d << cnt) & 0xff;

		
		mask = (1 << (cnt - 1)) - 1;
		res |= (d >> (9 - cnt)) & mask;

		
		if (ACCESS_FLAG(F_CF)) {	
			
			res |= 1 << (cnt - 1);
		}
		
		CONDITIONAL_SET_FLAG(cf, F_CF);
		
		
		CONDITIONAL_SET_FLAG(cnt == 1 && XOR2(cf + ((res >> 6) & 0x2)),
		    F_OF);

	}
	return (uint8) res;
}


static uint16 
rcl_word(struct x86emu *emu, uint16 d, uint8 s)
{
	uint32 res, cnt, mask, cf;

	res = d;
	if ((cnt = s % 17) != 0) {
		cf = (d >> (16 - cnt)) & 0x1;
		res = (d << cnt) & 0xffff;
		mask = (1 << (cnt - 1)) - 1;
		res |= (d >> (17 - cnt)) & mask;
		if (ACCESS_FLAG(F_CF)) {
			res |= 1 << (cnt - 1);
		}
		CONDITIONAL_SET_FLAG(cf, F_CF);
		CONDITIONAL_SET_FLAG(cnt == 1 && XOR2(cf + ((res >> 14) & 0x2)),
		    F_OF);
	}
	return (uint16) res;
}


static uint32 
rcl_long(struct x86emu *emu, uint32 d, uint8 s)
{
	uint32 res, cnt, mask, cf;

	res = d;
	if ((cnt = s % 33) != 0) {
		cf = (d >> (32 - cnt)) & 0x1;
		res = (d << cnt) & 0xffffffff;
		mask = (1 << (cnt - 1)) - 1;
		res |= (d >> (33 - cnt)) & mask;
		if (ACCESS_FLAG(F_CF)) {	
			res |= 1 << (cnt - 1);
		}
		CONDITIONAL_SET_FLAG(cf, F_CF);
		CONDITIONAL_SET_FLAG(cnt == 1 && XOR2(cf + ((res >> 30) & 0x2)),
		    F_OF);
	}
	return res;
}


static uint8 
rcr_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res, cnt;
	uint32 mask, cf, ocf = 0;

	
	
	res = d;
	if ((cnt = s % 9) != 0) {
		
		
		if (cnt == 1) {
			cf = d & 0x1;
			
			ocf = ACCESS_FLAG(F_CF) != 0;
		} else
			cf = (d >> (cnt - 1)) & 0x1;

		
		

		mask = (1 << (8 - cnt)) - 1;
		res = (d >> cnt) & mask;

		
		
		
		res |= (d << (9 - cnt));

		
		if (ACCESS_FLAG(F_CF)) {	
			
			res |= 1 << (8 - cnt);
		}
		
		CONDITIONAL_SET_FLAG(cf, F_CF);
		
		
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(ocf + ((d >> 6) & 0x2)),
			    F_OF);
		}
	}
	return (uint8) res;
}


static uint16 
rcr_word(struct x86emu *emu, uint16 d, uint8 s)
{
	uint32 res, cnt;
	uint32 mask, cf, ocf = 0;

	
	res = d;
	if ((cnt = s % 17) != 0) {
		if (cnt == 1) {
			cf = d & 0x1;
			ocf = ACCESS_FLAG(F_CF) != 0;
		} else
			cf = (d >> (cnt - 1)) & 0x1;
		mask = (1 << (16 - cnt)) - 1;
		res = (d >> cnt) & mask;
		res |= (d << (17 - cnt));
		if (ACCESS_FLAG(F_CF)) {
			res |= 1 << (16 - cnt);
		}
		CONDITIONAL_SET_FLAG(cf, F_CF);
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(ocf + ((d >> 14) & 0x2)),
			    F_OF);
		}
	}
	return (uint16) res;
}


static uint32 
rcr_long(struct x86emu *emu, uint32 d, uint8 s)
{
	uint32 res, cnt;
	uint32 mask, cf, ocf = 0;

	
	res = d;
	if ((cnt = s % 33) != 0) {
		if (cnt == 1) {
			cf = d & 0x1;
			ocf = ACCESS_FLAG(F_CF) != 0;
		} else
			cf = (d >> (cnt - 1)) & 0x1;
		mask = (1 << (32 - cnt)) - 1;
		res = (d >> cnt) & mask;
		if (cnt != 1)
			res |= (d << (33 - cnt));
		if (ACCESS_FLAG(F_CF)) {	
			res |= 1 << (32 - cnt);
		}
		CONDITIONAL_SET_FLAG(cf, F_CF);
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(ocf + ((d >> 30) & 0x2)),
			    F_OF);
		}
	}
	return res;
}


static uint8 
rol_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res, cnt, mask;

	
	
	res = d;
	if ((cnt = s % 8) != 0) {
		
		res = (d << cnt);

		
		mask = (1 << cnt) - 1;
		res |= (d >> (8 - cnt)) & mask;

		
		CONDITIONAL_SET_FLAG(res & 0x1, F_CF);
		
		CONDITIONAL_SET_FLAG(s == 1 &&
		    XOR2((res & 0x1) + ((res >> 6) & 0x2)),
		    F_OF);
	} if (s != 0) {
		
		CONDITIONAL_SET_FLAG(res & 0x1, F_CF);
	}
	return (uint8) res;
}


static uint16 
rol_word(struct x86emu *emu, uint16 d, uint8 s)
{
	uint32 res, cnt, mask;

	res = d;
	if ((cnt = s % 16) != 0) {
		res = (d << cnt);
		mask = (1 << cnt) - 1;
		res |= (d >> (16 - cnt)) & mask;
		CONDITIONAL_SET_FLAG(res & 0x1, F_CF);
		CONDITIONAL_SET_FLAG(s == 1 &&
		    XOR2((res & 0x1) + ((res >> 14) & 0x2)),
		    F_OF);
	} if (s != 0) {
		
		CONDITIONAL_SET_FLAG(res & 0x1, F_CF);
	}
	return (uint16) res;
}


static uint32 
rol_long(struct x86emu *emu, uint32 d, uint8 s)
{
	uint32 res, cnt, mask;

	res = d;
	if ((cnt = s % 32) != 0) {
		res = (d << cnt);
		mask = (1 << cnt) - 1;
		res |= (d >> (32 - cnt)) & mask;
		CONDITIONAL_SET_FLAG(res & 0x1, F_CF);
		CONDITIONAL_SET_FLAG(s == 1 &&
		    XOR2((res & 0x1) + ((res >> 30) & 0x2)),
		    F_OF);
	} if (s != 0) {
		
		CONDITIONAL_SET_FLAG(res & 0x1, F_CF);
	}
	return res;
}


static uint8 
ror_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res, cnt, mask;

	
	
	res = d;
	if ((cnt = s % 8) != 0) {	
		
		res = (d << (8 - cnt));

		
		mask = (1 << (8 - cnt)) - 1;
		res |= (d >> (cnt)) & mask;

		
		CONDITIONAL_SET_FLAG(res & 0x80, F_CF);
		
		CONDITIONAL_SET_FLAG(s == 1 && XOR2(res >> 6), F_OF);
	} else if (s != 0) {
		
		CONDITIONAL_SET_FLAG(res & 0x80, F_CF);
	}
	return (uint8) res;
}


static uint16 
ror_word(struct x86emu *emu, uint16 d, uint8 s)
{
	uint32 res, cnt, mask;

	res = d;
	if ((cnt = s % 16) != 0) {
		res = (d << (16 - cnt));
		mask = (1 << (16 - cnt)) - 1;
		res |= (d >> (cnt)) & mask;
		CONDITIONAL_SET_FLAG(res & 0x8000, F_CF);
		CONDITIONAL_SET_FLAG(s == 1 && XOR2(res >> 14), F_OF);
	} else if (s != 0) {
		
		CONDITIONAL_SET_FLAG(res & 0x8000, F_CF);
	}
	return (uint16) res;
}


static uint32 
ror_long(struct x86emu *emu, uint32 d, uint8 s)
{
	uint32 res, cnt, mask;

	res = d;
	if ((cnt = s % 32) != 0) {
		res = (d << (32 - cnt));
		mask = (1 << (32 - cnt)) - 1;
		res |= (d >> (cnt)) & mask;
		CONDITIONAL_SET_FLAG(res & 0x80000000, F_CF);
		CONDITIONAL_SET_FLAG(s == 1 && XOR2(res >> 30), F_OF);
	} else if (s != 0) {
		
		CONDITIONAL_SET_FLAG(res & 0x80000000, F_CF);
	}
	return res;
}


static uint8 
shl_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 8) {
		cnt = s % 8;

		
		if (cnt > 0) {
			res = d << cnt;
			cf = d & (1 << (8 - cnt));
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = (uint8) d;
		}

		if (cnt == 1) {
			
			CONDITIONAL_SET_FLAG(
			    (((res & 0x80) == 0x80) ^
				(ACCESS_FLAG(F_CF) != 0)),
			
			    F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CONDITIONAL_SET_FLAG((d << (s - 1)) & 0x80, F_CF);
		CLEAR_FLAG(F_OF);
		CLEAR_FLAG(F_SF);
		SET_FLAG(F_PF);
		SET_FLAG(F_ZF);
	}
	return (uint8) res;
}


static uint16 
shl_word(struct x86emu *emu, uint16 d, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 16) {
		cnt = s % 16;
		if (cnt > 0) {
			res = d << cnt;
			cf = d & (1 << (16 - cnt));
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = (uint16) d;
		}

		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(
			    (((res & 0x8000) == 0x8000) ^
				(ACCESS_FLAG(F_CF) != 0)),
			    F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CONDITIONAL_SET_FLAG((d << (s - 1)) & 0x8000, F_CF);
		CLEAR_FLAG(F_OF);
		CLEAR_FLAG(F_SF);
		SET_FLAG(F_PF);
		SET_FLAG(F_ZF);
	}
	return (uint16) res;
}


static uint32 
shl_long(struct x86emu *emu, uint32 d, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 32) {
		cnt = s % 32;
		if (cnt > 0) {
			res = d << cnt;
			cf = d & (1 << (32 - cnt));
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = d;
		}
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG((((res & 0x80000000) == 0x80000000)
			    ^ (ACCESS_FLAG(F_CF) != 0)), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CONDITIONAL_SET_FLAG((d << (s - 1)) & 0x80000000, F_CF);
		CLEAR_FLAG(F_OF);
		CLEAR_FLAG(F_SF);
		SET_FLAG(F_PF);
		SET_FLAG(F_ZF);
	}
	return res;
}


static uint8 
shr_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 8) {
		cnt = s % 8;
		if (cnt > 0) {
			cf = d & (1 << (cnt - 1));
			res = d >> cnt;
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = (uint8) d;
		}

		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(res >> 6), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CONDITIONAL_SET_FLAG((d >> (s - 1)) & 0x1, F_CF);
		CLEAR_FLAG(F_OF);
		CLEAR_FLAG(F_SF);
		SET_FLAG(F_PF);
		SET_FLAG(F_ZF);
	}
	return (uint8) res;
}


static uint16 
shr_word(struct x86emu *emu, uint16 d, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 16) {
		cnt = s % 16;
		if (cnt > 0) {
			cf = d & (1 << (cnt - 1));
			res = d >> cnt;
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = d;
		}

		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(res >> 14), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
		SET_FLAG(F_ZF);
		CLEAR_FLAG(F_SF);
		CLEAR_FLAG(F_PF);
	}
	return (uint16) res;
}


static uint32 
shr_long(struct x86emu *emu, uint32 d, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 32) {
		cnt = s % 32;
		if (cnt > 0) {
			cf = d & (1 << (cnt - 1));
			res = d >> cnt;
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = d;
		}
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(res >> 30), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
		SET_FLAG(F_ZF);
		CLEAR_FLAG(F_SF);
		CLEAR_FLAG(F_PF);
	}
	return res;
}


static uint8 
sar_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 cnt, res, cf, mask, sf;

	res = d;
	sf = d & 0x80;
	cnt = s % 8;
	if (cnt > 0 && cnt < 8) {
		mask = (1 << (8 - cnt)) - 1;
		cf = d & (1 << (cnt - 1));
		res = (d >> cnt) & mask;
		CONDITIONAL_SET_FLAG(cf, F_CF);
		if (sf) {
			res |= ~mask;
		}
		CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
		CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	} else if (cnt >= 8) {
		if (sf) {
			res = 0xff;
			SET_FLAG(F_CF);
			CLEAR_FLAG(F_ZF);
			SET_FLAG(F_SF);
			SET_FLAG(F_PF);
		} else {
			res = 0;
			CLEAR_FLAG(F_CF);
			SET_FLAG(F_ZF);
			CLEAR_FLAG(F_SF);
			CLEAR_FLAG(F_PF);
		}
	}
	return (uint8) res;
}


static uint16 
sar_word(struct x86emu *emu, uint16 d, uint8 s)
{
	uint32 cnt, res, cf, mask, sf;

	sf = d & 0x8000;
	cnt = s % 16;
	res = d;
	if (cnt > 0 && cnt < 16) {
		mask = (1 << (16 - cnt)) - 1;
		cf = d & (1 << (cnt - 1));
		res = (d >> cnt) & mask;
		CONDITIONAL_SET_FLAG(cf, F_CF);
		if (sf) {
			res |= ~mask;
		}
		CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
		CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
		CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	} else if (cnt >= 16) {
		if (sf) {
			res = 0xffff;
			SET_FLAG(F_CF);
			CLEAR_FLAG(F_ZF);
			SET_FLAG(F_SF);
			SET_FLAG(F_PF);
		} else {
			res = 0;
			CLEAR_FLAG(F_CF);
			SET_FLAG(F_ZF);
			CLEAR_FLAG(F_SF);
			CLEAR_FLAG(F_PF);
		}
	}
	return (uint16) res;
}


static uint32 
sar_long(struct x86emu *emu, uint32 d, uint8 s)
{
	uint32 cnt, res, cf, mask, sf;

	sf = d & 0x80000000;
	cnt = s % 32;
	res = d;
	if (cnt > 0 && cnt < 32) {
		mask = (1 << (32 - cnt)) - 1;
		cf = d & (1 << (cnt - 1));
		res = (d >> cnt) & mask;
		CONDITIONAL_SET_FLAG(cf, F_CF);
		if (sf) {
			res |= ~mask;
		}
		CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
		CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
		CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	} else if (cnt >= 32) {
		if (sf) {
			res = 0xffffffff;
			SET_FLAG(F_CF);
			CLEAR_FLAG(F_ZF);
			SET_FLAG(F_SF);
			SET_FLAG(F_PF);
		} else {
			res = 0;
			CLEAR_FLAG(F_CF);
			SET_FLAG(F_ZF);
			CLEAR_FLAG(F_SF);
			CLEAR_FLAG(F_PF);
		}
	}
	return res;
}


static uint16 
shld_word(struct x86emu *emu, uint16 d, uint16 fill, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 16) {
		cnt = s % 16;
		if (cnt > 0) {
			res = (d << cnt) | (fill >> (16 - cnt));
			cf = d & (1 << (16 - cnt));
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = d;
		}
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG((((res & 0x8000) == 0x8000) ^
				(ACCESS_FLAG(F_CF) != 0)), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CONDITIONAL_SET_FLAG((d << (s - 1)) & 0x8000, F_CF);
		CLEAR_FLAG(F_OF);
		CLEAR_FLAG(F_SF);
		SET_FLAG(F_PF);
		SET_FLAG(F_ZF);
	}
	return (uint16) res;
}


static uint32 
shld_long(struct x86emu *emu, uint32 d, uint32 fill, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 32) {
		cnt = s % 32;
		if (cnt > 0) {
			res = (d << cnt) | (fill >> (32 - cnt));
			cf = d & (1 << (32 - cnt));
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = d;
		}
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG((((res & 0x80000000) == 0x80000000)
			    ^ (ACCESS_FLAG(F_CF) != 0)), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CONDITIONAL_SET_FLAG((d << (s - 1)) & 0x80000000, F_CF);
		CLEAR_FLAG(F_OF);
		CLEAR_FLAG(F_SF);
		SET_FLAG(F_PF);
		SET_FLAG(F_ZF);
	}
	return res;
}


static uint16 
shrd_word(struct x86emu *emu, uint16 d, uint16 fill, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 16) {
		cnt = s % 16;
		if (cnt > 0) {
			cf = d & (1 << (cnt - 1));
			res = (d >> cnt) | (fill << (16 - cnt));
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = d;
		}

		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(res >> 14), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
		SET_FLAG(F_ZF);
		CLEAR_FLAG(F_SF);
		CLEAR_FLAG(F_PF);
	}
	return (uint16) res;
}


static uint32 
shrd_long(struct x86emu *emu, uint32 d, uint32 fill, uint8 s)
{
	uint32 cnt, res, cf;

	if (s < 32) {
		cnt = s % 32;
		if (cnt > 0) {
			cf = d & (1 << (cnt - 1));
			res = (d >> cnt) | (fill << (32 - cnt));
			CONDITIONAL_SET_FLAG(cf, F_CF);
			CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
			CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
			CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
		} else {
			res = d;
		}
		if (cnt == 1) {
			CONDITIONAL_SET_FLAG(XOR2(res >> 30), F_OF);
		} else {
			CLEAR_FLAG(F_OF);
		}
	} else {
		res = 0;
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
		SET_FLAG(F_ZF);
		CLEAR_FLAG(F_SF);
		CLEAR_FLAG(F_PF);
	}
	return res;
}


static uint8 
sbb_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res;	
	uint32 bc;

	if (ACCESS_FLAG(F_CF))
		res = d - s - 1;
	else
		res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x80, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return (uint8) res;
}


static uint16 
sbb_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint32 res;	
	uint32 bc;

	if (ACCESS_FLAG(F_CF))
		res = d - s - 1;
	else
		res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x8000, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return (uint16) res;
}


static uint32 
sbb_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 res;	
	uint32 bc;

	if (ACCESS_FLAG(F_CF))
		res = d - s - 1;
	else
		res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x80000000, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return res;
}


static uint8 
sub_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res;	
	uint32 bc;

	res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x80, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 6), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return (uint8) res;
}


static uint16 
sub_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint32 res;	
	uint32 bc;

	res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x8000, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 14), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return (uint16) res;
}


static uint32 
sub_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 res;	
	uint32 bc;

	res = d - s;
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG((res & 0xffffffff) == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);

	
	bc = (res & (~d | s)) | (~d & s);
	CONDITIONAL_SET_FLAG(bc & 0x80000000, F_CF);
	CONDITIONAL_SET_FLAG(XOR2(bc >> 30), F_OF);
	CONDITIONAL_SET_FLAG(bc & 0x8, F_AF);
	return res;
}


static void 
test_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint32 res;	

	res = d & s;

	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	
	CLEAR_FLAG(F_CF);
}


static void 
test_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint32 res;	

	res = d & s;

	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	
	CLEAR_FLAG(F_CF);
}


static void 
test_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 res;	

	res = d & s;

	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	
	CLEAR_FLAG(F_CF);
}


static uint8 
xor_byte(struct x86emu *emu, uint8 d, uint8 s)
{
	uint8 res;	

	res = d ^ s;
	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(res & 0x80, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res), F_PF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	return res;
}


static uint16 
xor_word(struct x86emu *emu, uint16 d, uint16 s)
{
	uint16 res;	

	res = d ^ s;
	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(res & 0x8000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	return res;
}


static uint32 
xor_long(struct x86emu *emu, uint32 d, uint32 s)
{
	uint32 res;	

	res = d ^ s;
	CLEAR_FLAG(F_OF);
	CONDITIONAL_SET_FLAG(res & 0x80000000, F_SF);
	CONDITIONAL_SET_FLAG(res == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(res & 0xff), F_PF);
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	return res;
}


static void 
imul_byte(struct x86emu *emu, uint8 s)
{
	int16 res = (int16) ((int8) emu->x86.R_AL * (int8) s);

	emu->x86.R_AX = res;
	if (((emu->x86.R_AL & 0x80) == 0 && emu->x86.R_AH == 0x00) ||
	    ((emu->x86.R_AL & 0x80) != 0 && emu->x86.R_AH == 0xFF)) {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	} else {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	}
}


static void 
imul_word(struct x86emu *emu, uint16 s)
{
	int32 res = (int16) emu->x86.R_AX * (int16) s;

	emu->x86.R_AX = (uint16) res;
	emu->x86.R_DX = (uint16) (res >> 16);
	if (((emu->x86.R_AX & 0x8000) == 0 && emu->x86.R_DX == 0x00) ||
	    ((emu->x86.R_AX & 0x8000) != 0 && emu->x86.R_DX == 0xFF)) {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	} else {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	}
}


static void 
imul_long(struct x86emu *emu, uint32 s)
{
	int64 res;
	
	res = (int64)(int32)emu->x86.R_EAX * (int32)s;
	emu->x86.R_EAX = (uint32)res;
	emu->x86.R_EDX = ((uint64)res) >> 32;
	if (((emu->x86.R_EAX & 0x80000000) == 0 && emu->x86.R_EDX == 0x00) ||
	    ((emu->x86.R_EAX & 0x80000000) != 0 && emu->x86.R_EDX == 0xFF)) {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	} else {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	}
}


static void 
mul_byte(struct x86emu *emu, uint8 s)
{
	uint16 res = (uint16) (emu->x86.R_AL * s);

	emu->x86.R_AX = res;
	if (emu->x86.R_AH == 0) {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	} else {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	}
}


static void 
mul_word(struct x86emu *emu, uint16 s)
{
	uint32 res = emu->x86.R_AX * s;

	emu->x86.R_AX = (uint16) res;
	emu->x86.R_DX = (uint16) (res >> 16);
	if (emu->x86.R_DX == 0) {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	} else {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	}
}


static void 
mul_long(struct x86emu *emu, uint32 s)
{
	uint64 res = (uint64) emu->x86.R_EAX * s;

	emu->x86.R_EAX = (uint32) res;
	emu->x86.R_EDX = (uint32) (res >> 32);

	if (emu->x86.R_EDX == 0) {
		CLEAR_FLAG(F_CF);
		CLEAR_FLAG(F_OF);
	} else {
		SET_FLAG(F_CF);
		SET_FLAG(F_OF);
	}
}


static void 
idiv_byte(struct x86emu *emu, uint8 s)
{
	int32 dvd, div, mod;

	dvd = (int16) emu->x86.R_AX;
	if (s == 0) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	div = dvd / (int8) s;
	mod = dvd % (int8) s;
	if (div > 0x7f || div < -0x7f) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	emu->x86.R_AL = (int8) div;
	emu->x86.R_AH = (int8) mod;
}


static void 
idiv_word(struct x86emu *emu, uint16 s)
{
	int32 dvd, div, mod;

	dvd = (((int32) emu->x86.R_DX) << 16) | emu->x86.R_AX;
	if (s == 0) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	div = dvd / (int16) s;
	mod = dvd % (int16) s;
	if (div > 0x7fff || div < -0x7fff) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_SF);
	CONDITIONAL_SET_FLAG(div == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(mod & 0xff), F_PF);

	emu->x86.R_AX = (uint16) div;
	emu->x86.R_DX = (uint16) mod;
}


static void 
idiv_long(struct x86emu *emu, uint32 s)
{
	int64 dvd, div, mod;

	dvd = (((int64) emu->x86.R_EDX) << 32) | emu->x86.R_EAX;
	if (s == 0) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	div = dvd / (int32) s;
	mod = dvd % (int32) s;
	if (div > 0x7fffffff || div < -0x7fffffff) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CLEAR_FLAG(F_SF);
	SET_FLAG(F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(mod & 0xff), F_PF);

	emu->x86.R_EAX = (uint32) div;
	emu->x86.R_EDX = (uint32) mod;
}


static void 
div_byte(struct x86emu *emu, uint8 s)
{
	uint32 dvd, div, mod;

	dvd = emu->x86.R_AX;
	if (s == 0) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	div = dvd / (uint8) s;
	mod = dvd % (uint8) s;
	if (div > 0xff) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	emu->x86.R_AL = (uint8) div;
	emu->x86.R_AH = (uint8) mod;
}


static void 
div_word(struct x86emu *emu, uint16 s)
{
	uint32 dvd, div, mod;

	dvd = (((uint32) emu->x86.R_DX) << 16) | emu->x86.R_AX;
	if (s == 0) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	div = dvd / (uint16) s;
	mod = dvd % (uint16) s;
	if (div > 0xffff) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_SF);
	CONDITIONAL_SET_FLAG(div == 0, F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(mod & 0xff), F_PF);

	emu->x86.R_AX = (uint16) div;
	emu->x86.R_DX = (uint16) mod;
}


static void 
div_long(struct x86emu *emu, uint32 s)
{
	uint64 dvd, div, mod;

	dvd = (((uint64) emu->x86.R_EDX) << 32) | emu->x86.R_EAX;
	if (s == 0) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	div = dvd / (uint32) s;
	mod = dvd % (uint32) s;
	if (div > 0xffffffff) {
		x86emu_intr_raise(emu, 8);
		return;
	}
	CLEAR_FLAG(F_CF);
	CLEAR_FLAG(F_AF);
	CLEAR_FLAG(F_SF);
	SET_FLAG(F_ZF);
	CONDITIONAL_SET_FLAG(PARITY(mod & 0xff), F_PF);

	emu->x86.R_EAX = (uint32) div;
	emu->x86.R_EDX = (uint32) mod;
}


static void 
ins(struct x86emu *emu, int32 size)
{
	int32 inc = size;

	if (ACCESS_FLAG(F_DF)) {
		inc = -size;
	}
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		uint32 count = ((emu->x86.mode & SYSMODE_PREFIX_DATA) ?
		    emu->x86.R_ECX : emu->x86.R_CX);
		switch (size) {
		case 1:
			while (count--) {
				store_byte(emu, emu->x86.R_ES, emu->x86.R_DI,
				    (*emu->emu_inb) (emu, emu->x86.R_DX));
				emu->x86.R_DI += inc;
			}
			break;

		case 2:
			while (count--) {
				store_word(emu, emu->x86.R_ES, emu->x86.R_DI,
				    (*emu->emu_inw) (emu, emu->x86.R_DX));
				emu->x86.R_DI += inc;
			}
			break;
		case 4:
			while (count--) {
				store_long(emu, emu->x86.R_ES, emu->x86.R_DI,
				    (*emu->emu_inl) (emu, emu->x86.R_DX));
				emu->x86.R_DI += inc;
				break;
			}
		}
		emu->x86.R_CX = 0;
		if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
			emu->x86.R_ECX = 0;
		}
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	} else {
		switch (size) {
		case 1:
			store_byte(emu, emu->x86.R_ES, emu->x86.R_DI,
			    (*emu->emu_inb) (emu, emu->x86.R_DX));
			break;
		case 2:
			store_word(emu, emu->x86.R_ES, emu->x86.R_DI,
			    (*emu->emu_inw) (emu, emu->x86.R_DX));
			break;
		case 4:
			store_long(emu, emu->x86.R_ES, emu->x86.R_DI,
			    (*emu->emu_inl) (emu, emu->x86.R_DX));
			break;
		}
		emu->x86.R_DI += inc;
	}
}


static void 
outs(struct x86emu *emu, int32 size)
{
	int32 inc = size;

	if (ACCESS_FLAG(F_DF)) {
		inc = -size;
	}
	if (emu->x86.mode & (SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE)) {
		
		
		uint32 count = ((emu->x86.mode & SYSMODE_PREFIX_DATA) ?
		    emu->x86.R_ECX : emu->x86.R_CX);
		switch (size) {
		case 1:
			while (count--) {
				(*emu->emu_outb) (emu, emu->x86.R_DX,
				    fetch_byte(emu, emu->x86.R_ES,
				    emu->x86.R_SI));
				emu->x86.R_SI += inc;
			}
			break;

		case 2:
			while (count--) {
				(*emu->emu_outw) (emu, emu->x86.R_DX,
				    fetch_word(emu, emu->x86.R_ES,
				    emu->x86.R_SI));
				emu->x86.R_SI += inc;
			}
			break;
		case 4:
			while (count--) {
				(*emu->emu_outl) (emu, emu->x86.R_DX,
				    fetch_long(emu, emu->x86.R_ES,
				    emu->x86.R_SI));
				emu->x86.R_SI += inc;
				break;
			}
		}
		emu->x86.R_CX = 0;
		if (emu->x86.mode & SYSMODE_PREFIX_DATA) {
			emu->x86.R_ECX = 0;
		}
		emu->x86.mode &= ~(SYSMODE_PREFIX_REPE | SYSMODE_PREFIX_REPNE);
	} else {
		switch (size) {
		case 1:
			(*emu->emu_outb) (emu, emu->x86.R_DX,
			    fetch_byte(emu, emu->x86.R_ES, emu->x86.R_SI));
			break;
		case 2:
			(*emu->emu_outw) (emu, emu->x86.R_DX,
			    fetch_word(emu, emu->x86.R_ES, emu->x86.R_SI));
			break;
		case 4:
			(*emu->emu_outl) (emu, emu->x86.R_DX,
			    fetch_long(emu, emu->x86.R_ES, emu->x86.R_SI));
			break;
		}
		emu->x86.R_SI += inc;
	}
}


static void 
push_word(struct x86emu *emu, uint16 w)
{
	emu->x86.R_SP -= 2;
	store_word(emu, emu->x86.R_SS, emu->x86.R_SP, w);
}


static void 
push_long(struct x86emu *emu, uint32 w)
{
	emu->x86.R_SP -= 4;
	store_long(emu, emu->x86.R_SS, emu->x86.R_SP, w);
}


static uint16 
pop_word(struct x86emu *emu)
{
	uint16 res;

	res = fetch_word(emu, emu->x86.R_SS, emu->x86.R_SP);
	emu->x86.R_SP += 2;
	return res;
}


static uint32 
pop_long(struct x86emu *emu)
{
	uint32 res;

	res = fetch_long(emu, emu->x86.R_SS, emu->x86.R_SP);
	emu->x86.R_SP += 4;
	return res;
}