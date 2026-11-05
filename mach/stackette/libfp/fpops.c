/*
 * Floating point for the Stackette, in software, on top of the extend() and
 * compact() core of mach/proto/fp.
 *
 * The EM floating point instructions work on the stack, and the callee of the
 * Stackette removes its own parameters and leaves its result in their place.
 * So a 4-byte operation is a plain function that takes the operands by value
 * (the top operand first) and returns the 4-byte result. The operations on 8
 * bytes are reached through fpwrap.s, which calls the functions with the
 * addresses of the operands on the stack.
 */

#include "FP_types.h"
#include "FP_shift.h"
#include "FP_trap.h"
#include "get_put.h"

/* the fp package calls this for the traps of EM; there is nowhere to go */
void _fptrp(int n)
{
	*(volatile char*)0xf000 = 'F';
	*(volatile char*)0xf000 = 'P';
	*(volatile char*)0xf000 = '0' + n;
	for (;;)
		;
}

SINGLE sf_adf4(SINGLE b, SINGLE a)
{
	EXTEND e1, e2;

	if (a == 0)
		return b;
	if (b == 0)
		return a;
	extend(&a, &e1, sizeof(SINGLE));
	extend(&b, &e2, sizeof(SINGLE));
	add_ext(&e1, &e2);
	compact(&e1, &a, sizeof(SINGLE));
	return a;
}

SINGLE sf_sbf4(SINGLE b, SINGLE a)
{
	EXTEND e1, e2;

	if (b == 0)
		return a;
	extend(&a, &e1, sizeof(SINGLE));
	extend(&b, &e2, sizeof(SINGLE));
	sub_ext(&e1, &e2);
	compact(&e1, &a, sizeof(SINGLE));
	return a;
}

SINGLE sf_mlf4(SINGLE b, SINGLE a)
{
	EXTEND e1, e2;

	extend(&a, &e1, sizeof(SINGLE));
	extend(&b, &e2, sizeof(SINGLE));
	mul_ext(&e1, &e2);
	compact(&e1, &a, sizeof(SINGLE));
	return a;
}

SINGLE sf_dvf4(SINGLE b, SINGLE a)
{
	EXTEND e1, e2;

	extend(&a, &e1, sizeof(SINGLE));
	extend(&b, &e2, sizeof(SINGLE));
	div_ext(&e1, &e2);
	compact(&e1, &a, sizeof(SINGLE));
	return a;
}

SINGLE sf_ngf4(SINGLE f)
{
	if (f != 0)
		f ^= 0x80000000UL;
	return f;
}

/* integer to float: the source is a long (the table widens a short) */
SINGLE sf_cif4(long src)
{
	EXTEND buf;
	SINGLE result;

	if (src == 0)
		return 0;
	zrf_ext(&buf);
	buf.exp = 31;
	buf.sign = src < 0 ? 0x8000 : 0;
	buf.m1 = src < 0 ? -src : src;
	nrm_ext(&buf);
	compact(&buf, &result, sizeof(SINGLE));
	return result;
}

SINGLE sf_cuf4(unsigned long src)
{
	EXTEND buf;
	SINGLE result;

	if (src == 0)
		return 0;
	zrf_ext(&buf);
	buf.exp = 31;
	buf.m1 = src;
	nrm_ext(&buf);
	compact(&buf, &result, sizeof(SINGLE));
	return result;
}

void sf_cif8x(long src, DOUBLE* res)
{
	EXTEND buf;

	if (src == 0) {
		res->d[0] = res->d[1] = 0;
		return;
	}
	zrf_ext(&buf);
	buf.exp = 31;
	buf.sign = src < 0 ? 0x8000 : 0;
	buf.m1 = src < 0 ? -src : src;
	nrm_ext(&buf);
	compact(&buf, &res->d[0], sizeof(DOUBLE));
}

void sf_cuf8x(unsigned long src, DOUBLE* res)
{
	EXTEND buf;

	if (src == 0) {
		res->d[0] = res->d[1] = 0;
		return;
	}
	zrf_ext(&buf);
	buf.exp = 31;
	buf.m1 = src;
	nrm_ext(&buf);
	compact(&buf, &res->d[0], sizeof(DOUBLE));
}

