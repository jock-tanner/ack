! Block move: the parameters are the byte count (at [pfp+0]), the destination
! ([pfp+2]) and the source ([pfp+4]). The count is even. The stack holds the
! destination, the source and the end of the destination.

.sect .text
.sect .rom
.sect .data
.sect .bss
.define .blm

.sect .text

.blm:
	frame 0, 6
	ld push, [pfp+2]
	add.tos [pfp+0]
	ld push, [pfp+4]
	ld push, [pfp+2]
blm_loop:
	cmp tos, [sp+2]
	skip ne
	bra blm_done
	ld push, [sp+1]
	ld tos, [tos]
	ld push, [sp+1]
	st.under [tos]
	st null, pop
	add.tos $2
	add.nos $2
	bra blm_loop
blm_done:
	addp sp, $6
	retf $0
