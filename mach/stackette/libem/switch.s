! Case jumps. The index is below the descriptor address on the stack. These
! routines are entered by `jmp` and leave by writing IP, so nothing is
! pushed on the return stack.
!
! csa: the descriptor is the default target, the lower bound, upper-lower,
!      and the targets (0 means the default).
! csb: the descriptor is the default target, the count n, and n pairs
!      (value, target).
! A zero default target is an error: both routines stop there.

.sect .text
.sect .rom
.sect .data
.sect .bss
.define .csa2
.define .csb2

.sect .text

.csa2:
	ld push, [tos]
	ld push, nos
	add.tos $2
	ld tos, [tos]
	sub.tos [sp+3]
	neg tos
	ld push, [sp+2]
	add.tos $4
	ld tos, [tos]
	cmp tos, [sp+1]
	st null, pop
	skip hs
	bra csa_dflt
	add.tos tos
	add.tos [sp+2]
	add.tos $6
	ld tos, [tos]
	cmp tos, $0
	skip ne
	bra csa_dflt
csa_go:
	st [sp+3], tos
	addp sp, $6
	st ip, pop
csa_dflt:
	ld tos, [sp+1]
	cmp tos, $0
	skip ne
	bra csa_fatal
	bra csa_go
csa_fatal:
	bra csa_fatal

.csb2:
	ld push, [tos]
	ld push, [sp+1]
	add.tos $2
	ld push, [tos]
	add.nos $2
csb_loop:
	cmp tos, $0
	skip ne
	bra csb_dflt
	ld push, nos
	ld tos, [tos]
	cmp tos, [sp+5]
	st null, pop
	skip ne
	bra csb_hit
	add.nos $4
	sub tos, $1
	bra csb_loop
csb_hit:
	ld push, nos
	add.tos $2
	ld tos, [tos]
	st [sp+5], tos
	addp sp, $10
	st ip, pop
csb_dflt:
	ld tos, [sp+2]
	cmp tos, $0
	skip ne
	bra csb_fatal
	st [sp+4], tos
	addp sp, $8
	st ip, pop
csb_fatal:
	bra csb_fatal