/* float to integer, truncating; the result is the same bits for both */
static long ftol(EXTEND* buf, int size, int is_signed)
{
	long new;
	short max_exp;

	if (buf->exp < 0)
		return 0;
	max_exp = is_signed ? 30 : 31;
	if (buf->exp > max_exp) {
		if (is_signed && buf->exp == max_exp + 1 && buf->sign &&
		    buf->m1 == NORMBIT && buf->m2 == 0L)
			return 0x80000000L;
		trap(EIOVFL);
		buf->exp %= max_exp;
	}
	new = buf->m1 >> (31 - buf->exp);
	if (is_signed && buf->sign)
		new = -new;
	return new;
}

long sf_cfi4(SINGLE f)
{
	EXTEND buf;

	extend(&f, &buf, sizeof(SINGLE));
	return ftol(&buf, 4, 1);
}

unsigned long sf_cfu4(SINGLE f)
{
	EXTEND buf;

	extend(&f, &buf, sizeof(SINGLE));
	return ftol(&buf, 4, 0);
}

long sf_cfi8x(DOUBLE* d)
{
	EXTEND buf;

	extend(&d->d[0], &buf, sizeof(DOUBLE));
	return ftol(&buf, 8, 1);
}

unsigned long sf_cfu8x(DOUBLE* d)
{
	EXTEND buf;

	extend(&d->d[0], &buf, sizeof(DOUBLE));
	return ftol(&buf, 8, 0);
}

/* float to double and back */
void sf_cff48x(SINGLE src, DOUBLE* res)
{
	EXTEND buf;

	extend(&src, &buf, sizeof(SINGLE));
	compact(&buf, &res->d[0], sizeof(DOUBLE));
}

SINGLE sf_cff84x(DOUBLE* src)
{
	EXTEND buf;
	SINGLE result;

	extend(&src->d[0], &buf, sizeof(DOUBLE));
	compact(&buf, &result, sizeof(SINGLE));
	return result;
}

/* doubles, in place: *a = *a op *b */
void sf_adf8(DOUBLE* a, DOUBLE* b)
{
	EXTEND e1, e2;

	if (a->d[0] == 0 && a->d[1] == 0) {
		*a = *b;
		return;
	}
	if (b->d[0] == 0 && b->d[1] == 0)
		return;
	extend(&a->d[0], &e1, sizeof(DOUBLE));
	extend(&b->d[0], &e2, sizeof(DOUBLE));
	add_ext(&e1, &e2);
	compact(&e1, &a->d[0], sizeof(DOUBLE));
}

void sf_sbf8(DOUBLE* a, DOUBLE* b)
{
	EXTEND e1, e2;

	if (b->d[0] == 0 && b->d[1] == 0)
		return;
	extend(&a->d[0], &e1, sizeof(DOUBLE));
	extend(&b->d[0], &e2, sizeof(DOUBLE));
	sub_ext(&e1, &e2);
	compact(&e1, &a->d[0], sizeof(DOUBLE));
}

void sf_mlf8(DOUBLE* a, DOUBLE* b)
{
	EXTEND e1, e2;

	extend(&a->d[0], &e1, sizeof(DOUBLE));
	extend(&b->d[0], &e2, sizeof(DOUBLE));
	mul_ext(&e1, &e2);
	compact(&e1, &a->d[0], sizeof(DOUBLE));
}

void sf_dvf8(DOUBLE* a, DOUBLE* b)
{
	EXTEND e1, e2;

	extend(&a->d[0], &e1, sizeof(DOUBLE));
	extend(&b->d[0], &e2, sizeof(DOUBLE));
	div_ext(&e1, &e2);
	compact(&e1, &a->d[0], sizeof(DOUBLE));
}

void sf_ngf8(DOUBLE* a)
{
	if (a->d[0] != 0 || a->d[1] != 0)
		a->d[1] ^= 0x80000000UL;
}

/* the comparison of doubles by address, for the wrappers */
int sf_cmf8x(DOUBLE* a, DOUBLE* b)
{
	return cmf8(*b, *a);
}
