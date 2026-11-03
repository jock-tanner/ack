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
