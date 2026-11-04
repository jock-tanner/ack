! Startup code of the Stackette platform.
!
! The CPU starts the system context at address 0 with PSP = 0xf000, RSP = 0
! and PFP = PSP, so the stacks are ready. The same address is entered again
! when the system context faults (a soft reset); the interrupt ID at 0xf02e
! is 0 only after a power-up.
!
! The image in flash is text, rom and then the initialized data, which is
! copied to its place in RAM. The bss is cleared. Both loops go through
! [mbp+midx++] with MBP = 0, and the source of the copy is a pointer on the
! stack.

.sect .text
.sect .rom
.sect .data
.sect .bss

.extern endrom, enddata, endbss, __m_a_i_n

.sect .text
begtext:
	ld push, [0xf02e]
	sub tos, $0
	st null, pop
	skip eq
	bra fault

	! The flash is 4 KiB in the default map, but the image is bigger: map the
	! pages below the data (code and data space) to the following pages of the
	! flash, so that the code, the rom and the image of the data are reached
	! at their own addresses. The pages are 0x1000 bytes, the physical page
	! of the flash is 0x2000 + n, and the MMU registers of the system context
	! are at 0xf100 (code) and 0xf160 (data), a word for each page.
	ld push, #begdata
	bswap tos
	shr tos
	shr tos
	shr tos
	shr tos
	ld push, $1
map:
	cmp tos, [sp+1]
	skip lo
	bra mapped
	ld push, tos
	add tos, #0x2000
	ld push, tos
	ld push, [sp+2]
	add tos, tos
	add tos, #0xf100
	st [tos], under
	add tos, $0x60
	st [tos], under
	st null, pop
	add tos, $1
	bra map
mapped:
	st null, pop
	st null, pop

	ld push, #endrom
	ld push, $0
	st mbp, pop
	ld push, #begdata
	st midx, pop
copy:
	ld push, midx
	sub tos, #enddata
	st null, pop
	skip ne
	bra copied
	ld push, [tos]
	st [mbp+midx++], pop
	add tos, $2
	bra copy
copied:
	st null, pop

clear:
	ld push, midx
	sub tos, #endbss
	st null, pop
	skip ne
	bra cleared
	ld push, $0
	st [mbp+midx++], pop
	bra clear
cleared:

	! main(argc, argv, envp): the parameters of the C start-up, the first
	! one on top
	ld push, #bootenvp
	ld push, #bootargv
	ld push, $1
	calla __m_a_i_n

! the exit routine of the C library: _exit
.define __exit
.extern __exit
__exit:
halt:
	bra halt

! a fault restarted the system context: write ! and stop
fault:
	ld push, $33
	st [0xf000], pop
	bra halt

.sect .rom
bootargv:
	.data2 bootname, 0
bootenvp:
	.data2 0
bootname:
	.asciz "a.out"

.sect .data
begdata:

.sect .bss
begbss:
