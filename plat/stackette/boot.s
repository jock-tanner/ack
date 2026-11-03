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

	calla __m_a_i_n

halt:
	bra halt

! a fault restarted the system context: write ! and stop
fault:
	ld push, $33
	st [0xf000], pop
	bra halt

.sect .data
begdata:

.sect .bss
begbss:
