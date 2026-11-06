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
.define .csa4
.define .csb4

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

! The same for a long index (low word on top of the high word below the
! descriptor address). The descriptor of csa has the default target, the lower
! bound (2 words), upper-lower (2 words, only the low one is used: a longer
! table does not fit the address space), the targets; that of csb the default,
! the count (the low word only), and n entries of the value (2 words) and the
! target.

.csa4:
	ld push, [tos]
	ld push, [sp+1]
	add.tos $4
	ld tos, [tos]
	ld push, [sp+2]
	add.tos $2
	ld tos, [tos]
	ld push, [sp+4]
	sub.tos [sp+1]
	ld push, [sp+6]
	sbc.tos [sp+3]
	cmp tos, $0
	skip eq
	bra csa4_d8
	ld push, [sp+5]
	add.tos $6
	ld tos, [tos]
	cmp tos, [sp+2]
	st null, pop
	skip hs
	bra csa4_d8
	ld push, [sp+1]
	add.tos tos
	add.tos [sp+6]
	add.tos $10
	ld tos, [tos]
	cmp tos, $0
	skip ne
	bra csa4_d9
csa4_go:
	st [sp+8], tos
	addp sp, $16
	st ip, pop
csa4_d8:
	ld push, [sp+4]
	bra csa4_chk
csa4_d9:
	ld tos, [sp+5]
csa4_chk:
	cmp tos, $0
	skip ne
	bra csa4_fatal
	bra csa4_go
csa4_fatal:
	bra csa4_fatal

.csb4:
	ld push, [tos]
	ld push, [sp+1]
	add.tos $6
	ld push, [sp+2]
	add.tos $2
	ld tos, [tos]
csb4_loop:
	cmp tos, $0
	skip ne
	bra csb4_d6
	ld push, [sp+1]
	ld tos, [tos]
	cmp tos, [sp+5]
	st null, pop
	skip eq
	bra csb4_next
	ld push, [sp+1]
	add.tos $2
	ld tos, [tos]
	cmp tos, [sp+6]
	st null, pop
	skip eq
	bra csb4_next
	ld push, [sp+1]
	add.tos $4
	ld tos, [tos]
	cmp tos, $0
	skip ne
	bra csb4_d7
csb4_go:
	st [sp+6], tos
	addp sp, $12
	st ip, pop
csb4_next:
	add.nos $6
	sub tos, $1
	bra csb4_loop
csb4_d6:
	ld push, [sp+2]
	bra csb4_chk
csb4_d7:
	ld tos, [sp+3]
csb4_chk:
	cmp tos, $0
	skip ne
	bra csb4_fatal
	bra csb4_go
csb4_fatal:
	bra csb4_fatal
