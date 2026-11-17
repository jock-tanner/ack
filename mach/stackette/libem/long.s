! Arithmetic on longs. A long has its low word on top: the parameters of a
! two-operand helper are b (low word at [pfp+0], high word at [pfp+2]) and
! a (low word at [pfp+4], high word at [pfp+6]).

.sect .text
.sect .rom
.sect .data
.sect .bss
.define .mli4
.define .dvu4
.define .rmu4
.define .dvi4
.define .rmi4
.define .cmi4
.define .cmu4

.sect .text

! a*b: the low 32 bits are al*bl (the full 32 bits) plus (ah*bl + al*bh)<<16,
! and that does not depend on the signs
.mli4:
	frame 2, 8
	addp sp, $-2
	ld push, [pfp+6]
	ld push, [pfp+0]
	mulu
	st.under null
	ld push, [pfp+4]
	ld push, [pfp+2]
	mulu
	st.under null
	add.push tos
	st [pfp-2], pop
	ld push, [pfp+4]
	ld push, [pfp+0]
	mulu
	add.nos [pfp-2]
	retf $2

! unsigned division and remainder of a by b, bit by bit. The result is the
! remainder (on top) and the quotient (below it). The quotient bits are
! shifted into the dividend, which is kept on the stack. Locals: the
! remainder in [pfp-2] (low) and [pfp-4], the bit count in [pfp-6].
udivmod4:
	frame 6, 8
	addp sp, $-6
	ld push, $0
	st [pfp-2], pop
	ld push, $0
	st [pfp-4], pop
	ld push, $32
	st [pfp-6], pop
! a divisor that is a power of two (below 65536), the usual one, is a shift
! and a mask: the count k is kept in [pfp-6], the remainder is a & (b-1)
	ld push, [pfp+2]
	cmp pop, $0
	skip eq
	bra udm_gen
	ld push, [pfp+0]
	cmp pop, $0
	skip ne
	bra udm_gen
	ld push, [pfp+0]
	sub tos, $1
	and.tos [pfp+0]
	cmp pop, $0
	skip eq
	bra udm_gen
	ld push, $0
	st [pfp-6], pop
	ld push, [pfp+0]
	st [pfp-2], pop
udm_k:
	ld push, [pfp-2]
	cmp pop, $1
	skip ne
	bra udm_kdone
	ld push, [pfp-2]
	shr tos, $1
	st [pfp-2], pop
	ld push, [pfp-6]
	add tos, $1
	st [pfp-6], pop
	bra udm_k
udm_kdone:
	ld push, [pfp+0]
	sub tos, $1
	and.tos [pfp+4]
	st [pfp-2], pop
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp-6]
	calla .sru4
	ld push, $0
	ld push, [pfp-2]
	retf $4
udm_gen:
	ld push, $32
	st [pfp-6], pop
	ld push, [pfp+6]
	ld push, [pfp+4]
udm_loop:
	add.tos tos
	adc.nos nos
	ld push, [pfp-4]
	ld push, [pfp-2]
	adc.tos tos
	adc.nos nos
	skip cc
	bra udm_sub
	sub.tos [pfp+0]
	sbc.nos [pfp+2]
	skip cs
	bra udm_restore
	bra udm_keep
udm_sub:
	sub.tos [pfp+0]
	sbc.nos [pfp+2]
udm_keep:
	st [pfp-2], pop
	st [pfp-4], pop
	or.tos $1
	bra udm_next
udm_restore:
	add.tos [pfp+0]
	adc.nos [pfp+2]
	st [pfp-2], pop
	st [pfp-4], pop
udm_next:
	ld push, [pfp-6]
	sub tos, $1
	st [pfp-6], pop
	skip eq
	bra udm_loop
	ld push, [pfp-4]
	ld push, [pfp-2]
	retf $4

! the arguments are copied for udivmod4, which leaves r on top of q
.dvu4:
	frame 0, 8
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	calla udivmod4
	st null, pop
	st null, pop
	retf $2

.rmu4:
	frame 0, 8
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	calla udivmod4
	st [sp+2], tos
	st [sp+3], nos
	addp sp, $4
	retf $2

! signed: divide the absolute values. The quotient is negative when the signs
! differ, the remainder gets the sign of a. A sign word is kept in [pfp-2].
.dvi4:
	frame 2, 8
	addp sp, $-2
	ld push, [pfp+6]
	xor.tos [pfp+2]
	st [pfp-2], pop
	ld push, [pfp+6]
	cmp tos, $0
	st null, pop
	skip mi
	bra dvi_a
	ld push, [pfp+6]
	ld push, [pfp+4]
	not nos
	neg tos
	adc.nos $0
	st [pfp+4], pop
	st [pfp+6], pop
dvi_a:
	ld push, [pfp+2]
	cmp tos, $0
	st null, pop
	skip mi
	bra dvi_b
	ld push, [pfp+2]
	ld push, [pfp+0]
	not nos
	neg tos
	adc.nos $0
	st [pfp+0], pop
	st [pfp+2], pop
dvi_b:
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	calla udivmod4
	st null, pop
	st null, pop
	ld push, [pfp-2]
	cmp tos, $0
	st null, pop
	skip mi
	bra dvi_q
	not nos
	neg tos
	adc.nos $0
dvi_q:
	retf $2

.rmi4:
	frame 2, 8
	addp sp, $-2
	ld push, [pfp+6]
	st [pfp-2], pop
	ld push, [pfp+6]
	cmp tos, $0
	st null, pop
	skip mi
	bra rmi_a
	ld push, [pfp+6]
	ld push, [pfp+4]
	not nos
	neg tos
	adc.nos $0
	st [pfp+4], pop
	st [pfp+6], pop
rmi_a:
	ld push, [pfp+2]
	cmp tos, $0
	st null, pop
	skip mi
	bra rmi_b
	ld push, [pfp+2]
	ld push, [pfp+0]
	not nos
	neg tos
	adc.nos $0
	st [pfp+0], pop
	st [pfp+2], pop
rmi_b:
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	calla udivmod4
	ld push, [pfp-2]
	cmp tos, $0
	st null, pop
	skip mi
	bra rmi_r
	not nos
	neg tos
	adc.nos $0
rmi_r:
	st [sp+2], tos
	st [sp+3], nos
	addp sp, $4
	retf $2

! comparisons give -1, 0 or 1; the high words decide unless they are equal
.cmi4:
	frame 0, 8
	ld push, [pfp+6]
	cmp tos, [pfp+2]
	st null, pop
	skip ne
	bra cmi4_low
	skip lt
	bra cmi4_gt
	bra cmi4_less
cmi4_low:
	ld push, [pfp+4]
	cmp tos, [pfp+0]
	st null, pop
	skip ne
	bra cmi4_equal
	skip lo
	bra cmi4_gt
cmi4_less:
	ld push, $-1
	retf $1
cmi4_gt:
	ld push, $1
	retf $1
cmi4_equal:
	ld push, $0
	retf $1

.cmu4:
	frame 0, 8
	ld push, [pfp+6]
	cmp tos, [pfp+2]
	st null, pop
	skip ne
	bra cmu4_low
	skip lo
	bra cmu4_gt
	bra cmu4_less
cmu4_low:
	ld push, [pfp+4]
	cmp tos, [pfp+0]
	st null, pop
	skip ne
	bra cmu4_equal
	skip lo
	bra cmu4_gt
cmu4_less:
	ld push, $-1
	retf $1
cmu4_gt:
	ld push, $1
	retf $1
cmu4_equal:
	ld push, $0
	retf $1

