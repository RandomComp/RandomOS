#ifndef __X86EMU_REGS_H
#define __X86EMU_REGS_H

#define R_AH register_a.I8_reg.h_reg

#define R_AL register_a.I8_reg.l_reg

#define R_BH register_b.I8_reg.h_reg

#define R_BL register_b.I8_reg.l_reg

#define R_CH register_c.I8_reg.h_reg

#define R_CL register_c.I8_reg.l_reg

#define R_DH register_d.I8_reg.h_reg

#define R_DL register_d.I8_reg.l_reg

#define R_AX register_a.I16_reg.x_reg

#define R_BX register_b.I16_reg.x_reg

#define R_CX register_c.I16_reg.x_reg

#define R_DX register_d.I16_reg.x_reg

#define R_EAX register_a.I32_reg.e_reg

#define R_EBX register_b.I32_reg.e_reg

#define R_ECX register_c.I32_reg.e_reg

#define R_EDX register_d.I32_reg.e_reg

#define R_SP register_sp.I16_reg.x_reg

#define R_BP register_bp.I16_reg.x_reg

#define R_SI register_si.I16_reg.x_reg

#define R_DI register_di.I16_reg.x_reg

#define R_IP register_ip.I16_reg.x_reg

#define R_FLG register_flags

#define R_ESP register_sp.I32_reg.e_reg

#define R_EBP register_bp.I32_reg.e_reg

#define R_ESI register_si.I32_reg.e_reg

#define R_EDI register_di.I32_reg.e_reg

#define R_EIP register_ip.I32_reg.e_reg

#define R_EFLG register_flags


#define R_CS register_cs

#define R_DS register_ds

#define R_SS register_ss

#define R_ES register_es

#define R_FS register_fs

#define R_GS register_gs

#define FB_CF 0x0001

#define FB_PF 0x0004

#define FB_AF 0x0010

#define FB_ZF 0x0040

#define FB_SF 0x0080

#define FB_TF 0x0100

#define FB_IF 0x0200

#define FB_DF 0x0400

#define FB_OF 0x0800

#define F_ALWAYS_ON (0x0002)

#define F_MSK (FB_CF | FB_PF | FB_AF | FB_ZF | FB_SF | FB_TF | FB_IF | FB_DF | FB_OF)

#define F_CF 0x0001

#define F_PF 0x0004

#define F_AF 0x0010

#define F_ZF 0x0040

#define F_SF 0x0080

#define F_TF 0x0100

#define F_IF 0x0200

#define F_DF 0x0400

#define F_OF 0x0800

#define SET_FLAG(flag) (emu->x86.R_FLG |= (flag))

#define CLEAR_FLAG(flag) (emu->x86.R_FLG &= ~(flag))

#define ACCESS_FLAG(flag) (emu->x86.R_FLG & (flag))

#define CLEARALL_FLAG(m) (emu->x86.R_FLG = 0)

#define CONDITIONAL_SET_FLAG(COND,FLAG) \
	if (COND) SET_FLAG(FLAG); else CLEAR_FLAG(FLAG)

#define F_PF_CALC 0x010000

#define F_ZF_CALC 0x020000

#define F_SF_CALC 0x040000

#define F_ALL_CALC 0xff0000


#define SYSMODE_SEG_DS_SS 0x00000001
#define SYSMODE_SEGOVR_CS 0x00000002
#define SYSMODE_SEGOVR_DS 0x00000004
#define SYSMODE_SEGOVR_ES 0x00000008
#define SYSMODE_SEGOVR_FS 0x00000010
#define SYSMODE_SEGOVR_GS 0x00000020
#define SYSMODE_SEGOVR_SS 0x00000040
#define SYSMODE_PREFIX_REPE 0x00000080
#define SYSMODE_PREFIX_REPNE 0x00000100
#define SYSMODE_PREFIX_DATA 0x00000200
#define SYSMODE_PREFIX_ADDR 0x00000400
#define SYSMODE_INTR_PENDING 0x10000000
#define SYSMODE_EXTRN_INTR 0x20000000
#define SYSMODE_HALTED 0x40000000

#define SYSMODE_SEGMASK (SYSMODE_SEG_DS_SS | \
						 SYSMODE_SEGOVR_CS | \
						 SYSMODE_SEGOVR_DS | \
						 SYSMODE_SEGOVR_ES | \
						 SYSMODE_SEGOVR_FS | \
						 SYSMODE_SEGOVR_GS | \
						 SYSMODE_SEGOVR_SS)
#define SYSMODE_CLRMASK (SYSMODE_SEG_DS_SS | \
						 SYSMODE_SEGOVR_CS | \
						 SYSMODE_SEGOVR_DS | \
						 SYSMODE_SEGOVR_ES | \
						 SYSMODE_SEGOVR_FS | \
						 SYSMODE_SEGOVR_GS | \
						 SYSMODE_SEGOVR_SS | \
						 SYSMODE_PREFIX_DATA | \
						 SYSMODE_PREFIX_ADDR)

#define INTR_SYNCH 0x1

#endif