! Wrappers for the floating point instructions on 8 bytes, and for the
! conversions that have more result words than parameter words. A parameter
! is a word of the stack: the top operand is at [pfp+0], the low word of an
! operand first.

.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.define .adf8
.adf8:
	frame 0, 16
	lea push, [pfp+0]
	lea push, [pfp+8]
	calla _sf_adf8
	retn $4

.define .sbf8
.sbf8:
	frame 0, 16
	lea push, [pfp+0]
	lea push, [pfp+8]
	calla _sf_sbf8
	retn $4

.define .mlf8
.mlf8:
	frame 0, 16
	lea push, [pfp+0]
	lea push, [pfp+8]
	calla _sf_mlf8
	retn $4

.define .dvf8
.dvf8:
	frame 0, 16
	lea push, [pfp+0]
	lea push, [pfp+8]
	calla _sf_dvf8
	retn $4

.define .ngf8
.ngf8:
	frame 0, 8
	lea push, [pfp+0]
	calla _sf_ngf8
	retn $0

.define .cmf8
.cmf8:
	frame 0, 16
	lea push, [pfp+0]
	lea push, [pfp+8]
	calla _sf_cmf8x
	retf $1

.define .cif8
.cif8:
	frame 8, 4
	addp sp, $-8
	lea push, [pfp-8]
	ld push, [pfp+2]
	ld push, [pfp+0]
	calla _sf_cif8x
	ld push, [pfp-2]
	ld push, [pfp-4]
	ld push, [pfp-6]
	ld push, [pfp-8]
	retf $4

.define .cuf8
.cuf8:
	frame 8, 4
	addp sp, $-8
	lea push, [pfp-8]
	ld push, [pfp+2]
	ld push, [pfp+0]
	calla _sf_cuf8x
	ld push, [pfp-2]
	ld push, [pfp-4]
	ld push, [pfp-6]
	ld push, [pfp-8]
	retf $4

.define .cff48
.cff48:
	frame 8, 4
	addp sp, $-8
	lea push, [pfp-8]
	ld push, [pfp+2]
	ld push, [pfp+0]
	calla _sf_cff48x
	ld push, [pfp-2]
	ld push, [pfp-4]
	ld push, [pfp-6]
	ld push, [pfp-8]
	retf $4

.define .cff84
.cff84:
	frame 0, 8
	lea push, [pfp+0]
	calla _sf_cff84x
	retf $2

.define .cfi8
.cfi8:
	frame 0, 8
	lea push, [pfp+0]
	calla _sf_cfi8x
	retf $2

.define .cfu8
.cfu8:
	frame 0, 8
	lea push, [pfp+0]
	calla _sf_cfu8x
	retf $2

.define .fef4
.fef4:
	frame 6, 4
	addp sp, $-6
	ld push, [pfp+2]
	ld push, [pfp+0]
	lea push, [pfp-6]
	calla _fef4
	ld push, [pfp-2]
	ld push, [pfp-4]
	ld push, [pfp-6]
	retf $3

.define .fef8
.fef8:
	frame 10, 8
	addp sp, $-10
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	lea push, [pfp-10]
	calla _fef8
	ld push, [pfp-2]
	ld push, [pfp-4]
	ld push, [pfp-6]
	ld push, [pfp-8]
	ld push, [pfp-10]
	retf $5

.define .fif4
.fif4:
	frame 8, 8
	addp sp, $-8
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	lea push, [pfp-8]
	calla _fif4
	ld push, [pfp-2]
	ld push, [pfp-4]
	ld push, [pfp-6]
	ld push, [pfp-8]
	retf $4

.define .fif8
.fif8:
	frame 16, 16
	addp sp, $-16
	ld push, [pfp+14]
	ld push, [pfp+12]
	ld push, [pfp+10]
	ld push, [pfp+8]
	ld push, [pfp+6]
	ld push, [pfp+4]
	ld push, [pfp+2]
	ld push, [pfp+0]
	lea push, [pfp-16]
	calla _fif8
	ld push, [pfp-2]
	ld push, [pfp-4]
	ld push, [pfp-6]
	ld push, [pfp-8]
	ld push, [pfp-10]
	ld push, [pfp-12]
	ld push, [pfp-14]
	ld push, [pfp-16]
	retf $8
