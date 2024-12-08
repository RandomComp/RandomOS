#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include "types.h"

struct multibootAOutSymbolTable {
	uint32 tabsize;

	uint32 strsize;

	uint32 addr;

	uint32 reserved;
};

struct multibootELFSectionHeaderTable {
	uint32 num;

	uint32 size;

	uint32 addr;

	uint32 shndx;
};

struct multibootInfo {
	uint32 flags;

	uint32 mem_lower;

	uint32 mem_upper;

	uint32 boot_device;

	uint32 cmdline;

	uint32 mods_count;
	
	uint32 mods_addr;

	union {
		struct multibootAOutSymbolTable aout_sym;

		struct multibootELFSectionHeaderTable elf_sec;
	} u;

	uint32 mmap_length;
	
	uint32 mmap_addr;

	uint32 drives_length;
	
	uint32 drives_addr;

	uint32 config_table;

	uint32 boot_loader_name;

	uint32 apm_table;

	uint32 vbe_control_info;

	uint32 vbe_mode_info;

	uint16 vbe_mode;

	uint16 vbe_interface_seg;

	uint16 vbe_interface_off;

	uint16 vbe_interface_len;

	uint64 framebuffer_addr;

	uint32 framebuffer_pitch;

	uint32 framebuffer_width;

	uint32 framebuffer_height;

	uint8 framebuffer_bpp;

	#define FRAMEBUFFER_TYPE_INDEXED 0

	#define FRAMEBUFFER_TYPE_RGB 1

	#define FRAMEBUFFER_TYPE_EGA_TEXT 2

	uint8 framebuffer_type;

	union {
		struct {
			uint32 framebuffer_palette_addr;

			uint16 framebuffer_palette_num_colors;
		};

		struct {
			uint8 framebuffer_red_field_position;

			uint8 framebuffer_red_mask_size;

			uint8 framebuffer_green_field_position;

			uint8 framebuffer_green_mask_size;

			uint8 framebuffer_blue_field_position;
			
			uint8 framebuffer_blue_mask_size;
		};
	};
};

struct multibootMMapEntry {
	uint32 size;

	uint32 addr_low;

	uint32 addr_high;

	uint32 len_low;

	uint32 len_high;
	
	#define MULTIBOOT_MEMORY_AVAILABLE 1

	#define MULTIBOOT_MEMORY_RESERVED 2

	#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3

	#define MULTIBOOT_MEMORY_NVS 4

	#define MULTIBOOT_MEMORY_BADRAM 5

	uint32 type;

} PACKED;

#endif