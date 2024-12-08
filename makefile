CFLAGS  = -m32 -fno-builtin -nostdinc -nostdlib

GASFLAGS = --32

OBJFILES = \
	loader.o  \
	sources/std.o  \
	sources/pci.o  \
	sources/bios.o  \
	sources/vbe.o  \
	sources/warning.o  \
	sources/fatal_error.o  \
	sources/error.o  \
	sources/mems.o \
	sources/mem.o \
	sources/speaker.o \
	sources/power.o \
	sources/acpi.o \
	sources/io.o  \
	sources/gdt.o  \
	sources/gdts.o  \
	sources/idt.o  \
	sources/idts.o  \
	sources/pit.o  \
	sources/keyboardps2.o  \
	sources/mem.o  \
	sources/format.o  \
	sources/time.o  \
	sources/pci_driver.o \
	sources/colors.o \
	sources/glyph.o \
	sources/ugsm.o \
	sources/ascii.o \
	sources/rus.o \
	sources/qdivrem.o  \
	sources/udivdi3.o  \
	sources/umoddi3.o  \
	sources/divdi3.o  \
	sources/moddi3.o  \
	sources/x86emu.o  \
	sources/x86emu_util.o  \
	sources/setjmp.o  \
	sources/math.o  \
	sources/abc.o  \
	sources/archive.o  \
	sources/RL/lexer.o  \
	sources/RMAL/lexer.o  \
	sources/RMAL/parser.o  \
	kernel.o

image:
	@echo "Creating hdd.img..."
	@dd if=/dev/zero of=./hdd.img bs=512 count=16065 1>/dev/null 2>&1

	@echo "Creating bootable first FAT32 partition..."
	@losetup /dev/loop10 ./hdd.img
	@(echo c; echo u; echo n; echo p; echo 1; echo ;  echo ; echo a; echo 1; echo t; echo c; echo w;) | fdisk /dev/loop10 1>/dev/null 2>&1 || true

	@echo "Mounting partition to /dev/loop11..."
	@losetup /dev/loop11 ./hdd.img \
		--offset 32256 \
		--sizelimit 8224768
	@losetup -d /dev/loop10

	@echo "Format partition..."
	@mkdosfs /dev/loop11

	@echo "Copy kernel and grub files on partition..."
	@mkdir -p tempdir
	@mount /dev/loop11 tempdir
	@mkdir tempdir/boot
	@cp -r ./grub tempdir/boot/
	@cp kernel.bin tempdir/
	@sleep 1
	@umount /dev/loop11
	@rm -r tempdir
	@losetup -d /dev/loop11

	@echo "Installing GRUB..."
	@echo "device (hd0) hdd.img \n \
	       root (hd0,0)         \n \
	       setup (hd0)          \n \
	       quit\n" | grub --batch 1>/dev/null
	@echo "Done!"

all: gassources nasmsources csources kernel.bin image vmwareDisk clean run
run:
	@qemu-system-i386 -soundhw pcspk -drive file=hdd.img,format=raw
flash:
	@sudo dd if=./hdd.img of=/dev/sdb

nasmsources:
	@nasm -f elf32 sources/gdt.n -o sources/gdts.o

	@nasm -f elf32 sources/idt.n -o sources/idts.o

gassources:
	@as $(GASFLAGS) -o loader.o loader.s

	@as $(GASFLAGS) -o sources/mems.o sources/mem.s

	@as $(GASFLAGS) -o sources/setjmp.o sources/setjmp.s

csources:
	@gcc -Iinclude $(CFLAGS) -o sources/std.o -c sources/std.c

	@gcc -Iinclude $(CFLAGS) -o sources/pci.o -c sources/pci.c

	@gcc -Iinclude $(CFLAGS) -o sources/bios.o -c sources/bios.c

	@gcc -Iinclude $(CFLAGS) -o sources/vbe.o -c sources/vbe.c

	@gcc -Iinclude $(CFLAGS) -o sources/warning.o -c sources/warning.c

	@gcc -Iinclude $(CFLAGS) -o sources/fatal_error.o -c sources/fatal_error.c

	@gcc -Iinclude $(CFLAGS) -o sources/error.o -c sources/error.c

	@gcc -Iinclude $(CFLAGS) -o sources/power.o -c sources/power.c

	@gcc -Iinclude $(CFLAGS) -o sources/acpi.o -c sources/acpi.c

	@gcc -Iinclude $(CFLAGS) -o sources/io.o -c sources/io.c

	@gcc -Iinclude $(CFLAGS) -o sources/gdt.o -c sources/gdt.c

	@gcc -Iinclude $(CFLAGS) -o sources/idt.o -c sources/idt.c

	@gcc -Iinclude $(CFLAGS) -o sources/pit.o -c sources/pit.c

	@gcc -Iinclude $(CFLAGS) -o sources/keyboardps2.o -c sources/keyboardps2.c

	@gcc -Iinclude $(CFLAGS) -o sources/mem.o -c sources/mem.c

	@gcc -Iinclude $(CFLAGS) -o sources/speaker.o -c sources/speaker.c

	@gcc -Iinclude $(CFLAGS) -o sources/format.o -c sources/format.c

	@gcc -Iinclude $(CFLAGS) -o sources/time.o -c sources/time.c

	@gcc -Iinclude $(CFLAGS) -o sources/pci_driver.o -c sources/pci_driver.c

	@gcc -Iinclude $(CFLAGS) -o sources/colors.o -c sources/colors.c

	@gcc -Iinclude $(CFLAGS) -o sources/glyph.o -c sources/glyph.c

	@gcc -Iinclude $(CFLAGS) -o sources/ugsm.o -c sources/ugsm.c

	@gcc -Iinclude $(CFLAGS) -o sources/ascii.o -c sources/ascii.c

	@gcc -Iinclude $(CFLAGS) -o sources/rus.o -c sources/rus.c

	@gcc -Iinclude $(CFLAGS) -o sources/qdivrem.o -c sources/qdivrem.c

	@gcc -Iinclude $(CFLAGS) -o sources/udivdi3.o -c sources/udivdi3.c

	@gcc -Iinclude $(CFLAGS) -o sources/umoddi3.o -c sources/umoddi3.c

	@gcc -Iinclude $(CFLAGS) -o sources/divdi3.o -c sources/divdi3.c

	@gcc -Iinclude $(CFLAGS) -o sources/moddi3.o -c sources/moddi3.c

	@gcc -Iinclude $(CFLAGS) -o sources/x86emu.o -c sources/x86emu.c

	@gcc -Iinclude $(CFLAGS) -o sources/x86emu_util.o -c sources/x86emu_util.c

	@gcc -Iinclude $(CFLAGS) -o sources/math.o -c sources/math.c

	@gcc -Iinclude $(CFLAGS) -o sources/abc.o -c sources/abc.c

	@gcc -Iinclude $(CFLAGS) -o sources/archive.o -c sources/archive.c

	@gcc -Iinclude $(CFLAGS) -o sources/RL/lexer.o -c sources/RL/lexer.c

	@gcc -Iinclude $(CFLAGS) -o sources/RMAL/lexer.o -c sources/RMAL/lexer.c

	@gcc -Iinclude $(CFLAGS) -o sources/RMAL/parser.o -c sources/RMAL/parser.c

	@gcc -Iinclude $(CFLAGS) -o kernel.o -c kernel.c

kernel.bin: $(OBJFILES)
	@ld -m elf_i386 -T linker.ld -o $@ $^
vmwareDisk:
	@qemu-img convert -f raw hdd.img -O vmdk ./RandomOS.vmdk
clean:
	@rm -f $(OBJFILES) kernel.bin