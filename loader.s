.text

.global loader

.set MAGIC, 0x1badb002

.set FLAGS, 0x00000003

.set CHECKSUM, -(MAGIC + FLAGS)

.align 4

	.long MAGIC

	.long FLAGS

	.long CHECKSUM

.set stackSize, 0x4000

.lcomm stack, stackSize

.comm mbd, 4

.comm magic, 4

loader:
	movl $(stack + stackSize), %esp
	
	push %ebx

	push %eax

	call main

	cli
hang:
	hlt

	jmp hang
