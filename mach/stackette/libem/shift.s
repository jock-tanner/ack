! Shifts of a word by a variable count. The count is on top of the value; the
! callee removes both and leaves the result on the stack (frame: two
! parameter words, one result word, see retf).

.sect .text
.sect .rom
.sect .data
.sect .bss
.define .sli2
.define .sri2
.define .sru2
.define .sli4
.define .sri4
.define .sru4

.sect .text

! shift left: clear the carry, rotate through it
.sli2:
	frame 0, 4
	ld push, [pfp+2]
	ld push, [pfp+0]
sli2_loop:
	cmp tos, $0
	skip ne
	bra sli2_done
	clrb $1
	rlc nos
	sub tos, $1
	bra sli2_loop
sli2_done:
	st null, pop
	retf $1

! shift right, signed: the carry gets the sign of the value (by rotating a
! copy), and the rotate right through it puts it back in at the top
.sri2:
	frame 0, 4
	ld push, [pfp+2]
	ld push, [pfp+0]
sri2_loop:
	cmp tos, $0
	skip ne
	bra sri2_done
	ld push, nos
	rlc tos
	st null, pop
	rrc nos
	sub tos, $1
	bra sri2_loop
sri2_done:
	st null, pop
	retf $1

! shift right, unsigned
.sru2:
	frame 0, 4
	ld push, [pfp+2]
	ld push, [pfp+0]
sru2_loop:
	cmp tos, $0
	skip ne
	bra sru2_done
	clrb $1
	rrc nos
	sub tos, $1
	bra sru2_loop
sru2_done:
	st null, pop
	retf $1

! Shifts of a long (low word on top) by a variable count: two instructions
! per bit. The count is in MIDX so that both words are on top of the stack.
! Parameters: the count, then the low word, then the high word (3 words),
! the result is a long.
.sli4:
	frame 0, 6
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	st midx, pop
sli4_loop:
	ld push, midx
	cmp pop, $0
	skip ne
	bra sli4_done
	add tos, tos
	adc nos, nos
	addp midx, $-1
	bra sli4_loop
sli4_done:
	retf $2

.sru4:
	frame 0, 6
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	st midx, pop
sru4_loop:
	ld push, midx
	cmp pop, $0
	skip ne
	bra sru4_done
	shr nos
	rrc tos
	addp midx, $-1
	bra sru4_loop
sru4_done:
	retf $2

.sri4:
	frame 0, 6
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	st midx, pop
sri4_loop:
	ld push, midx
	cmp pop, $0
	skip ne
	bra sri4_done
	sar nos
	rrc tos
	addp midx, $-1
	bra sri4_loop
sri4_done:
	retf $2
