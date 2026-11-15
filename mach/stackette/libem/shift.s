! Shifts by a variable count. The count is on top of the value; the callee
! removes both and leaves the result on the stack (frame: parameter words,
! one result word, see retf). The shl, shr and sar instructions saturate: a
! count of the width or more gives 0 (for sar the sign), which the long
! shifts use.
!
! The compiler does the shifts of a word in line (shl push, tos and the like),
! the routines of a word are here for the code that calls them.

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

.sli2:
	frame 0, 4
	ld push, [pfp+2]
	ld push, [pfp+0]
	shl push, tos
	retf $1

.sri2:
	frame 0, 4
	ld push, [pfp+2]
	ld push, [pfp+0]
	sar push, tos
	retf $1

.sru2:
	frame 0, 4
	ld push, [pfp+2]
	ld push, [pfp+0]
	shr push, tos
	retf $1

! Shifts of a long (low word on top of the stack): the parameters are the
! count, then the low word, then the high word (3 words); the result is a long
! (high word first, so that the low word is on top). With the count n below 16
! one half gets the bits that fall out of the other: n = 0 works as the shifts
! by 16 give 0 (or the sign). With 16 or more only one word is left.
.sli4:
	frame 0, 6
	ld push, [pfp+0]
	cmp pop, $16
	skip lo
	bra sli4_big
	ld push, [pfp+4]
	ld push, [pfp+0]
	shl push, tos
	ld push, [pfp+2]
	ld push, $16
	sub tos, [pfp+0]
	shr push, tos
	or push, tos
	ld push, [pfp+2]
	ld push, [pfp+0]
	shl push, tos
	retf $2
sli4_big:
	ld push, [pfp+2]
	ld push, [pfp+0]
	sub tos, $16
	shl push, tos
	ld push, $0
	retf $2

.sru4:
	frame 0, 6
	ld push, [pfp+0]
	cmp pop, $16
	skip lo
	bra sru4_big
	ld push, [pfp+4]
	ld push, [pfp+0]
	shr push, tos
	ld push, [pfp+2]
	ld push, [pfp+0]
	shr push, tos
	ld push, [pfp+4]
	ld push, $16
	sub tos, [pfp+0]
	shl push, tos
	or push, tos
	retf $2
sru4_big:
	ld push, $0
	ld push, [pfp+4]
	ld push, [pfp+0]
	sub tos, $16
	shr push, tos
	retf $2

.sri4:
	frame 0, 6
	ld push, [pfp+0]
	cmp pop, $16
	skip lo
	bra sri4_big
	ld push, [pfp+4]
	ld push, [pfp+0]
	sar push, tos
	ld push, [pfp+2]
	ld push, [pfp+0]
	shr push, tos
	ld push, [pfp+4]
	ld push, $16
	sub tos, [pfp+0]
	shl push, tos
	or push, tos
	retf $2
sri4_big:
	ld push, [pfp+4]
	ld push, $15
	sar push, tos
	ld push, [pfp+4]
	ld push, [pfp+0]
	sub tos, $16
	sar push, tos
	retf $2
