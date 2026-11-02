! Startup code of the Stackette platform. TODO: set the stacks, clear the bss,
! copy the initialized data from the code space, and call main.

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
begtext:
	nop
	jmp __m_a_i_n

.sect .data
begdata:

.sect .bss
begbss:
